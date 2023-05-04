////////////////////////////////////////////////////////////////////////////////

/***
    IMPLEMENTATION:
    Everything related to the "stripe"
                                      ***/
                                      

////////////////////////////////////////////////////////////////////////////////                                      

#include "led_core.h"


////////////////////////////////////////////////////////////////////////////////
/// LEDCore ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void LEDCore::StopMode() 
{
    if(stopped) {
        stopped = false;
        currentMode = savedMode;
    } else {
        stopped = true;
        savedMode = currentMode;
        currentMode = mode_stop;
    }
} 

void LEDCore::PrevMode() 
{ 
    if(currentMode != mode_null) { 
        int current = (static_cast<int>(currentMode));
        if(current <= k_min_num_mode) { 
            currentMode = static_cast<enum_mode>(k_max_num_mode);
        } else {     
            currentMode = static_cast<enum_mode>(current - 1);
        }
    } 
}
    
void LEDCore::NextMode() 
{
    if(currentMode != mode_null) { 
        int current = (static_cast<int>(currentMode));
        if(current >= k_max_num_mode) { 
            currentMode = static_cast<enum_mode>(k_min_num_mode);
        } else {     
            currentMode = static_cast<enum_mode>(current + 1);
        }
    }     
}   


void LEDCore::Show()
{
    if(bright == MAX_BRIGHT) {
        for(size_t i = 0; i < NUM_LEDS; ++i) {
            leds.at(i)->color(fl_rgb_color(fstleds[i].GetIntR(),
                                           fstleds[i].GetIntG(),
                                           fstleds[i].GetIntB()));
            leds.at(i)->redraw();
        }
    } else if(bright < MAX_BRIGHT && bright > MIN_BRIGHT) {
        RGBLed led{};
        for(size_t i = 0; i < NUM_LEDS; ++i) {
            double ratio = bright / static_cast<double>(PRECISION_BRIGHT);
            led.r = ratio * fstleds[i].GetIntR();
            led.g = ratio * fstleds[i].GetIntG();
            led.b = ratio * fstleds[i].GetIntB();
            leds.at(i)->color(fl_rgb_color(led.r, led.g, led.b));
            leds.at(i)->redraw();
        }
    } else { // Off, color is black;
        for(const auto& led : leds) {
            led->color(FL_BLACK);
            led->redraw();
        }
    }
}

void LEDCore::Clear()
{
    for(auto& fl : fstleds)
        fl.SetIntRGB(0, 0, 0); // Black;
    if(bright == MAX_BRIGHT) { 
        Show(); 
        return; 
    } else {
        int current_bright = bright;
        bright = MAX_BRIGHT;
        Show();
        bright = current_bright;
    }
}

void LEDCore::Waits(double sec)
{
    Timer t;
    while(true) {
        if(t.Elapsed() >= sec) { return; }
        if(!Fl::check()) { std::exit(0); }
    }
}

void LEDCore::Fill(int r, int g, int b)
{
    for(auto& fl : fstleds)
        fl.SetIntRGB(r, g, b);
    Show();
}


////////////////////////////////////////////////////////////////////////////////
/// SUPPORT FUNCTIONS //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <typename T, std::size_t size>
void fill_in_turn(LEDCore *core,
                  const std::array<T, size> &carr, 
                  size_t shift_color)
{
	size_t ccount{ carr.size() };
	for(size_t ptr = 0; ptr < NUM_LEDS; ++ptr)
		(*core)[ptr] = carr.at((ptr + shift_color) % ccount);
}


////////////////////////////////////////////////////////////////////////////////
/// Pattern ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void ModeStop::IntLoop()
{
    core->Clear();
    core->SetMode(mode_null);
    core->Waits(0.001);
}


void ModeRainbow::IntLoop()
{
    int shue{};
    while(core->GetMode() == curr) {
        fill_rainbow(core->GetFstleds(), NUM_LEDS, shue, 20);
        core->Show();
        core->Waits(0.01);
        --shue;
        if(shue < 0) { shue = 255; }
    }
}


