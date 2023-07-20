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

#include <array>

#include <unistd.h>


/// TCP-SERVER PART (temporary) ////////////////////////////////////////////////

class FdHandler {
private:
	int fd;
	bool own_fd;
public:
	FdHandler(int a_fd, bool own = true) : fd(a_fd), own_fd(own) {}
	virtual ~FdHandler() { if(own_fd) { close(fd); } }
	virtual void Handle(bool r, bool w) = 0;
	int GetFd() const { return fd; }
	virtual bool WantRead() const { return true; }
	virtual bool WantWrite() const { return false; }
};

class FdSelector {
private:
public:
};


////////////////////////////////////////////////////////////////////////////////
/// CLASS: Selector ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Selector {
    friend class Window365;
private:
    bool quit_flag;
    
    LEDCore *core;
    std::array<OOFLButton*, NUM_ALL_BUTTONS> buttons;
    
    ModeRainbow 			rainbow{};									   // 1
    ModeRainbowMeteor 		rainbowMeteor{};      						   // 2
    ModeRainbowGlitter 		rainbowGlitter{};							   // 3
    ModeStars				stars{};			                           // 4	  
    ModeRunningDots			runningDots{};								   // 5
    ModePacifica            pacifica{};                                    // 6
    ModeRGB                 rgb{};                                         // 7 
    ModeCMYK                cmyk{};                                        // 8
    ModeWhite               white{};                                       // 9  
    ModeStop 				stop{};                                        // O

    void Select();

public:
    Selector(LEDCore *c) : quit_flag(false), core(c), buttons{} {}

    // No copying and assignment:
    Selector(const Selector&) = delete;
    Selector& operator=(const Selector&) = delete;
	void BreakLoop() { quit_flag = true; }
    void Run();
};


////////////////////////////////////////////////////////////////////////////////

#endif
