////////////////////////////////////////////////////////////////////////////////

/*** 
    LedEmulator365 
                  ***/


////////////////////////////////////////////////////////////////////////////////
/// HEADERS ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <X11/Xlib.h>
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
    	std::cerr << "Can't open display!" << std::endl;
    	std::exit(1); 
    }

    int dsFd { ConnectionNumber(ds) };
std::cerr << "Display fd = " << dsFd << std::endl;    
    
    ////////////////////////////////////////////////////////////////////////////
    
    try {
        auto core { new LEDCore };
        auto fdsel{ new FdSelector };
         
        auto server{ FdServer::Start(dsFd, fdsel, core, TCP_LISTEN_PORT) };
if(!server) { // ERROR HANDLING, WORKING WITHOUT SERVER!!!???
	std::cerr << "Error of starting TCP-server, std::exit(1)" << std::endl;
	std::exit(1);
} else {
	std::cerr << "TCP-server running on port " << TCP_LISTEN_PORT << std::endl; 
}

		auto selector { new MainSelector(server, core) };
        auto window{ Window365::Make(ds, core, selector) };
      
        window->show(argc, argv);
Fl::check();
        selector->Run();
        
    } catch(...) {
        eptr = std::current_exception();
        return process_exception(eptr);
    }
        
std::cerr << "Return 0, bye" << std::endl; 
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
