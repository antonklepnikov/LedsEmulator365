////////////////////////////////////////////////////////////////////////////////

/*** 
     IMPLEMENTATION:
     TCP-server
                ***/


////////////////////////////////////////////////////////////////////////////////

#include "tcp_srv.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void FdSelector::Add(FdHandler *fdh)
{
	int i{};
	int fd = fdh->GetFd();
	if(!fdArray) {
		fdArrayLen = fd > 15 ? fd + 1 : 16;
		fdArray = new FdHandler*[fdArrayLen];
		for(i = 0; i < fdArrayLen; ++i) { fdArray[i] = nullptr; }
	}
	if(fdArrayLen <= fd) {
		int newLen = fd + 1;
		FdHandler **tmpArr = new FdHandler*[newLen];
		for(i = 0; i <= fd; ++i) { tmpArr[i] = i < fdArrayLen ? fdArray[i] : 0; }
		fdArrayLen = newLen;
		delete[] fdArray;
		fdArray = tmpArr;
	}
	if(fd > maxFd) { maxFd = fd; }
	fdArray[fd] = fdh;
}

bool FdSelector::Remove(FdHandler *fdh)
{
	int fd{ fdh->GetFd() };
	if(fd >= fdArrayLen || fdArray[fd] != fdh) { return false; }
	fdArray[fd] = nullptr;
	if(fd == maxFd) { while(maxFd >= 0 && !fdArray[maxFd]) { -- maxFd; } }
	return true;
}

void FdSelector::FdSelect()
{
	int i{};
	fd_set rds;
    fd_set wrs;
	FD_ZERO(&rds);
    FD_ZERO(&wrs);
	timeval to;
    to.tv_sec = SELECT_DELAY_SEC;
    to.tv_usec = SELECT_DELAY_USEC;
	for(i = 0; i <= maxFd; ++i) {
		if(fdArray[i]) {
			if(fdArray[i]->WantRead()) { FD_SET(i, &rds); }
			if(fdArray[i]->WantWrite()) { FD_SET(i, &wrs); }
		}
	}
	int res = select(maxFd + 1, &rds, &wrs, 0, &to);
	if(res < 0 && errno != EINTR) { noErr = false; }
	else if(res > 0) {
		for(i = 0; i <= maxFd; ++i) {
			if(!fdArray[i]) { continue; }
			bool r = FD_ISSET(i, &rds);
			bool w = FD_ISSET(i, &wrs);
			if(r || w) { fdArray[i]->Handle(r, w); }
		}
	}
}


////////////////////////////////////////////////////////////////////////////////

FdServer FdServer::Start(int display, FdSelector *fsl, LEDCore *cp,
                           SrvLogger *sl, int port)
{
	int ls{ socket(AF_INET, SOCK_STREAM, 0) };
	if(ls == -1)
	    throw FdServerException("FdServer start fault in: socket()");
	int opt { 1 };
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	int res{ bind(ls, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) };
	if(res == -1)
	    throw FdServerException("FdServer start fault in: bind()");
	res = listen(ls, TCP_QLEN_FOR_LISTEN);
	if(res == -1)
	    throw FdServerException("FdServer start fault in: listen()");
	return FdServer(display, fsl, cp, sl, ls);
}

FdServer::FdServer(int fdDisp, FdSelector *fsl, 
                   LEDCore *cp, SrvLogger *sl, int fdSrv) 
	                    : FdHandler(fdSrv, true), disp(fdDisp), 
	                      fdsel(fsl), slg(sl), cpi(), serverStop(false)
{ 
	cpi.Init(cp);
	fdsel->Add(&disp);
	fdsel->Add(this);	
}

FdServer::~FdServer()
{ 
	fdsel->Remove(this);
	close(GetFd());
}

void FdServer::Handle(bool r, [[maybe_unused]] bool w)
{
	if(!r) { return; }
	sockaddr_in addr{};
	socklen_t len{ sizeof(addr) };
	int sd{ accept(GetFd(), reinterpret_cast<sockaddr*>(&addr), &len) };
	if(sd == -1) { return; }
	TcpSession *p = new TcpSession(this, sd);
	fdsel->Add(p);
	slg->WriteLog("The remote user was connected, IP: 127.0.0.1");
}

void FdServer::RemoveTcpSession(TcpSession *s)
{ 
	fdsel->Remove(s); 
	close(s->GetFd());
	delete s;
	slg->WriteLog("The remote user has disconnected, IP: 127.0.0.1");
}


////////////////////////////////////////////////////////////////////////////////

void TcpSession::Say(const char *msg)
{
	write(GetFd(), msg, strlen(msg));
}

TcpSession::TcpSession(FdServer *am, int fd) 
	: FdHandler(fd, true), bufUsed(0), ignoring(false), srvMaster(am) 
{ 
	Say(SERVER_WELCOME); 
}

void TcpSession::Handle(bool r, [[maybe_unused]] bool w)
{
	if(!r) { return; }
	if(bufUsed >= static_cast<int>(sizeof(buffer))) {
		bufUsed = 0;
		ignoring = true;
	}
	if(ignoring) { ReadAndIgnore(); }
	else { ReadAndCheck(); }
}

