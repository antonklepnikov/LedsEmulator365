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

void LEDCore::Show()
{
    if(bright == MAX_BRIGHT) {
        for(int i = 0; i < NUM_LEDS; ++i) {
            leds[i]->color(fl_rgb_color(fstleds[i].GetIntR(),
                                        fstleds[i].GetIntG(),
                                        fstleds[i].GetIntB()));
            leds[i]->redraw();
        }
    } else if(bright < MAX_BRIGHT && bright > MIN_BRIGHT) {
        RGBLed led{};
        for(int i = 0; i < NUM_LEDS; ++i) {
            double ratio = bright / static_cast<double>(PRECISION_BRIGHT);
            led.r = ratio * fstleds[i].GetIntR();
            led.g = ratio * fstleds[i].GetIntG();
            led.b = ratio * fstleds[i].GetIntB();
            leds[i]->color(fl_rgb_color(led.r, led.g, led.b));
            leds[i]->redraw();
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
	size_t ptr{ 0 };
	size_t shift{ 5 };
	size_t ccount = carr.size();
	while(core->GetMode() == curr) {
		for(ptr = 0; ptr < NUM_LEDS; ++ptr) {
		    (*core)[ptr] = carr.at((ptr + shift) % ccount);
		}
		core->Show();
		core->Waits(k_delay);
		--shift;
		if(shift == 0) { shift = 5; }
	}	
}


void ModePacifica::IntLoop()
{
    fill_solid(core->GetFstleds(), NUM_LEDS, CRGB::Cyan);
    core->Show();
}


////////////////////////////////////////////////////////////////////////////////
