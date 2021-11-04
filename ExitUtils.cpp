#include <algorithm>
#include <cxxabi.h>   // for __cxa_demangle
#include <dlfcn.h>    // for dladdr
#include <execinfo.h> // for backtrace
#include <iomanip>
#include <iostream>
#include <mach-o/dyld.h> // for _dyld_get_image_header
#include <sstream>
#include <string>
#include <string_view>

#include "ExitUtils.hpp"
#include "LocationInfo.hpp"
#include "System.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

bool print_full_path_in_stack_trace = false;

static std::string process_name_with_path_impl() {
    std::string buf;
    std::uint32_t size = 128;
    buf.resize(size);
    while (_NSGetExecutablePath(&buf[0],&size))
	buf.resize(size);
    buf.resize(std::strlen(&buf[0]));
    return buf;//std::filesystem::path(buf).string();
}

std::string const& process_name_with_path() {
    static std::string name = process_name_with_path_impl();
    return name;
}

static void print_old_stack_trace(std::ostream& os, void** call_stack, SZ skip_frames, SZ size) {
    C** symbol_list = backtrace_symbols(call_stack,static_cast<int>(size));
    C* demangle_buffer = nullptr;
    SZ len;
    for (SZ i = skip_frames; i < size; ++i) {
	Dl_info info;
	C const * name_to_print = symbol_list[i];
	if (dladdr(call_stack[i],&info) && info.dli_sname != nullptr) {
	    name_to_print = info.dli_sname;
	    SI status;
	    C* demangle_result = abi::__cxa_demangle(
		info.dli_sname,
		demangle_buffer,
		&len,
		&status);
	    if (status == 0)
		name_to_print = demangle_buffer = demangle_result;
	}
	os << std::setw(2) << i-skip_frames << ": " << name_to_print << "\n";
    }
    if (demangle_buffer != nullptr)
	free(demangle_buffer);
}

void print_stack_trace(std::ostream& os, SZ skip_frames) {
    auto const width = os.width();
    
    std::ostringstream command;
    command << "atos"; // Mac utility for finding demangled name, file, and line information
    if (print_full_path_in_stack_trace)
	command << " -fullPath"; // give full paths to files rather than just names
    command << " -o " << process_name_with_path(); // executable 
    command << " -l " << _dyld_get_image_header(0); // load address
    
    // skip this frame too
    ++skip_frames;
    constexpr SZ max_print = 256;
    constexpr SZ dummy_end = 1; // last frame is unknown
    constexpr SZ max_skip_frames = 255;
    if (skip_frames > max_skip_frames)
	skip_frames = max_skip_frames;
    SZ size = max_print + skip_frames + dummy_end;
    void * call_stack[max_skip_frames + max_print + dummy_end];
    size = static_cast<SZ>(backtrace(call_stack,static_cast<int>(size)));
    size = size>=dummy_end ? size-dummy_end : 0;
    for (SZ i = skip_frames; i < size; ++i)
	command << " " << call_stack[i];
    auto const decoded_stack = system_call(command.str());
    
    if (!decoded_stack) {
	os << "Stack trace using old version because atos failed due to:\n" << decoded_stack.cause() << "\n";
	print_old_stack_trace(os,call_stack,skip_frames,size);
    } else {
	os << "Stack trace:\n";
	std::string const& resolved = decoded_stack->output;
	std::vector<std::string> result;
	result.emplace_back();
	for (char const c : resolved) {
	    if (c == '\n') {
		result.emplace_back();
	    } else {
		result.back() += c;
	    }
	}
	if (result.back().size() == 0)
	    result.pop_back();

	// go through once without printing so we increase max sizes of fields to align them all
	for (SZ i = 0; i < result.size(); ++i)
	    LocationInfo{i,result[i]};

	LocationInfo::print_header(os);
	os << "\n";
	for (SZ i = 0; i < result.size(); ++i)
	    os << LocationInfo(i,result[i]) << "\n";
	LocationInfo::reset();
    }
    os << std::flush;
    os.width(width);
}

std::string stack_trace(SZ skip_frames) {
    std::stringstream ss;
    print_stack_trace(ss,1+skip_frames);
    return ss.str();
}

[[noreturn]] static void stack_trace_handler(int s) {
    std::cout << "Signal " << s << " caught. Stack trace:" << std::endl;
    print_stack_trace();
    exit(1);
}

static auto discard1 = signal(SIGINT,stack_trace_handler);
static auto discard2 = signal(SIGSEGV,stack_trace_handler);

/*
  static void stack_trace_handler(int,struct __siginfo*, void*) {
  std::cout << "?" << std::endl;
  print_stack_trace();
  exit(1);
  }
  static int setup_handler() {
  static struct sigaction action{};
  action.sa_flags = SA_SIGINFO;
  action.sa_sigaction = stack_trace_handler;
  sigaction(SIGSEGV,&action,nullptr);
  sigaction(SIGINT,&action,nullptr);
  return 0;
  }
  static int unused = setup_handler();
*/

// possible witchcraft to overtake the c++ throw ABI:
// https://monoinfinito.wordpress.com/category/programming/c/exceptions/page/1/