void ModeRainbowMeteor::FadeAll()
{
    for(int i = 0; i < NUM_LEDS; ++i) {
        (*core)[i].Nscale8(247);
    }
}

void ModeRainbowMeteor::IntLoop()
{
    int hue{ 150 };
    core->Clear();
    while(core->GetMode() == curr) {
        for(int i = 0; i < NUM_LEDS; ++i) {
            ++hue;
            if(hue > 255) { hue = 0; }
            (*core)[i] = CHSV(hue, 255, 255);
            FadeAll();
            core->Show();
            core->Waits(0.03);
            if(core->GetMode() != curr) { return; }
        }
        for(int i = NUM_LEDS - 1; i >= 0; --i) {
            ++hue;
            if(hue < 0) { hue = 255; }
            (*core)[i] = CHSV(hue, 255, 255);
            FadeAll();
            core->Show();
            core->Waits(0.03);
            if(core->GetMode() != curr) { return; }            
        }
    }
}


void ModeRainbowGlitter::AddGlitter(int chance)
{
	if(prandom_range(100) <= chance) {
	    (*core)[prandom_range(NUM_LEDS)] += CRGB::White;
	}
}

void ModeRainbowGlitter::IntLoop()
{
    int shue{ 0 };
    while(core->GetMode() == curr) {
        fill_rainbow(core->GetFstleds(), NUM_LEDS, shue, 5);
        AddGlitter(30);
        AddGlitter(50);
        core->Show();
        core->Waits(0.1);
        shue -= 5;
        if(shue < 0) { shue = 255; }
    }
}


void ModeStars::IntLoop()
{
	size_t i{};
	size_t random_led{ 0 };
	int star_count{ 0 };
	fill_solid(core->GetFstleds(), NUM_LEDS, base_col);
	// First, checking stars:
	for(i = 0; i < NUM_LEDS; ++i) {
		if(barr.at(i)) {
			(*core)[i] = star_col;
			++star_count;
		}
	}
	// Random stars:
	while(core->GetMode() == curr) {
		random_led = static_cast<size_t>(prandom_range(NUM_LEDS) - 1);
		if(barr.at(random_led)) { continue; }
		barr.at(random_led) = true;
		(*core)[random_led] = star_col;
		++star_count;
		core->Show();
		core->Waits(k_delay);
		if(star_count >= NUM_LEDS) { break; }
	}
	// Random base:
	while(core->GetMode() == curr) {
		random_led = static_cast<size_t>(prandom_range(NUM_LEDS) - 1);
		if(!barr.at(random_led)) { continue; }
		barr.at(random_led) = false;
		(*core)[random_led] = base_col;
		--star_count;
		core->Show();
		core->Waits(k_delay);
		if(star_count <= 0) { break; }
	}
}


void ModeRunningDots::IntLoop()
{
    size_t max_shift = carr.size();
    size_t shift = max_shift;
    while(core->GetMode() == curr) {
        fill_in_turn(core, carr, shift);
        core->Show();
        core->Waits(k_delay);
        --shift;
        if(shift == 0)
            shift = max_shift;
	}
}



void ModePacifica::pacifica_one_layer(CRGBPalette16& p, 
									  uint16_t cistart, uint16_t wavescale, 
									  uint8_t bri, uint16_t ioff)
{
	uint16_t ci = cistart;
	uint16_t waveangle = ioff;
	uint16_t wavescale_half = (wavescale / 2) + 20;
	for(uint16_t i = 0; i < NUM_LEDS; i++) {
    	waveangle += 250;
    	uint16_t s16 = sin16(waveangle) + 32768;
    	uint16_t cs = scale16(s16, wavescale_half) + wavescale_half;
    	ci += cs;
    	uint16_t sindex16 = sin16(ci) + 32768;
    	uint8_t sindex8 = scale16(sindex16, 240);
    	CRGB c = ColorFromPalette(p, sindex8, bri, LINEARBLEND);
    	(*core)[i] += c;
	}
}

