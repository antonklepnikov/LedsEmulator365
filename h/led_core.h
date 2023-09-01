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

#include <array>
#include <bitset>


////////////////////////////////////////////////////////////////////////////////
/// CLASS: LEDCore /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class LEDCore {
    friend class CorePultInterface;
    friend class Window365;
private:
    Timer mainTimer;    
    Timer waitTimer;
    std::array<OOFLBox*, NUM_LEDS> leds;
    std::array<CRGB, NUM_LEDS> fstleds;
    enum_mode currentMode{ mode_null };
    enum_mode befStopMode{ mode_null };
    bool isStop{ false };
    bool core_quit_flag{ false };
    bool in_waiting{ false };
    double wait_for{ 0.0 };    
    int bright{ INIT_BRIGHT };
    const int k_min_num_mode{ 1 };
    const int k_max_num_mode{ 9 };
public:
	// Initializing the pseudo-random number generator in the constructor:
    LEDCore() : mainTimer(), waitTimer(), leds(), fstleds() { prandom_init(); }

    // No copying and assignment:
    LEDCore(const LEDCore&) = delete;
    LEDCore& operator=(const LEDCore&) = delete;

    CRGB& operator[](int idx) { return fstleds.at(static_cast<size_t>(idx)); }
    CRGB* GetFstleds() { return fstleds.data(); }
 
    int GetBright() const { return bright; }
    enum_mode GetMode() const { return currentMode; }
    uint32_t GetMillis() const { return mainTimer.Elapsed() * 1000;  }
 
    void SetMode(enum_mode m);
    void BrightUp();
    void BrightDown();
    void StopMode();
    void PrevMode();
    void NextMode();
    
    void SetBright(int b) { bright = b; }
    void Show();
    void Clear();
    void Waits(double sec);
    void Fill(int r, int g, int b);
    
    void FltkStep() { if(!Fl::check()) { core_quit_flag = true; } }
    bool CoreRun() const { return !core_quit_flag; }
    
    void SetLongWait(double sec);
    void ClearLongWait();
    bool NoLongWait();
};

class CorePultInterface {
private:
	LEDCore *core;
public:
	CorePultInterface() = default;
	
	void Init(LEDCore *cp) { if(cp) { core = cp; } }
	
	void Mode(enum_mode m) 	{	core->SetMode(m);	}
	void Ok()				{	core->StopMode();	}
	void Up() 				{	core->BrightUp();	}
	void Down()				{	core->BrightDown();	}
	void Left() 			{	core->PrevMode();	}
	void Right() 			{	core->NextMode();	}
	
	// No copying and assignment:
    CorePultInterface(const CorePultInterface&) = delete;
    CorePultInterface& operator=(const CorePultInterface&) = delete;
};


////////////////////////////////////////////////////////////////////////////////
/// SUPPORT FUNCTIONS //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T, std::size_t size>
void fill_in_turn(LEDCore *core, 
                  const std::array<T, size> &carr, 
                  size_t shift_color = 0);


////////////////////////////////////////////////////////////////////////////////
/// CLASS: Pattern and its heirs ("stripe" operation modes) ////////////////////
////////////////////////////////////////////////////////////////////////////////

class Pattern {
protected:
    LEDCore* core;
    enum_mode mode;
    std::array<CRGB, NUM_LEDS> ledDump;
    virtual void PatternStep() = 0;
public:
    Pattern(LEDCore *c, enum_mode m) : core(c), mode(m), ledDump()
    {
    	for(auto& l : ledDump) { l = CRGB::Black; }
    }
    
    // No copying and assignment:
    Pattern(const Pattern&) = delete;
    Pattern& operator=(const Pattern&) = delete;
    
    void Step() {
        if(core->NoLongWait()) { 
        	LoadState();
        	PatternStep(); 
        	core->Show();
        	core->FltkStep();
        	SaveState();
        }
    }
    
    void SaveState();
    void LoadState();
    
    virtual ~Pattern() = default;
};

/////////////////////////////////////
/// class ModeStop //////////////////
/////////////////////////////////////
class ModeStop : public Pattern {
protected:
    virtual void PatternStep();
public:
    ModeStop(LEDCore *c, enum_mode m) : Pattern(c, m) {}
    virtual ~ModeStop() = default;
};

/////////////////////////////////////
/// class ModeRainbow ///////////////
/////////////////////////////////////
class ModeRainbow : public Pattern {
protected:
	const double k_delay{ 0.1 };
	int k_deltaHue{ 30 };
	const int k_stepHue{ 5 };
	int initHue;
    virtual void PatternStep();
public:
    ModeRainbow(LEDCore *c, enum_mode m) : Pattern(c, m), initHue(0) {}
    virtual ~ModeRainbow() = default;
};

