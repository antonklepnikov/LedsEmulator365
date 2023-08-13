#ifndef SELECTOR_AK_H
#define SELECTOR_AK_H

////////////////////////////////////////////////////////////////////////////////

/***
    Managing modes
                  ***/


////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "led_core.h"
#include "oofl.h"

#include <array>

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>


////////////////////////////////////////////////////////////////////////////////
/// TCP-SERVER PART (temporary) ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// TEXT CONSTANTS FOR SERVER-CLIENT INTERRACTION //////////////////////////////

static const char SERVER_WELCOME[] =
	"SRV: Hello, you are connected to LEDCore!\nCLI: ";
static const char SERVER_NEW_LINE[] = "\nCLI: ";

static const char CLIENT_EXIT[] = "exit";
static const char CLIENT_MODE_1[] = "mode=1";
static const char CLIENT_MODE_2[] = "mode=2";
static const char CLIENT_MODE_3[] = "mode=3";
static const char CLIENT_MODE_4[] = "mode=4";
static const char CLIENT_MODE_5[] = "mode=5";
static const char CLIENT_MODE_6[] = "mode=6";
static const char CLIENT_MODE_7[] = "mode=7";
static const char CLIENT_MODE_8[] = "mode=8";
static const char CLIENT_MODE_9[] = "mode=9";
static const char CLIENT_UP[] = "up";
static const char CLIENT_DOWN[] = "down";
static const char CLIENT_RIGHT[] = "right";
static const char CLIENT_LEFT[] = "left";
static const char CLIENT_OK[] = "ok";


////////////////////////////////////////////////////////////////////////////////


class FdHandler {
private:
	int fd;
	bool ownFd;
public:
	FdHandler(int aFd, bool own) : fd(aFd), ownFd(own) {}
	virtual ~FdHandler() { if(ownFd) { close(fd); } }
	virtual void Handle(bool r, bool w) = 0;
	int GetFd() const { return fd; }
	virtual bool WantRead() const { return true; }
	virtual bool WantWrite() const { return false; }
};

class FdSelector { // STD::VECTOR?!
private:
	FdHandler **fdArray;
	int fdArrayLen;
	int maxFd;
	bool noErr;
public:
	FdSelector() : fdArray(0), fdArrayLen(0), maxFd(-1), noErr(true) {}
	~FdSelector() { if(fdArray) delete[] fdArray; }
	void Add(FdHandler *fdh);
	bool Remove(FdHandler *fdh);
	bool FdSelReady() const { return noErr; } 
	void FdSelect();
    // No copying and assignment:
    FdSelector(const FdSelector&) = delete;
    FdSelector& operator=(const FdSelector&) = delete;	
};


////////////////////////////////////////////////////////////////////////////////

class FdServer;

class TcpSession : private FdHandler {
friend class FdServer;
private:
	char buffer[TCP_LINE_MAX_LENGTH + 1];
	int bufUsed;
	bool ignoring;
	FdServer *srvMaster;
	
	TcpSession(FdServer *am, int fd);
	virtual ~TcpSession() {}
	virtual void Handle(bool r, bool w);
	void Say(const char *msg);
	void ReadAndIgnore();
	void ReadAndCheck();
	void CheckLines();
	void ProcessLine(const char *str);
	void ServerAnswer(const char *str);
	
	// No copying and assignment:
    TcpSession(const TcpSession&) = delete;
    TcpSession& operator=(const TcpSession&) = delete;
};

class DisplaySession : private FdHandler {
friend class FdServer;
private:
	bool windowClosed;
	
	DisplaySession(int fd) : FdHandler(fd, false), windowClosed(false) {}
	virtual ~DisplaySession() {}
	virtual void Handle(bool r, bool w);
};

class CorePultInterface {
private:
	LEDCore *core;
public:
	CorePultInterface(LEDCore *cp) : core(cp) {}
	
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

class FdServer : public FdHandler {
private:
	DisplaySession disp;
	FdSelector *fdsel;
	CorePultInterface cpi;
	bool serverStop;
	FdServer(int fdDisp, FdSelector *aFds, LEDCore *cp, int fdSrv);
public:
	static FdServer* Start(int display, FdSelector *fsl, LEDCore *cp, int port);
	
	virtual ~FdServer();
	virtual void Handle(bool r, bool w);
	void RemoveTcpSession(TcpSession *s);
	void ServerStep() { fdsel->FdSelect(); }
	bool ServerReady() const { return (!disp.windowClosed && 
	                                   !serverStop &&
	                                   fdsel->FdSelReady()); }

	void GUI_Mode(enum_mode m) 	{	cpi.Mode(m);	}
	void GUI_Ok()				{	cpi.Ok();		}
	void GUI_Up() 				{	cpi.Up();		}
	void GUI_Down()				{	cpi.Down();		}
	void GUI_Left() 			{	cpi.Left();		}
	void GUI_Right() 			{	cpi.Right();	}

	// No copying and assignment:
    FdServer(const FdServer&) = delete;
    FdServer& operator=(const FdServer&) = delete;	
};


////////////////////////////////////////////////////////////////////////////////
/// CLASS: MainSelector ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class MainSelector {
    friend class Window365;
private:
    FdServer *tcps;
    LEDCore *core;
    
    std::array<OOFLButton*, NUM_ALL_BUTTONS> buttons;
    
    ModeRainbow 			rainbow{};									   // 1
    ModeRainbowMeteor 		rainbowMeteor{};      						   // 2
    ModeRainbowGlitter 		rainbowGlitter{};							   // 3
    ModeStars				stars{};			                           // 4	  
    ModeRunningDots			runningDots{};								   // 5
    ModePacifica            pacifica{};                                    // 6
    ModeRGB                 rgb{};                                         // 7 
    ModeCMYK                cmyk{};                                        // 8
    ModeWhite               white{};                                       // 9  
    ModeStop 				stop{};                                        // O

    void MainLoopStep();

public:
    MainSelector(FdServer *s, LEDCore *c) 
    	: tcps(s), core(c), buttons{} {}

    // No copying and assignment:
    MainSelector(const MainSelector&) = delete;
    MainSelector& operator=(const MainSelector&) = delete;
    
    void Run();
};


////////////////////////////////////////////////////////////////////////////////

#endif
