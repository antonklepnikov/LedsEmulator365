////////////////////////////////////////////////////////////////////////////////

/*** 
     IMPLEMENTATION:
     Everything related to the graphical user interface 
                                                       ***/


////////////////////////////////////////////////////////////////////////////////

#include "led_gui.h"


////////////////////////////////////////////////////////////////////////////////

Window365 * Window365::Make(LEDCore *core, MainLoop *ml)
{
    static const char* buttons_lbl[] = { "1", "2", "3", "4", "5",
                                         "6", "7", "8", "9" };
    int x, y, i, j, vi, k, ivi;
    const auto shift{ ITEM_SIZE + BREAKUP };
    const auto rows{ NUM_LEDS / NUM_COLUMNS };
    // Main window:
    const auto w{ NUM_COLUMNS * shift + 3 * BREAKUP };
    const auto h{ rows * shift + 3 * ITEM_SIZE + 6 * BREAKUP };
    auto *win{ new Window365(w, h) };
    win->color(FL_GRAY);
    // LEDs:
    for(j = 0; j < rows; ++j) {
        if(j % 2 == 0) {
            for(i = 0; i < NUM_COLUMNS; ++i) {
                vi = (j * NUM_COLUMNS) + i;
                x = (vi < NUM_COLUMNS ? vi : (vi % NUM_COLUMNS)) * shift
                    + 2 * BREAKUP;
                y = vi / NUM_COLUMNS * shift + 2 * BREAKUP;
                core->leds.at(static_cast<size_t>(vi)) = new LED(x, y);
                }
        } else {
            for(i = NUM_COLUMNS - 1, k = 0; i >= 0; --i, k += 2) {
                vi = (j * NUM_COLUMNS) + i;
                ivi = vi - NUM_COLUMNS + 1 + k;
                x = (vi < NUM_COLUMNS ? vi : (vi % NUM_COLUMNS)) * shift
                    + 2 * BREAKUP;
                y = vi / NUM_COLUMNS * shift + 2 * BREAKUP;
                core->leds.at(static_cast<size_t>(ivi)) = new LED(x, y);
            }
        }
    }
    // Digit buttons;
    for(i = 0; i < NUM_DIG_BUTTONS; ++i) {
        x = (i < 3 ? i : (i % 3)) * shift + 2 * BREAKUP;
        y = ((i / 3) * shift + BREAKUP) + (rows * shift + 2 * BREAKUP);
        ml->buttons.at(static_cast<size_t>(i)) = 
            new ModeButton(x, y, buttons_lbl[i], 
                           core, static_cast<enum_mode>(i+1));
    }
    // Control buttons;
    x = w - (2 * ITEM_SIZE + 3 * BREAKUP);
    y = rows * shift + 3 * BREAKUP;
    ml->buttons.at(static_cast<size_t>(B_UP)) = 
        new BrightUpButton (x, y, "@8->", core);
    x -= shift; y += shift;
    ml->buttons.at(static_cast<size_t>(B_LEFT)) = 
        new BrowseLeftButton (x, y, "@undo", core);
    x += shift;
    ml->buttons.at(static_cast<size_t>(B_OK)) = 
        new OkButton (x, y, "@-2circle", core);
    x += shift;
    ml->buttons.at(static_cast<size_t>(B_RIGHT)) = 
        new BrowseRightButton (x, y, "@redo", core);
    x -= shift; y += shift;
    ml->buttons.at(static_cast<size_t>(B_DOWN)) = 
        new BrightDownButton (x, y, "@2->", core);
    win->end();
    return win;
}


////////////////////////////////////////////////////////////////////////////////

int KeyHandler::key_handle(int event) {
    if (event == FL_SHORTCUT) {
        switch(Fl::event_key()) {
            case '1':
                cpi.Mode(mode_1);    return 1;
            case '2':
                cpi.Mode(mode_2);    return 1;
            case '3':
                cpi.Mode(mode_3);    return 1;
            case '4':
                cpi.Mode(mode_4);    return 1;
            case '5':
                cpi.Mode(mode_5);    return 1;
            case '6':
                cpi.Mode(mode_6);    return 1;
            case '7':
                cpi.Mode(mode_7);    return 1;
            case '8':
                cpi.Mode(mode_8);    return 1;
            case '9':
                cpi.Mode(mode_9);    return 1;
            case 'w':
                cpi.Up();            return 1;
            case 'a':
                cpi.Left();          return 1;
            case 's':
                cpi.Ok();            return 1;
            case 'd':
                cpi.Right();         return 1;
            case 'x':
                cpi.Down();          return 1;                                                
            default:
                return 0;
        }
    } 
    else { return 0; }    
}


////////////////////////////////////////////////////////////////////////////////