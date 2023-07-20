////////////////////////////////////////////////////////////////////////////////

/*** 
     IMPLEMENTATION:
     Managing modes 
                   ***/


////////////////////////////////////////////////////////////////////////////////

#include "selector.h"


////////////////////////////////////////////////////////////////////////////////

void Selector::Select()
{
    auto mode{ core->GetMode() };
    switch(mode) {
        case mode_1:
            if(!rainbow.GetReady())
            	rainbow.Init(core, mode_1);
            rainbow.Enable();
            break;
        case mode_2:
            if(!rainbowMeteor.GetReady())
            	rainbowMeteor.Init(core, mode_2);
            rainbowMeteor.Enable();
	        break;
        case mode_3:
            if(!rainbowGlitter.GetReady())
            	rainbowGlitter.Init(core, mode_3); 
            rainbowGlitter.Enable();
            break;
        case mode_4:
            if(!stars.GetReady())
            	stars.Init(core, mode_4); 
            stars.Enable();
            break;
        case mode_5:
            if(!runningDots.GetReady())
            	runningDots.Init(core, mode_5); 
            runningDots.Enable();
            break;
        case mode_6:
            if(!pacifica.GetReady())
            	pacifica.Init(core, mode_6); 
            pacifica.Enable();
            break;
        case mode_7:
           if(!rgb.GetReady())
            	rgb.Init(core, mode_7); 
            rgb.Enable();
            break;
        case mode_8:
           if(!cmyk.GetReady())
            	cmyk.Init(core, mode_8); 
            cmyk.Enable();
            break;
        case mode_9:
           if(!white.GetReady())
            	white.Init(core, mode_6); 
            white.Enable();
            break;
        case mode_stop:
            if(!stop.GetReady()) { stop.Init(core, mode_stop); }
            stop.Enable();
            break;
        default:
            break;
    }
}

void Selector::Run()
{
    Timer tm;
    auto thc{ true };
    while(!quit_flag && core->CoreRun()) {
        if(thc) {
            if(!Fl::check()) { BreakLoop(); }
            Select();
            tm.Reset();
            thc = false;
        }
        if(tm.Elapsed() >= 0.15) { thc = true; }
    }
}


////////////////////////////////////////////////////////////////////////////////
