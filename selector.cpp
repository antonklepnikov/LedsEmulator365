////////////////////////////////////////////////////////////////////////////////

/*** 
     IMPLEMENTATION:
     Managing modes 
                   ***/


////////////////////////////////////////////////////////////////////////////////

#include "selector.h"


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

FdServer* FdServer::Start(int display, FdSelector *fsl, LEDCore *cp, int port)
{
	int ls{ socket(AF_INET, SOCK_STREAM, 0) };
	if(ls == -1) { return 0; }
	int opt { 1 };
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	int res{ bind(ls, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) };
	if(res == -1) { return 0; }
	res = listen(ls, TCP_QLEN_FOR_LISTEN);
	if(res == -1) { return 0; }
	return new FdServer(display, fsl, cp, ls);
}

FdServer::FdServer(int fdDisp, FdSelector *fsl, LEDCore *cp, int fdSrv) 
	: FdHandler(fdSrv, true), disp(fdDisp), fdsel(fsl), cpi(cp)
{ 
	fdsel->Add(&disp);
	fdsel->Add(this);	
}

FdServer::~FdServer() { fdsel->Remove(this); }

void FdServer::Handle(bool r, [[maybe_unused]] bool w) // LOGGING?!
                                                       // DELETE TCPSESSION?!
{
	if(!r) { return; }
	sockaddr_in addr{};
	socklen_t len{ sizeof(addr) };
	int sd{ accept(GetFd(), reinterpret_cast<sockaddr*>(&addr), &len) };
	if(sd == -1) { return; }
	TcpSession *p = new TcpSession(this, sd);
	fdsel->Add(p);
}

////////////////////////////////////////////////////////////////////////////////

void TcpSession::Say(const char *msg)
{
	write(GetFd(), msg, strlen(msg));
}

TcpSession::TcpSession(FdServer *am, int fd) 
	: FdHandler(fd, true), bufUsed(0), ignoring(false), name(nullptr), 
	srvMaster(am) 
{ 
	Say("You are connected to LEDCore, input command here: "); 
}

TcpSession::~TcpSession() 
{
	if(name) { delete[] name; }
}

void TcpSession::Handle([[maybe_unused]] bool r, [[maybe_unused]] bool w)
{
	Say("This is the temporary loopback for LEDCore interface. ");
}

////////////////////////////////////////////////////////////////////////////////

void DisplaySession::Handle(bool r, [[maybe_unused]] bool w)
{
	if(!r) { return; }
	if(!Fl::check()) { std::exit(0); } //FLAG!!!
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void MainSelector::ModeChooser()
{
    auto mode{ core->GetMode() };
    switch(mode) {
        case mode_1:
            if(!rainbow.GetReady())
            	rainbow.Init(core, mode_1);
            rainbow.Enable();
            break;
        case mode_2:
            if(!rainbowMeteor.GetReady())
            	rainbowMeteor.Init(core, mode_2);
            rainbowMeteor.Enable();
	        break;
        case mode_3:
            if(!rainbowGlitter.GetReady())
            	rainbowGlitter.Init(core, mode_3); 
            rainbowGlitter.Enable();
            break;
        case mode_4:
            if(!stars.GetReady())
            	stars.Init(core, mode_4); 
            stars.Enable();
            break;
        case mode_5:
            if(!runningDots.GetReady())
            	runningDots.Init(core, mode_5); 
            runningDots.Enable();
            break;
        case mode_6:
            if(!pacifica.GetReady())
            	pacifica.Init(core, mode_6); 
            pacifica.Enable();
            break;
        case mode_7:
           if(!rgb.GetReady())
            	rgb.Init(core, mode_7); 
            rgb.Enable();
            break;
        case mode_8:
           if(!cmyk.GetReady())
            	cmyk.Init(core, mode_8); 
            cmyk.Enable();
            break;
        case mode_9:
           if(!white.GetReady())
            	white.Init(core, mode_6); 
            white.Enable();
            break;
        case mode_stop:
            if(!stop.GetReady()) { stop.Init(core, mode_stop); }
            stop.Enable();
            break;
        default:
            break;
    }
}

void MainSelector::Run()
{   
    while(!quit_flag && core->CoreRun()) {
    	
    	//if(!Fl::check()) { BreakLoop(); }
        //if(!fdsel.FdSelReady()) { BreakLoop(); }
    	//fdsel.FdSelect();
        
        tcps->ServerStep();
        ModeChooser();
    }
}


////////////////////////////////////////////////////////////////////////////////
