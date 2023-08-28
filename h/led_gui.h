#ifndef LED_GUI_AK_H
#define LED_GUI_AK_H


////////////////////////////////////////////////////////////////////////////////

/***
    Everything related to the graphical user interface
                                                      ***/


////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "led_core.h"
#include "oofl.h"
#include "main_loop.h"

#include <FL/Fl.H>
#include <FL/platform.H>
#include <X11/Xlib.h>

                                    
////////////////////////////////////////////////////////////////////////////////
/// CLASS: LED /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class LED : public OOFLBox {
public:
    LED(int x, int y) : OOFLBox(x, y, ITEM_SIZE, ITEM_SIZE) 
    {
        box(FL_OVAL_BOX);
        color(FL_BLACK);
    }
};


////////////////////////////////////////////////////////////////////////////////
/// CLASS: Pult and its heirs //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Pult : public OOFLButton {
protected:
    LEDCore *core;
public:
    Pult(int x, int y, const char *lb, LEDCore *c)
        : OOFLButton(x, y, ITEM_SIZE, ITEM_SIZE, lb), core(c) 
    {
        box(FL_FLAT_BOX);
        color(FL_GRAY);
        labelcolor(FL_BLACK);
        down_color(FL_DARK1);
    }
    // No copying and assignment:
    Pult(const Pult&) = delete;
    Pult& operator=(const Pult&) = delete;
};

class ModeButton : public Pult {
private:
    enum_mode mode;
public:
    ModeButton(int x, int y, const char *lb, LEDCore *c, enum_mode m)
        : Pult(x, y, lb, c), mode(m)
            { labelsize(FONT_SIZE_DIGIT_BUTTON); }
    virtual void OnPress() { core->SetMode(mode); }
};

class OkButton : public Pult {
public:
    OkButton(int x, int y, const char *lb, LEDCore *c)
        : Pult(x, y, lb, c) { labelsize(FONT_SIZE_CONTROL_BUTTON); }
    virtual void OnPress() { core->StopMode();  }
};

class BrightUpButton : public Pult {
public:
    BrightUpButton(int x, int y, const char *lb, LEDCore *c)
        : Pult(x, y, lb, c) { labelsize(FONT_SIZE_CONTROL_BUTTON); }
    virtual void OnPress() { core->BrightUp(); }
};

class BrightDownButton : public Pult {
public:
    BrightDownButton(int x, int y, const char *lb, LEDCore *c)
        : Pult(x, y, lb, c) { labelsize(FONT_SIZE_CONTROL_BUTTON); }
    virtual void OnPress() { core->BrightDown(); }
};


class BrowseLeftButton : public Pult {
public:
    BrowseLeftButton(int x, int y, const char *lb, LEDCore *c)
        : Pult(x, y, lb, c) { labelsize(FONT_SIZE_CONTROL_BUTTON); }
    virtual void OnPress() { core->PrevMode(); }
};

class BrowseRightButton : public Pult {
public:
    BrowseRightButton(int x, int y, const char *lb, LEDCore *c)
        : Pult(x, y, lb, c) { labelsize(FONT_SIZE_CONTROL_BUTTON); }
    virtual void OnPress() { core->NextMode(); }
};

////////////////////////////////////////////////////////////////////////////////
/// CLASS: Window365 ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Window365 : public OOFLWindow {
private:
    Window365(int w, int h) :
        OOFLWindow(0, 0, w, h, "< LEDs emulator 365 >") {}
public:
    static Window365 * Make(LEDCore *core, MainLoop *ml); // led_gui.cpp
};


////////////////////////////////////////////////////////////////////////////////
/// Keyboard events handling ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class KeyHandler {
private:
    static int key_handle(int event);
public:
    static CorePultInterface cpi;
    KeyHandler() { Fl::add_handler(key_handle); }
};


////////////////////////////////////////////////////////////////////////////////

#endif