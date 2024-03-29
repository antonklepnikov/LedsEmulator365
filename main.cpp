////////////////////////////////////////////////////////////////////////////////

/*** 
    LedEmulator365 
                  ***/


////////////////////////////////////////////////////////////////////////////////
/// HEADERS ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>

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
    if(argc <= 1) {
        std::cerr << "Usage: " << argv[0] << " <TCP-server port [1024..49151]>" 
                  << std::endl;
        return 1;
    }
    std::stringstream convert{ argv[1] };
    int port{};
    if(!(convert >> port) || (port < 1024 || port > 49151)) { port = 1024; }
    
    std::exception_ptr exceptPtr; // Object for storing exceptions or nullptr.
    
    auto display { XOpenDisplay(0) };
    if(!display) {
        std::cerr << "Can't open display, exit(1)" << std::endl;
        std::exit(1); 
    }        	
   	fl_open_display(display);
    auto displayFd { ConnectionNumber(display) };    
        
    try {    
        
        SrvLogger logger(le365const::server_log_file.data());
        std::string logMsg{ "Logs are recorded in: " };
        logMsg += le365const::server_log_file;
        logger.WriteLog(logMsg.c_str());
        
        Selector selector(&logger);
        LEDCore core;

        TcpServer server{ TcpServer::Start(
                            displayFd, &selector, &core, &logger, port) };
        std::string srvMsg{ "TCP-server listens port: " + 
                             std::to_string(port) };    
        logger.WriteLog(srvMsg.c_str());

        MainLoop loop(&server, &core);
        auto window{ Window365::Make(&core, &loop) };
    
        [[maybe_unused]] auto keyHandler{ new KeyHandler() };
        KeyHandler::cpi.Init(&core);
    
        window->show();
        loop.Run();
        
        logger.WriteLog("Program shutdown with code: 0");
        return 0;
    
    } catch(...) {
        exceptPtr = std::current_exception();
        return process_exception(exceptPtr);
    }
}
