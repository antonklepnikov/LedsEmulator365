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
    /* 
    static const char* buttons_lbl[] = { "1", "2", "3", "4", "5",
                                         "6", "7", "8", "9" };
    */
    auto buttons_lbl{ std::to_array<std::string_view>({"1", "2", "3", "4", "5",
                                                      "6", "7", "8", "9"}) }; 
    
    int x, y, i, j, vi, k, ivi;
    const auto shift{ le365const::item_size + le365const::breakup };
    const auto rows{ le365const::num_leds / le365const::num_columns };
    // Main window:
    const auto w{ le365const::num_columns * shift + 3 * le365const::breakup };
    const auto h{ rows * shift + 3 * le365const::item_size + 6 
                    * le365const::breakup };
    auto *win{ new Window365(w, h) };
    win->color(FL_GRAY);
    // LEDs:
    for(j = 0; j < rows; ++j) {
        if(j % 2 == 0) {
            for(i = 0; i < le365const::num_columns; ++i) {
                vi = (j * le365const::num_columns) + i;
                x = (vi < le365const::num_columns ? vi :
                        (vi % le365const::num_columns)) * shift + 2 
                            * le365const::breakup;
                y = vi / le365const::num_columns * shift + 2 
                        * le365const::breakup;
                core->leds.at(static_cast<size_t>(vi)) = new LED(x, y);
                }
        } else {
            for(i = le365const::num_columns - 1, k = 0; i >= 0; --i, k += 2) {
                vi = (j * le365const::num_columns) + i;
                ivi = vi - le365const::num_columns + 1 + k;
                x = (vi < le365const::num_columns ? vi :
                        (vi % le365const::num_columns)) * shift + 2 
                            * le365const::breakup;
                y = vi / le365const::num_columns * shift + 2 
                        * le365const::breakup;
                core->leds.at(static_cast<size_t>(ivi)) = new LED(x, y);
            }
        }
    }
    // Digit buttons;
    for(i = 0; i < le365const::num_dig_buttons; ++i) {
        x = (i < 3 ? i : (i % 3)) * shift + 2 * le365const::breakup;
        y = ((i / 3) * shift + le365const::breakup) + (rows * shift + 2
                * le365const::breakup);
        ml->buttons.at(static_cast<size_t>(i)) = 
            new ModeButton(x, y, buttons_lbl.at(static_cast<size_t>(i)).data(), 
                           core, static_cast<enum_mode>(i+1));
    }
    // Control buttons;
    x = w - (2 * le365const::item_size + 3 * le365const::breakup);
    y = rows * shift + 3 * le365const::breakup;
    ml->buttons.at(static_cast<size_t>(le365const::b_up)) = 
        new BrightUpButton (x, y, "@8->", core);
    x -= shift; y += shift;
    ml->buttons.at(static_cast<size_t>(le365const::b_left)) = 
        new BrowseLeftButton (x, y, "@undo", core);
    x += shift;
    ml->buttons.at(static_cast<size_t>(le365const::b_ok)) = 
        new OkButton (x, y, "@-2circle", core);
    x += shift;
    ml->buttons.at(static_cast<size_t>(le365const::b_right)) = 
        new BrowseRightButton (x, y, "@redo", core);
    x -= shift; y += shift;
    ml->buttons.at(static_cast<size_t>(le365const::b_down)) = 
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