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

// 	FROM FASTLED_PORT:
//	- GET_MILLIS()
//	- CRGBPalette16
//	+ scale8
//	- scale16
//	- sin8()
//	- sin16()
//	- beat8()
//	- beat16()
//	- beatsin8()
//	- beatsin16()
//	- beatsin88()
//  - getAverageLight()

class ModePacifica : public Pattern {
protected:
	const double k_delay{ 0.02 };
	
	/*
	CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 
      0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 
      0x00003B, 0x000046, 0x14554B, 0x28AA50 };
	CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 
      0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 
      0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
	CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 
      0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 
      0x001C70, 0x002080, 0x1040BF, 0x2060FF };
	*/
	
	/*
	// Add one layer of waves into the led array
	void pacifica_one_layer(CRGBPalette16& p, 
	          			    uint16_t cistart, uint16_t wavescale, 
							uint8_t bri, uint16_t ioff);
	// Add extra 'white' to areas 
	// where the four layers of light have lined up brightly
	void pacifica_add_whitecaps();
	// Deepen the blues and greens
	void pacifica_deepen_colors();
	*/
	
	virtual void IntLoop();
public:
	ModePacifica() = default;
	virtual ~ModePacifica() = default;
};


////////////////////////////////////////////////////////////////////////////////

#endif