void ModePacifica::pacifica_add_whitecaps()
{
	uint8_t basethreshold = beatsin8(core->GetMillis(), 9, 55, 65);
	uint8_t wave = beat8(core->GetMillis(), 7);  
	for(uint16_t i = 0; i < NUM_LEDS; i++) {
  		uint8_t threshold = scale8(sin8(wave), 20) + basethreshold;
    	wave += 7;
    	uint8_t l = (*core)[i].GetAverageLight();
    	if(l > threshold) {
        	uint8_t overage = l - threshold;
        	uint8_t overage2 = qadd8(overage, overage);
      		(*core)[i] += CRGB(overage, overage2, qadd8(overage2, overage2));
    	}
  	}
}

void ModePacifica::IntLoop()
{   
	// Increment the four "color index start" counters, one for each wave layer.
	// Each is incremented at a different speed, and the speeds vary over time.
	static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
	static uint32_t sLastms = 0;
	uint32_t ms = core->GetMillis();
	uint32_t deltams = ms - sLastms;
	sLastms = ms;
	uint16_t speedfactor1 = beatsin16(core->GetMillis(), 3, 179, 269);
	uint16_t speedfactor2 = beatsin16(core->GetMillis(), 4, 179, 269);
	uint32_t deltams1 = (deltams * speedfactor1) / 256;
	uint32_t deltams2 = (deltams * speedfactor2) / 256;
	uint32_t deltams3 = (deltams1 + deltams2) / 2;
	sCIStart1 += (deltams1 * beatsin88(core->GetMillis(), 1011, 10, 13));
	sCIStart2 -= (deltams3 * beatsin88(core->GetMillis(), 777, 8, 11));
	sCIStart3 -= (deltams1 * beatsin88(core->GetMillis(), 501, 5, 7));
	sCIStart4 -= (deltams2 * beatsin88(core->GetMillis(), 257, 4, 6));

	// Clear out the LED array to a dim background blue-green
	fill_solid(core->GetFstleds(), NUM_LEDS, CRGB(4, 72, 87));

	// Render each of four layers, with different scales and speeds, that vary over time
	pacifica_one_layer(pacifica_palette_1, sCIStart1, 
	                   beatsin16(core->GetMillis(), 3, 11 * 256, 14 * 256), 
	                   beatsin8(core->GetMillis(), 10, 70, 130), 
	                   0 - beat16(core->GetMillis(), 301));
	pacifica_one_layer(pacifica_palette_2, sCIStart2, 
	                   beatsin16(core->GetMillis(), 4,  6 * 256,  9 * 256), 
	                   beatsin8(core->GetMillis(), 17, 40,  80), 
	                   beat16(core->GetMillis(), 401));
	pacifica_one_layer(pacifica_palette_3, sCIStart3, 6 * 256, 
	                   beatsin8(core->GetMillis(), 9, 10,38), 
	                   0 - beat16(core->GetMillis(), 503));
	pacifica_one_layer(pacifica_palette_3, sCIStart4, 5 * 256, 
	                   beatsin8(core->GetMillis(), 8, 10,28), 
	                   beat16(core->GetMillis(), 601));

	// Add brighter 'whitecaps' where the waves lines up more
	pacifica_add_whitecaps();
		
  	core->Show();
	core->Waits(k_delay);
}


void ModeRGB::IntLoop()
{
    fill_in_turn(core, carr);
    core->Show();
    core->Waits(k_delay);
}


void ModeCMYK::IntLoop()
{
    fill_in_turn(core, carr);
    core->Show();
    core->Waits(k_delay);
}


void ModeWhite::IntLoop()
{
    fill_solid(core->GetFstleds(), NUM_LEDS, CRGB::White);
    core->Show();
	core->Waits(k_delay);
}


////////////////////////////////////////////////////////////////////////////////