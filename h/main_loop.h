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
    TcpServer *srv;
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
    MainLoop(TcpServer *sp, LEDCore *cp) 
    	: srv(sp), core(cp), buttons(),
    	  rainbow(cp, mode_1), rainbowMeteor(cp, mode_2), 
    	  rainbowGlitter(cp, mode_3), stars(cp, mode_4), runningDots(cp, mode_5), 
    	  pacifica(cp, mode_6), rgb(cp, mode_7), cmyk(cp, mode_8), 
    	  white(cp, mode_9), stop(cp, mode_stop) {}

    // No copying and assignment:
    MainLoop(const MainLoop&) = delete;
    MainLoop& operator=(const MainLoop&) = delete;
    
    void Run();
};


////////////////////////////////////////////////////////////////////////////////

#endif
