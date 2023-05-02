#ifndef LED_CORE_AK_H
#define LED_CORE_AK_H


////////////////////////////////////////////////////////////////////////////////

/***
    Everything related to the "stripe"
                                      ***/


////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "fastled_port.h"
#include "oofl.h"
#include "timer.h"

#include <FL/Fl.H>

#include <vector>
#include <array>


////////////////////////////////////////////////////////////////////////////////
/// CLASS: LEDCore /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class LEDCore {
    friend class Window365;
private:
    OOFLBox *leds[NUM_LEDS];
    CRGB fstleds[NUM_LEDS];
    int bright{INIT_BRIGHT};
    enum_mode mode{mode_null};
public:
	// Initializing the pseudo-random number generator in the constructor:
    LEDCore() { prandom_init(); }
    // No copying and assignment:
    LEDCore(const LEDCore&) = delete;
    LEDCore& operator=(const LEDCore&) = delete;
    int GetBright() const { return bright; }
    void SetBright(int b) { bright = b; }
    enum_mode GetMode() const { return mode; }
    void SetMode(enum_mode m) { mode = m; }
    CRGB& operator[](int idx) { return fstleds[idx]; }
    CRGB* GetFstleds() { return fstleds; }
    void Show();
    void Clear();
    void Waits(double sec);
    void Fill(int r, int g, int b);    
};


////////////////////////////////////////////////////////////////////////////////
/// CLASS: Pattern and its heirs ("stripe" operation modes) ////////////////////
////////////////////////////////////////////////////////////////////////////////

class Pattern {
protected:
    LEDCore* core{nullptr};
    enum_mode curr{mode_null};
    bool ready{false};
    virtual void IntLoop() = 0;
public:
    Pattern() = default;
    // No copying and assignment:
    Pattern(const Pattern&) = delete;
    Pattern& operator=(const Pattern&) = delete;
    void Init(LEDCore *c, enum_mode m) { core = c; curr = m; ready = true; }
    bool GetReady() const { return ready; }
    void Enable() {
        if(!ready) {
            throw std::logic_error("Pattern initialization is expected!"); 
        }
        IntLoop();
    }
    virtual ~Pattern() = default;
};

/////////////////////////////////////
/// class ModeStop //////////////////
/////////////////////////////////////
class ModeStop : public Pattern {
protected:
    virtual void IntLoop();
public:
    ModeStop() = default;
    virtual ~ModeStop() = default;
};

/////////////////////////////////////
/// class ModeRainbow ///////////////
/////////////////////////////////////
class ModeRainbow : public Pattern {
protected:
    virtual void IntLoop();
public:
    ModeRainbow() = default;
    virtual ~ModeRainbow() = default;
};

/////////////////////////////////////
/// class ModeRainbowMeteor /////////
/////////////////////////////////////
class ModeRainbowMeteor : public Pattern {
protected:
    void FadeAll();
    virtual void IntLoop();
public:
    ModeRainbowMeteor() = default;
    virtual ~ModeRainbowMeteor() = default;
};

/////////////////////////////////////
/// class ModeRainbowGlitter ////////
/////////////////////////////////////
class ModeRainbowGlitter : public Pattern {
protected:
	void AddGlitter(int chance);
	virtual void IntLoop();
public:
	ModeRainbowGlitter() = default;
	virtual ~ModeRainbowGlitter() = default;
};

/////////////////////////////////////
/// class ModeStars /////////////////
/////////////////////////////////////
class ModeStars : public Pattern {
protected:
	CRGB base_col{ CRGB::White };
	CRGB star_col{ CRGB::Gold };
	const double k_delay{ 0.05 };
	std::vector<bool> barr;
	virtual void IntLoop();
public:
	ModeStars() : barr(NUM_LEDS) {}
	virtual ~ModeStars() = default;
};

/////////////////////////////////////
/// class ModeRunningDots ///////////
/////////////////////////////////////
class ModeRunningDots : public Pattern {
protected:
	const double k_delay{ 0.5 };
	std::array<CRGB, 5> carr{ CRGB::Blue, CRGB::Pink, CRGB::Green, 
	                          CRGB::Gold, CRGB::Red };
	virtual void IntLoop();
public:
	ModeRunningDots() = default;
	virtual ~ModeRunningDots() = default;
};

/////////////////////////////////////
/// class ModePacifica //////////////
/////////////////////////////////////
class ModePacifica : public Pattern {
protected:
	const double k_delay{ 0.5 };
	virtual void IntLoop();
public:
	ModePacifica() = default;
	virtual ~ModePacifica() = default;
};


////////////////////////////////////////////////////////////////////////////////

#endif