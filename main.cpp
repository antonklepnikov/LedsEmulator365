#define BOOST_LOG_DYN_LINK

////////////////////////////////////////////////////////////////////////////////

/*** 
    LedEmulator365 
                  ***/


////////////////////////////////////////////////////////////////////////////////
/// HEADERS ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <X11/Xlib.h>
#include <boost/log/trivial.hpp>
#include "led_gui.h"
#include "led_core.h"
#include "process_exception.h"


////////////////////////////////////////////////////////////////////////////////
/// GLOBAL /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CorePultInterface KeyHandler::cpi;


////////////////////////////////////////////////////////////////////////////////
/// MAIN BLOCK /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    std::exception_ptr exceptPtr;    // Object for storing exceptions or nullptr.
    try {
        auto display { XOpenDisplay(0) };

////////////////////////////////////////////////////////////////////////////////
if(!display) {
    BOOST_LOG_TRIVIAL(fatal) << "Can't open display, exit(1)";
   	std::exit(1); 
}
////////////////////////////////////////////////////////////////////////////////

    	fl_open_display(display);
        auto displayFd { ConnectionNumber(display) };    
        auto core{ new LEDCore };
        auto fdSel{ new FdSelector };
        auto server{ FdServer::Start(displayFd, fdSel, core, TCP_LISTEN_PORT) };

////////////////////////////////////////////////////////////////////////////////
if(!server) {
     BOOST_LOG_TRIVIAL(error)
     	<< "Error of starting TCP-server, working without network...";
} 
else {
	BOOST_LOG_TRIVIAL(info) << "TCP-server is running on port: " 
	                        << TCP_LISTEN_PORT;
}
////////////////////////////////////////////////////////////////////////////////

        auto selector { new MainSelector(server, core) };
        auto window{ Window365::Make(core, selector) };
        [[maybe_unused]] auto keyHandler{ new KeyHandler() };
        KeyHandler::cpi.Init(core);
        window->show(argc, argv);
        selector->Run();
    } catch(...) {
        exceptPtr = std::current_exception();
        return process_exception(exceptPtr);
    }

////////////////////////////////////////////////////////////////////////////////
BOOST_LOG_TRIVIAL(info) << "Shutdown, exit(0)";
////////////////////////////////////////////////////////////////////////////////

    return 0;
}