/////////////////////////////////////
/// class ModeRainbowMeteor /////////
/////////////////////////////////////
class ModeRainbowMeteor : public Pattern {
protected:
    const double k_delay{ 0.03 };
    int hue;
    int it;
    bool dirForward;
    void FadeAll();
    virtual void PatternStep();
public:
    ModeRainbowMeteor(LEDCore *c, enum_mode m) 
    	: Pattern(c, m), hue(150), it(0), dirForward(true) {}
    virtual ~ModeRainbowMeteor() = default;
};

/////////////////////////////////////
/// class ModeRainbowGlitter ////////
/////////////////////////////////////
class ModeRainbowGlitter : public Pattern {
protected:
	const double k_delay{ 0.1 };
	const int k_chanceGlitter1{ 30 };
	const int k_chanceGlitter2{ 50 };
	const int k_deltaHue{ 5 };
	const int k_stepHue{ 5 };
	int initHue;
	void AddGlitter(int chance);
	virtual void PatternStep();
public:
	ModeRainbowGlitter(LEDCore *c, enum_mode m) : Pattern(c, m), initHue(0) {}
	virtual ~ModeRainbowGlitter() = default;
};

/////////////////////////////////////
/// class ModeStars /////////////////
/////////////////////////////////////
class ModeStars : public Pattern {
protected:
	const double k_delay{ 0.1 };
	std::bitset<NUM_LEDS> stars;
	size_t random_led;
	int star_count;
	bool filling;
	CRGB base_col{ CRGB::White };
	CRGB star_col{ CRGB::Gold };
	virtual void PatternStep();
public:
	ModeStars(LEDCore *c, enum_mode m) 
	    : Pattern(c, m), stars(),
	      random_led(0), star_count(0), filling(true) {}
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
	size_t max_shift;
    size_t shift;
	virtual void PatternStep();
public:
	ModeRunningDots(LEDCore *c, enum_mode m) 
	    : Pattern(c, m), max_shift(0), shift(0)
	{
	    max_shift = carr.size();
	    shift = max_shift;
	}
	virtual ~ModeRunningDots() = default;
};

/////////////////////////////////////
/// class ModePacifica //////////////
/////////////////////////////////////

class ModePacifica : public Pattern {
protected:
	const double k_delay{ 0.02 };
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
	// Add one layer of waves into the led array
	void pacifica_one_layer(CRGBPalette16& p, 
	          			    uint16_t cistart, uint16_t wavescale, 
							uint8_t bri, uint16_t ioff);
	// Add extra 'white' to areas 
	// where the four layers of light have lined up brightly
	void pacifica_add_whitecaps();
	// Deepen the blues and greens
	void pacifica_deepen_colors();
	virtual void PatternStep();
public:
	ModePacifica(LEDCore *c, enum_mode m) : Pattern(c, m) {}
	virtual ~ModePacifica() = default;
};

/////////////////////////////////////
/// class ModeRGB ///////////////////
/////////////////////////////////////
class ModeRGB : public Pattern {
protected:
	const double k_delay{ 0.5 };
	std::array<CRGB, 3> carr{ CRGB::Red, CRGB::Green, CRGB::Blue };
	virtual void PatternStep();
public:
	ModeRGB(LEDCore *c, enum_mode m) : Pattern(c, m) {}
	virtual ~ModeRGB() = default;
};

/////////////////////////////////////
/// class ModeCMYK //////////////////
/////////////////////////////////////
class ModeCMYK : public Pattern {
protected:
	const double k_delay{ 0.5 };
	std::array<CRGB, 4> carr{ CRGB::Cyan, CRGB::Magenta, 
	                          CRGB::Yellow, CRGB::Black };
	virtual void PatternStep();
public:
	ModeCMYK(LEDCore *c, enum_mode m) : Pattern(c, m) {}
	virtual ~ModeCMYK() = default;
};

/////////////////////////////////////
/// class ModeWhite /////////////////
/////////////////////////////////////
class ModeWhite : public Pattern {
protected:
	const double k_delay{ 0.5 };
	virtual void PatternStep();
public:
	ModeWhite(LEDCore *c, enum_mode m) : Pattern(c, m) {}
	virtual ~ModeWhite() = default;
};


////////////////////////////////////////////////////////////////////////////////

#endif