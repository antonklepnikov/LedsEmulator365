////////////////////////////////////////////////////////////////////////////////

/*** 
    LedEmulator365 
                  ***/


////////////////////////////////////////////////////////////////////////////////
/// HEADERS ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <X11/Xlib.h>
#include <iostream>

#include "led_gui.h"
#include "led_core.h"
#include "process_exception.h"


////////////////////////////////////////////////////////////////////////////////
/// MAIN BLOCK /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    std::exception_ptr eptr;    // Object for storing exceptions or nullptr.
    
    ////////////////////////////////////////////////////////////////////////////
    
    Display *ds { nullptr };
    ds = XOpenDisplay(0);
    if(!ds) { 
    	std::cout << "Can't open display!" << std::endl;
    	std::exit(1); 
    }
    // int fd { ConnectionNumber(ds) };
    
    
    ////////////////////////////////////////////////////////////////////////////
    
    try {
        LEDCore core;
        Selector selector(&core);
        auto *window{ Window365::Make(ds, &core, &selector) };
        window->show(argc, argv);
        selector.Run();
    } catch(...) {
        eptr = std::current_exception();
        return process_exception(eptr);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
