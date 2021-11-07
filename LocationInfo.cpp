#include "LocationInfo.hpp"

#include <cstring>
#include <iomanip>
#include <vector>

int LocationInfo::max_file_info = 5;
int LocationInfo::max_line_info = 4;

void LocationInfo::reset() {
    max_file_info = 5;
    max_line_info = 4;
}

static std::string remove_paren_contents(std::string_view const& in) {
    // Swallow everything within (), excluding the ()
    // If there were contents, replace with ...
    std::string out;
    std::size_t num_open_paren = 0;
    for (char const& c : in) {
	if (c == '(') {
	    out += c;
	    ++num_open_paren;
	} else if (num_open_paren == 0) {
	    out += c;
	} else if (c == ')') {
	    if ((&c)[-1] != '(')
		out += "...";
	    --num_open_paren;
	    out += c;
	}
    }
    return out;
}

static std::string remove_extra_space(std::string_view const& in) {
    std::string out;
    bool prev_space = true;
    for (char const c : in) {
	bool cur_space = c == ' ';
	if (!cur_space || !prev_space)
	    out += c;
	prev_space = cur_space;
    }
    if (out.size() > 0 && out.back() == ' ')
	out.pop_back();
    return out;
}

std::string LocationInfo::function_name(std::string_view const& atos) {
    std::string no_paren_contents = remove_paren_contents(atos);
    std::string no_extra_space = remove_extra_space(no_paren_contents);
    return no_extra_space;
}

LocationInfo::LocationInfo(std::size_t frame_num, std::string const& atos_output)
    : frame(frame_num) {
    std::size_t const sz = atos_output.size();
    std::size_t const open = atos_output.find_last_of('(');
    std::size_t const colon = atos_output.find_last_of(':');
    if (atos_output.size() > 0 &&
	atos_output.back() == ')' &&
	colon != std::string::npos &&
	open < colon) {
	file_info = std::string_view{atos_output}.substr(open+1,colon-open);
	line_info = std::string_view{atos_output}.substr(colon+1,sz-colon-2);
	std::size_t useless_start = open;
	// (in the_executable) isn't useful - remove it
	constexpr char const * remove = " (in ";
	if (std::size_t prev_open = atos_output.find_last_of('(',open-1);
	    prev_open != std::string::npos &&
	    prev_open < useless_start &&
	    prev_open > 0 &&
	    std::string_view{atos_output}.substr(prev_open-1,strlen(remove)) == remove)
	    useless_start = prev_open-1;
	fcn_info = function_name(std::string_view{atos_output}.substr(0,useless_start));
    } else {
	file_info = "<unknown>";
	fcn_info = atos_output;
    }
    max_file_info = std::max(max_file_info,static_cast<int>(file_info.size()));
    max_line_info = std::max(max_line_info,static_cast<int>(line_info.size()));
}

std::ostream& LocationInfo::print_header(std::ostream& os) {
    auto const width = os.width();
    os << std::setw(max_frm_info) << "Frame" << " ";
    os << std::setw(max_file_info) << "File:";
    os << std::setw(max_line_info) << "Line" << " ";
    os << "Function";
    os.width(width);
    return os;
}

std::ostream& operator<<(std::ostream& os, LocationInfo const& info) {
    auto const width = os.width();
    os << std::setw(LocationInfo::max_frm_info) << info.frame    << " ";
    os << std::setw(LocationInfo::max_file_info) << info.file_info;
    os << std::setw(LocationInfo::max_line_info) << info.line_info << " ";
    os << info.fcn_info;
    os.width(width);
    return os;
}
