#pragma once

#include <ostream>
#include <string>

struct LocationInfo {
  private:
    std::size_t frame;
    std::string_view file_info;
    std::string_view line_info;
    std::string fcn_info;
    static int const max_frm_info = 5;
    static int max_file_info;
    static int max_line_info;
    static std::string function_name(std::string_view const & atos);
    friend std::ostream& operator<<(std::ostream& os, LocationInfo const & info);
  public:
    LocationInfo(std::size_t frame_num, std::string const & atos_output);
    static std::ostream& print_header(std::ostream& os);
    static void reset();
};
std::ostream& operator<<(std::ostream& os, LocationInfo const & info);

