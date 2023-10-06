////////////////////////////////////////////////////////////////////////////////

/*** 
     IMPLEMENTATION:
     TCP-server
                ***/


////////////////////////////////////////////////////////////////////////////////

#include "tcp_srv.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Selector::Add(FdHandler *fdh)
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

bool Selector::Remove(FdHandler *fdh)
{
	int fd{ fdh->GetFd() };
	if(fd >= fdArrayLen || fdArray[fd] != fdh) { return false; }
	fdArray[fd] = nullptr;
	if(fd == maxFd) { while(maxFd >= 0 && !fdArray[maxFd]) { --maxFd; } }
	return true;
}

void Selector::Select()
{
	int i{};
	fd_set rds;
    fd_set wrs;
	FD_ZERO(&rds);
    FD_ZERO(&wrs);
	timeval to;
    to.tv_sec   = le365const::select_delay_sec;
    to.tv_usec  = le365const::select_delay_usec;
	for(i = 0; i <= maxFd; ++i) {
		if(fdArray[i]) {
			if(fdArray[i]->WantRead()) { FD_SET(i, &rds); }
			if(fdArray[i]->WantWrite()) { FD_SET(i, &wrs); }
		}
	}
	int res{ select(maxFd + 1, &rds, &wrs, 0, &to) };
	if(res < 0 && errno != EINTR) { 
	    std::string err{ "Selector in select(): " };
	    err += strerror(errno);
	    slg->WriteLog(err.c_str());
	    throw TcpServerFault(err);
	 }
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

TcpServer TcpServer::Start(int display, Selector *sel, LEDCore *cp,
                           SrvLogger *slg, int port)
{
	int ls{ socket(AF_INET, SOCK_STREAM, 0) };
	if(ls == -1) {
		slg->WriteLog("TcpServerFault(Start() in: socket())");
	    throw TcpServerFault("Start() in: socket()");
	}
	int opt { 1 };
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	int res{ bind(ls, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) };
	if(res == -1) {
	    slg->WriteLog("TcpServerFault(Start() in: bind())");
	    throw TcpServerFault("Start() in: bind()");
	}
	res = listen(ls, le365const::tcp_qlen_for_listen);
	if(res == -1) {
	    slg->WriteLog("TcpServerFault(Start() in: listen())");
	    throw TcpServerFault("Start() in: listen()");
	}
	return TcpServer(display, sel, cp, slg, ls);
}

TcpServer::TcpServer(int fdDisp, Selector *asl, 
                     LEDCore *cp, SrvLogger *alg, int fdSrv) 
	                      : FdHandler(fdSrv, true), disp(fdDisp), 
	                        sel(asl), slg(alg), lcp(cp),
	                        garblist(), serverStop(false)
{ 
	asl->Add(&disp);
	asl->Add(this);	
}

TcpServer::~TcpServer()
{ 
	sel->Remove(this);
	close(GetFd());
}

void TcpServer::Handle(bool r, [[maybe_unused]] bool w)
{
	if(!r) { return; }
	sockaddr_in addr{};
	socklen_t len{ sizeof(addr) };
	int sd{ accept(GetFd(), reinterpret_cast<sockaddr*>(&addr), &len) };
	if(sd == -1) { 
		slg->WriteLog("TcpServerFault(Handle() in: accept())");
		throw TcpServerFault("Handle() in: accept()"); 
	}
	TcpSession *p = new TcpSession(this, sd, lcp);
	sel->Add(p);
	p->networkDetails = inet_ntoa(addr.sin_addr);
	p->networkDetails += ":";
	p->networkDetails += std::to_string(ntohs(addr.sin_port));
	std::string logMsg{ p->networkDetails + " has connected" };
	slg->WriteLog(logMsg.c_str());
}

void TcpServer::RemoveTcpSession(TcpSession *s)
{ 
	std::string logMsg{ s->networkDetails + " has disconnected" }; 
	sel->Remove(s);	
	int res{ 0 };
	int fd{ s->GetFd() };
	res = shutdown(fd, SHUT_RDWR);
	if(res != 0) { 
		slg->WriteLog("TcpServerFault(RemoveTcpSession() in: shutdown())");
		throw TcpServerFault("RemoveTcpSession() in: shutdown()");
	}
	res = close(fd);
	if(res != 0) {
		slg->WriteLog("TcpServerFault(RemoveTcpSession() in: close()");
		throw TcpServerFault("RemoveTcpSession() in: close()"); 
	}
	garblist.push_back(s);
	slg->WriteLog(logMsg.c_str());
}

void TcpServer::GarbCollect()
{
    auto iter = garblist.begin();
    while(iter != garblist.end()) {      
		delete *iter; // Delete session object.
        iter = garblist.erase(iter); // Erase std::list item and go to next.
    }

}

void TcpServer::ServerStep()
{
    sel->Select();
    GarbCollect();
}


////////////////////////////////////////////////////////////////////////////////

void TcpSession::Halt()
{
	master->RemoveTcpSession(this);	
}

void TcpSession::Say(const char *msg)
{
	write(GetFd(), msg, strlen(msg));
}

TcpSession::TcpSession(TcpServer *am, int fd, LEDCore *cp) 
	: FdHandler(fd, true), bufUsed(0), ignoring(false), master(am), cpi(),
        handleMap{
                    { le365const::client_exit.data(),
                        [this]() { this->ServerAnswer("Bye!");      
                                   this->Halt(); }},
                    { le365const::client_mode_1.data(),
                        [this]() { this->ServerAnswer("Mode=1");
                                   this->cpi.Mode(mode_1); }},
                    { le365const::client_mode_2.data(),
                        [this]() { this->ServerAnswer("Mode=2");
                                   this->cpi.Mode(mode_2); }},
                    { le365const::client_mode_3.data(),
                        [this]() { this->ServerAnswer("Mode=3");
                                   this->cpi.Mode(mode_3); }},
                    { le365const::client_mode_4.data(),
                        [this]() { this->ServerAnswer("Mode=4");
                                   this->cpi.Mode(mode_4); }},
                    { le365const::client_mode_5.data(),
                        [this]() { this->ServerAnswer("Mode=5");
                                   this->cpi.Mode(mode_5); }},
                    { le365const::client_mode_6.data(),
                        [this]() { this->ServerAnswer("Mode=6");    
                                   this->cpi.Mode(mode_6); }},
                    { le365const::client_mode_7.data(),
                        [this]() { this->ServerAnswer("Mode=7");
                                   this->cpi.Mode(mode_7); }},
                    { le365const::client_mode_8.data(),
                        [this]() { this->ServerAnswer("Mode=8");
                                   this->cpi.Mode(mode_8); }},
                    { le365const::client_mode_9.data(),
                        [this]() { this->ServerAnswer("Mode=9");
                                   this->cpi.Mode(mode_9); }},
                    { le365const::client_up.data(),
                        [this]() { this->ServerAnswer("Up");
                                   this->cpi.Up(); }},
                    { le365const::client_down.data(),
                        [this]() { this->ServerAnswer("Down");   
                                   this->cpi.Down(); }},
                    { le365const::client_right.data(),
                        [this]() { this->ServerAnswer("Right");     
                                   this->cpi.Right(); }},
                    { le365const::client_left.data(),
                        [this]() { this->ServerAnswer("Left");      
                                   this->cpi.Left(); }},
                    { le365const::client_ok.data(),
                        [this]() { this->ServerAnswer("Ok");
                                   this->cpi.Ok(); }}
                 }
{ 
	cpi.Init(cp);
	Say(le365const::server_welcome.data()); 
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
	if(r < 1) { Halt(); } 
	else {
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
}

void TcpSession::ReadAndCheck()
{
	int r = read(GetFd(), buffer + bufUsed, 
	             sizeof(buffer) - static_cast<size_t>(bufUsed));
	if(r < 1) { Halt(); }
	else {
		bufUsed += r;
		CheckLines();
	}	
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
	if(auto f{ handleMap.find(str) }; f != handleMap.end()) { 
	    f->second(); 
	} else { 
	    ServerAnswer("Unrecognized command");
	}
}

void TcpSession::ServerAnswer(const char *str)
{
	const char *srvStr{ le365const::server_new_line.data() };
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
