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


////////////////////////////////////////////////////////////////////////////////
/// TCP-SERVER PART (temporary) ////////////////////////////////////////////////
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
	char *name;
	FdServer *srvMaster;
	
	TcpSession(FdServer *am, int fd);
	virtual ~TcpSession();
	virtual void Handle(bool r, bool w);
	void Say(const char *msg);
	void ReadAndIgnore();
	void ReadAndCheck();
	void CheckLines();
	void ProcessLine(const char *str);
	
	// No copying and assignment:
    TcpSession(const TcpSession&) = delete;
    TcpSession& operator=(const TcpSession&) = delete;
};

class DisplaySession : private FdHandler {
friend class FdServer;
private:
	DisplaySession(FdServer *argSrvMaster, int fd);
	virtual ~DisplaySession();
	virtual void Handle(bool r, bool w);
};


class FdServer : public FdHandler {
private:
	FdSelector *fdsel;
	LEDCore *core;
	FdServer(FdSelector *aFds, LEDCore *cp, int fd);
public:
	static FdServer* Start(FdSelector *fsl, LEDCore *cp, int port);
	
	virtual ~FdServer();
	virtual void Handle(bool r, bool w);
	void RemoveTcpSession(TcpSession *s) { fdsel->Remove(s); }

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
    FdSelector fdsel; 
    bool quit_flag;
    
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

    void ModeChooser();

public:
    MainSelector(LEDCore *c) : fdsel{}, quit_flag(false), core(c), buttons{} {}

    // No copying and assignment:
    MainSelector(const MainSelector&) = delete;
    MainSelector& operator=(const MainSelector&) = delete;
    
	void BreakLoop() { quit_flag = true; }
    void Run();
};


////////////////////////////////////////////////////////////////////////////////

#endif
