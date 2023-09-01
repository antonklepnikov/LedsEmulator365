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
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <string>
#include <exception>
#include <functional>
#include <unordered_map>
#include <list>


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

class Selector {
private:
	FdHandler **fdArray;
	int fdArrayLen;
	int maxFd;
public:
	Selector() : fdArray(0), fdArrayLen(0), maxFd(-1) {}
	~Selector() { if(fdArray) delete[] fdArray; }
	void Add(FdHandler *fdh);
	bool Remove(FdHandler *fdh);
	void Select();
    // No copying and assignment:
    Selector(const Selector&) = delete;
    Selector& operator=(const Selector&) = delete;	
};


////////////////////////////////////////////////////////////////////////////////

class TcpServer;
class TcpSession;

typedef std::function<void()> HandleFn;


class TcpSession : private FdHandler {
friend class TcpServer;
private:
	char buffer[TCP_LINE_MAX_LENGTH + 1];
	int bufUsed;
	bool ignoring;
	std::string networkDetails{};
	TcpServer *master;
	CorePultInterface cpi;
	std::unordered_map<std::string, HandleFn> handleMap; // For branching, 
	                                                     // see the constructor.
	TcpSession(TcpServer *am, int fd, LEDCore *cp);
	virtual ~TcpSession() {}
	virtual void Handle(bool r, bool w);
	void Halt();
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
friend class TcpServer;
private:
	bool windowClosed;
	DisplaySession(int fd) : FdHandler(fd, false), windowClosed(false) {}
	virtual ~DisplaySession() {}
	virtual void Handle(bool r, bool w);
};

class TcpServer : public FdHandler {
private:
	DisplaySession disp;
	Selector *sel;
	SrvLogger *slg;
	LEDCore *lcp;
	std::list<TcpSession*> garblist;
	bool serverStop;
	TcpServer(int fdDisp, Selector *aFds, 
	          LEDCore *cp, SrvLogger *sl, int fdSrv);
    void GarbCollect();
public:
	static TcpServer Start(int display, Selector *sp, LEDCore *cp, 
	                       SrvLogger *sl, int port);
	virtual ~TcpServer();
	virtual void Handle(bool r, bool w);
	void RemoveTcpSession(TcpSession *s);
	void ServerStep();
	bool ServerReady() const { return (!disp.windowClosed && !serverStop); }
	// No copying and assignment:
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;	
};

////////////////////////////////////////////////////////////////////////////////

class TcpServerFault : public std::exception {
private:
    std::string error;
public:
    TcpServerFault(std::string_view er) : error(er) {}
    const char* what() const noexcept override { return error.c_str(); }
};


////////////////////////////////////////////////////////////////////////////////

#endif
