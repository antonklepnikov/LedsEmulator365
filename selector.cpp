////////////////////////////////////////////////////////////////////////////////

/*** 
     IMPLEMENTATION:
     Managing modes 
                   ***/


////////////////////////////////////////////////////////////////////////////////

#include "selector.h"


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

void FdSelector::FdSelRun()
{
	while(!quitFlag) {
		int i{};
		fd_set rds;
	    fd_set wrs;
		FD_ZERO(&rds);
	    FD_ZERO(&wrs);
		timeval to;
	    to.tv_sec = 0;
	    to.tv_usec = 10000;
		for(i = 0; i <= maxFd; ++i) {
			if(fdArray[i]) {
				if(fdArray[i]->WantRead()) { FD_SET(i, &rds); }
				if(fdArray[i]->WantWrite()) { FD_SET(i, &wrs); }
			}
		}
		int res = select(maxFd + 1, &rds, &wrs, 0, &to);
		if(res < 0) {
			if(errno == EINTR) { continue; }
			else { break; }
		}
		if(res > 0) {
			for(i = 0; i <= maxFd; ++i) {
				if(!fdArray[i]) { continue; }
				bool r = FD_ISSET(i, &rds);
				bool w = FD_ISSET(i, &wrs);
				if(r || w) { fdArray[i]->Handle(r, w); }
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////

void Selector::Select()
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

void Selector::Run()
{
    Timer tm;
    auto thc{ true };
    while(!quit_flag && core->CoreRun()) {
        if(thc) {
            if(!Fl::check()) { BreakLoop(); }
            Select();
            tm.Reset();
            thc = false;
        }
        if(tm.Elapsed() >= 0.15) { thc = true; }
    }
}


////////////////////////////////////////////////////////////////////////////////
