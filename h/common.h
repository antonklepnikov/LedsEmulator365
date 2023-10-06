#ifndef COMMON_AK_H
#define COMMON_AK_H

#include <string>


////////////////////////////////////////////////////////////////////////////////
/// CONSTANTS //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace le365const {

/// NUMERIC CONSTANTS //////////////////////////////////////////////////////////
	
	/// TCP-Server /////////////////////////////////////////////////////////////
	inline constexpr int select_delay_sec{ 0 };
    inline constexpr int select_delay_usec{ 50000 };
    
    inline constexpr int tcp_line_max_length{ 1023 };
    inline constexpr int tcp_qlen_for_listen{ 16 };

    /// PULT ////////////////////////////////////////////// ! DON'T TOUCH ! ////
    inline constexpr int num_dig_buttons{ 9 };
    inline constexpr int num_all_buttons{ 14 };
    
    inline constexpr int font_size_digit_button{ 22 };
    inline constexpr int font_size_control_button{ 17 };
    
    inline constexpr int b_1{ 0 };
    inline constexpr int b_2{ 1 };
    inline constexpr int b_3{ 2 };
    inline constexpr int b_4{ 3 };
    inline constexpr int b_5{ 4 };
    inline constexpr int b_6{ 5 };
    inline constexpr int b_7{ 6 };
    inline constexpr int b_8{ 7 }; 
    inline constexpr int b_9{ 8 };
    inline constexpr int b_up{ 9 };
    inline constexpr int b_left{ 10 };
    inline constexpr int b_ok{ 11 };
    inline constexpr int b_right{ 12 };
    inline constexpr int b_down{ 13 };

    /// LEDS ///////////////////////////////////////////////////////////////////
    inline constexpr int num_leds{ 75 };
    
    inline constexpr int num_columns{ 15 };
    inline constexpr int item_size{ 50 };
    inline constexpr int breakup{ 10 };
    
    inline constexpr int max_bright{ 100 };
    inline constexpr int min_bright{ 0 };
    inline constexpr int init_bright{ 100 };
    inline constexpr int step_bright{ 25 };
    inline constexpr int precision_bright{ 100 };


/// TEXT CONSTANTS /////////////////////////////////////////////////////////////

    /// SERVER /////////////////////////////////////////////////////////////////
    inline constexpr std::string_view server_welcome{ 
        "SRV: Hello, you are connected to LEDCore!\nCLI: " };
    inline constexpr std::string_view server_new_line{ "\nCLI: " };
    
    inline constexpr std::string_view client_exit       { "quit" };
    inline constexpr std::string_view client_mode_1     { "m1" };
    inline constexpr std::string_view client_mode_2     { "m2" };
    inline constexpr std::string_view client_mode_3     { "m3" };
    inline constexpr std::string_view client_mode_4     { "m4" };
    inline constexpr std::string_view client_mode_5     { "m5" };
    inline constexpr std::string_view client_mode_6     { "m6" };
    inline constexpr std::string_view client_mode_7     { "m7" };
    inline constexpr std::string_view client_mode_8     { "m8" };
    inline constexpr std::string_view client_mode_9     { "m9" };
    inline constexpr std::string_view client_up         { "up" };
    inline constexpr std::string_view client_down       { "down" };
    inline constexpr std::string_view client_right      { "right" };
    inline constexpr std::string_view client_left       { "left" };
    inline constexpr std::string_view client_ok         { "ok" };
    
    /// LOGGING ////////////////////////////////////////////////////////////////
    inline constexpr std::string_view server_log_file{ "le365_tcp.log" };
}


////////////////////////////////////////////////////////////////////////////////
/// USER-DEFINED TYPES /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum enum_mode {
  mode_null = -1, mode_stop = 0,
  mode_1 = 1, mode_2 = 2, mode_3 = 3,
  mode_4 = 4, mode_5 = 5, mode_6 = 6,
  mode_7 = 7, mode_8 = 8, mode_9 = 9
};

struct RGBLed {
    int r{};
    int g{};
    int b{};
};


////////////////////////////////////////////////////////////////////////////////

#endif
