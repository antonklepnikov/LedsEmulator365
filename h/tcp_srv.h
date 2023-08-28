#ifndef TCP_SRV_AK_H
#define TCP_SRV_AK_H


////////////////////////////////////////////////////////////////////////////////

/***
     TCP-server
                ***/


////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "srv_logger.h"
#include "led_core.h"

#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>


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

class FdSelector {
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

class FdServer : public FdHandler {
private:
	DisplaySession disp;
	FdSelector *fdsel;
	SrvLogger *slg;
	CorePultInterface cpi;
	bool serverStop;
	FdServer(int fdDisp, FdSelector *aFds, 
	         LEDCore *cp, SrvLogger *sl, int fdSrv);
public:
	static FdServer* Start(int display, FdSelector *fsl, LEDCore *cp, 
	                       SrvLogger *sl, int port);
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

#endif
