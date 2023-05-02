#ifndef OOFL_AK_H
#define OOFL_AK_H


////////////////////////////////////////////////////////////////////////////////

/*** 
    Intermediate layer for FLTK libray classes 
    with a different organization of work 
                                         ***/


////////////////////////////////////////////////////////////////////////////////

#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>


////////////////////////////////////////////////////////////////////////////////

class OOFLWindow : public Fl_Window {
public:
    OOFLWindow(int x, int y, int w, int h, const char *lb)
        : Fl_Window(x, y, w, h, lb) {} 
};


class OOFLBox : public Fl_Box {
public:
    OOFLBox(int x, int y, int w, int h) : Fl_Box(x, y, w, h) {}
};


class OOFLButton : public Fl_Button {
public:
    OOFLButton(int x, int y, int w, int h, const char *lb)
        : Fl_Button(x, y, w, h, lb) { callback(CallbackFunction, 0); }
    virtual ~OOFLButton() = default;
    virtual void OnPress() = 0;
private:
    static void CallbackFunction(Fl_Widget *widg, 
                                 [[maybe_unused]] void *user) 
    { 
        static_cast<OOFLButton*>(widg)->OnPress();
    }
};


////////////////////////////////////////////////////////////////////////////////

#endif
