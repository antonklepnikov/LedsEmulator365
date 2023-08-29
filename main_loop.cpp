////////////////////////////////////////////////////////////////////////////////

/*** 
     IMPLEMENTATION:
     Main loop of the programm
                               ***/


////////////////////////////////////////////////////////////////////////////////

#include "main_loop.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void MainLoop::ModeStep()
{
    switch(core->GetMode()) {
        case mode_1:        rainbow.Step();        break;
        case mode_2:        rainbowMeteor.Step();  break;
        case mode_3:        rainbowGlitter.Step(); break;
        case mode_4:        stars.Step();          break;
        case mode_5:        runningDots.Step();    break;
        case mode_6:        pacifica.Step();       break;
        case mode_7:        rgb.Step();            break;
        case mode_8:        cmyk.Step();           break;
        case mode_9:        white.Step();          break;
        case mode_stop:     stop.Step();           break;        
        case mode_null:
        default:                                   break;
    }
}

int MainLoop::Run()
{   
    //core->FltkStep();
    while(core->CoreRun() && tcps->ServerReady()) {
        tcps->ServerStep();
        ModeStep();
    }
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
