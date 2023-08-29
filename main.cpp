////////////////////////////////////////////////////////////////////////////////

/*** 
    LedEmulator365 
                  ***/


////////////////////////////////////////////////////////////////////////////////
/// HEADERS ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <X11/Xlib.h>

#include "common.h"
#include "led_core.h"
#include "tcp_srv.h"
#include "led_gui.h"
#include "main_loop.h"
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
    
    auto display { XOpenDisplay(0) };
    if(!display) {
        std::cerr << "Can't open display, exit(1)" << std::endl;
        std::exit(1); 
    }        	
   	fl_open_display(display);
    auto displayFd { ConnectionNumber(display) };    
        
    try {    
        
        FdSelector selector;
        LEDCore core;
        
        SrvLogger logger(SERVER_LOG_FILE);
        logger.WriteLog("Logging system sucessful started");

        FdServer server{ FdServer::Start(displayFd, &selector, &core, &logger, 
                                         TCP_LISTEN_PORT) };
        std::clog << "TCP-server is running on port: " 
                  << TCP_LISTEN_PORT << std::endl;

        MainLoop loop(&server, &core);
        auto window{ Window365::Make(&core, &loop) };
    
        [[maybe_unused]] auto keyHandler{ new KeyHandler() };
        KeyHandler::cpi.Init(&core);
    
        window->show(argc, argv);
        return loop.Run();
    
    } catch(...) {
        exceptPtr = std::current_exception();
        return process_exception(exceptPtr);
    }
}
