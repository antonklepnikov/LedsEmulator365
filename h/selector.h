#ifndef SELECTOR_AK_H
#define SELECTOR_AK_H


////////////////////////////////////////////////////////////////////////////////

/***
    Managing modes
                  ***/


////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "led_core.h"
#include "oofl.h"


////////////////////////////////////////////////////////////////////////////////
/// CLASS: Selector ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Selector {
    friend class Window365;
private:
    LEDCore *core;
    OOFLButton *buttons[NUM_ALL_BUTTONS];
    ModeRainbow 			rainbow{};									   // 1
    ModeRainbowMeteor 		rainbowMeteor{};      						   // 2
    ModeRainbowGlitter 		rainbowGlitter{};							   // 3
    ModeStars				stars{};			                           // 4	  
    ModeRunningDots			runningDots{};								   // 5
    ModePacifica            pacifica{};                                    // 6
    ModeStop 				stop{};                                        // O
    void Select();
public:
    Selector(LEDCore *c) : core(c) {}
    // No copying and assignment:
    Selector(const Selector&) = delete;
    Selector& operator=(const Selector&) = delete;
    void Run();
};


////////////////////////////////////////////////////////////////////////////////

#endif
