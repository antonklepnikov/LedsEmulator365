#ifndef COMMON_AK_H
#define COMMON_AK_H


////////////////////////////////////////////////////////////////////////////////
/// CONSTANTS //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// NUMERIC CONSTANTS //////////////////////////////////////////////////////////

enum {

	/// TCP-Server /////////////////////////////////////////////
	SELECT_DELAY_SEC = 0,
    SELECT_DELAY_USEC = 50000,
    
    TCP_LINE_MAX_LENGTH = 1023,
    TCP_QLEN_FOR_LISTEN = 16,
    

    /// PULT ////////////////////////////// ! DON'T TOUCH ! ////
    NUM_DIG_BUTTONS = 9,
    NUM_ALL_BUTTONS = 14,
    B_1 = 0, B_2 = 1, B_3 = 2, B_4 = 3,
    B_5 = 4, B_6 = 5, B_7 = 6, B_8 = 7, B_9 = 8,
    B_UP = 9, B_LEFT = 10, B_OK = 11, B_RIGHT = 12, B_DOWN = 13,
    
  
    /// LEDS ///////////////////////////////////////////////////
    NUM_LEDS = 75,
    NUM_COLUMNS = 15,
    ITEM_SIZE = 50,
    BREAKUP = 10,

    FONT_SIZE_DIGIT_BUTTON = 22,
    FONT_SIZE_CONTROL_BUTTON = 17,

    MAX_BRIGHT = 100,
    MIN_BRIGHT = 0,
    INIT_BRIGHT = 100,
    STEP_BRIGHT = 25,
    PRECISION_BRIGHT = 100

};


/// TEXT CONSTANTS /////////////////////////////////////////////////////////////

static const char SERVER_WELCOME[] =
	"SRV: Hello, you are connected to LEDCore!\nCLI: ";
static const char SERVER_NEW_LINE[] = "\nCLI: ";

static const char CLIENT_EXIT[] = "quit";
static const char CLIENT_MODE_1[] = "m1";
static const char CLIENT_MODE_2[] = "m2";
static const char CLIENT_MODE_3[] = "m3";
static const char CLIENT_MODE_4[] = "m4";
static const char CLIENT_MODE_5[] = "m5";
static const char CLIENT_MODE_6[] = "m6";
static const char CLIENT_MODE_7[] = "m7";
static const char CLIENT_MODE_8[] = "m8";
static const char CLIENT_MODE_9[] = "m9";
static const char CLIENT_UP[] = "up";
static const char CLIENT_DOWN[] = "down";
static const char CLIENT_RIGHT[] = "right";
static const char CLIENT_LEFT[] = "left";
static const char CLIENT_OK[] = "ok";


static const char SERVER_LOG_FILE[] = "le365_tcp.log";


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
