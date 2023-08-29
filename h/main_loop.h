#ifndef SELECTOR_AK_H
#define SELECTOR_AK_H



////////////////////////////////////////////////////////////////////////////////

/***
     Main loop of the programm
                               ***/


////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "tcp_srv.h"
#include "led_core.h"
#include "oofl.h"

#include <array>


////////////////////////////////////////////////////////////////////////////////
/// CLASS: MainLoop ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class MainLoop {
    friend class Window365;
private:
    FdServer *tcps;
    LEDCore *core;
    
    std::array<OOFLButton*, NUM_ALL_BUTTONS> buttons;
    
    ModeRainbow 			rainbow;									   // 1
    ModeRainbowMeteor 		rainbowMeteor;      						   // 2
    ModeRainbowGlitter 		rainbowGlitter; 							   // 3
    ModeStars				stars;  			                           // 4	  
    ModeRunningDots			runningDots;								   // 5
    ModePacifica            pacifica;                                      // 6
    ModeRGB                 rgb;                                           // 7 
    ModeCMYK                cmyk;                                          // 8
    ModeWhite               white;                                         // 9  
    ModeStop 				stop;                                          // O

    void ModeStep();

public:
    MainLoop(FdServer *s, LEDCore *c) 
    	: tcps(s), core(c), buttons(),
    	  rainbow(c, mode_1), rainbowMeteor(c, mode_2), 
    	  rainbowGlitter(c, mode_3), stars(c, mode_4), runningDots(c, mode_5), 
    	  pacifica(c, mode_6), rgb(c, mode_7), cmyk(c, mode_8), 
    	  white(c, mode_9), stop(c, mode_stop) {}

    // No copying and assignment:
    MainLoop(const MainLoop&) = delete;
    MainLoop& operator=(const MainLoop&) = delete;
    
    int Run();
};


////////////////////////////////////////////////////////////////////////////////

#endif