void TcpSession::ReadAndIgnore()
{
	int r = read(GetFd(), buffer, sizeof(buffer));
	if(r < 1) {
		srvMaster->RemoveTcpSession(this);
		return;
	}
	for(int i = 0; i < r; ++i) {
		if(buffer[i] == '\n') { // Stop ignoring.
			int rest{ r - i - 1 };
			if(rest > 0) { 
				memmove(buffer, buffer + i + 1, static_cast<size_t>(rest)); 
			}
			bufUsed = rest;
			ignoring = false;
			CheckLines();
		}
	}
}

void TcpSession::ReadAndCheck()
{
	int r = read(GetFd(), buffer + bufUsed, 
	             sizeof(buffer) - static_cast<size_t>(bufUsed));
	if(r < 1) {
		srvMaster->RemoveTcpSession(this);
		return;
	}
	bufUsed += r;
	CheckLines();	
}

void TcpSession::CheckLines()
{
	if(bufUsed <= 0) { return; }
	int i{};
	for(i = 0; i < bufUsed; ++i) {
		if(buffer[i] == '\n') {
			buffer[i] = 0;
			if(i > 0 && buffer[i-1] == '\r') { buffer[i-1] = 0; }
			ProcessLine(buffer);
			int rest{ bufUsed - 1 - i };
			if(rest < 0) { return; }				// This check is need ???!!!
			memmove(buffer, buffer + i + 1, static_cast<size_t>(rest));
			bufUsed = rest;
			CheckLines();
			return;
		}
	}
}

void TcpSession::ProcessLine(const char *str)
{
	if(!strcmp(str, CLIENT_EXIT)) { 
		ServerAnswer("Ok, bye"); 
		srvMaster->RemoveTcpSession(this);
	}
	else if(!strcmp(str, CLIENT_MODE_1)) { 
		ServerAnswer("Ok, mode_1"); 
		srvMaster->GUI_Mode(mode_1);
	}
	else if(!strcmp(str, CLIENT_MODE_2)) { 
		ServerAnswer("Ok, mode_2"); 
		srvMaster->GUI_Mode(mode_2);
	}
	else if(!strcmp(str, CLIENT_MODE_3)) { 
		ServerAnswer("Ok, mode_3"); 
		srvMaster->GUI_Mode(mode_3);
	}
	else if(!strcmp(str, CLIENT_MODE_4)) { 
		ServerAnswer("Ok, mode_4"); 
		srvMaster->GUI_Mode(mode_4);
	}
	else if(!strcmp(str, CLIENT_MODE_5)) { 
		ServerAnswer("Ok, mode_5"); 
		srvMaster->GUI_Mode(mode_5);
	}
	else if(!strcmp(str, CLIENT_MODE_6)) { 
		ServerAnswer("Ok, mode_6"); 
		srvMaster->GUI_Mode(mode_6);
	}
	else if(!strcmp(str, CLIENT_MODE_7)) { 
		ServerAnswer("Ok, mode_7"); 
		srvMaster->GUI_Mode(mode_7);
	}
	else if(!strcmp(str, CLIENT_MODE_8)) { 
		ServerAnswer("Ok, mode_8"); 
		srvMaster->GUI_Mode(mode_8);
	}
	else if(!strcmp(str, CLIENT_MODE_9)) { 
		ServerAnswer("Ok, mode_9"); 
		srvMaster->GUI_Mode(mode_9);
	}
	else if(!strcmp(str, CLIENT_UP)) { 
		ServerAnswer("Ok, up"); 
		srvMaster->GUI_Up();
	}
	else if(!strcmp(str, CLIENT_DOWN)) { 
		ServerAnswer("Ok, down"); 
		srvMaster->GUI_Down();
	}
	else if(!strcmp(str, CLIENT_LEFT)) { 
		ServerAnswer("Ok, left"); 
		srvMaster->GUI_Left();
	}
	else if(!strcmp(str, CLIENT_RIGHT)) { 
		ServerAnswer("Ok, right"); 
		srvMaster->GUI_Right();
	}
	else if(!strcmp(str, CLIENT_OK)) { 
		ServerAnswer("Ok, OK"); 
		srvMaster->GUI_Ok();
	}
	else { ServerAnswer("Bad, command not found"); }
}

void TcpSession::ServerAnswer(const char *str)
{
	const char *srvStr{ SERVER_NEW_LINE };
	int lenSrvStr{ static_cast<int>(strlen(srvStr)) };
	int lenStr{ static_cast<int>(strlen(str)) };
	char *msg = new char[lenStr + lenSrvStr];
	sprintf(msg, "SRV: %s%s", str, srvStr);
	Say(msg);
	delete[] msg;
}


////////////////////////////////////////////////////////////////////////////////

void DisplaySession::Handle(bool r, [[maybe_unused]] bool w)
{
	if(!r) { return; }
	if(!Fl::check()) { windowClosed = true; }
}


////////////////////////////////////////////////////////////////////////////////
