////////////////////////////////////////////////////////////////////////////////

/*** 
     IMPLEMENTATION:
     Everything related to the graphical user interface 
                                                       ***/


////////////////////////////////////////////////////////////////////////////////

#include "led_gui.h"


////////////////////////////////////////////////////////////////////////////////

void BrightUpButton::OnPress()
{
    auto b{ core->GetBright() };
    b += STEP_BRIGHT;
    if(b > MAX_BRIGHT) {
        core->SetBright(MAX_BRIGHT);
    } else {
        core->SetBright(b);
    }
}


void BrightDownButton::OnPress()
{
    auto b{ core->GetBright() };
    b -= STEP_BRIGHT;
    if(b < MIN_BRIGHT) {
        core->SetBright(MIN_BRIGHT);
    } else {
        core->SetBright(b);
    }
}


Window365 * Window365::Make(LEDCore *core, Selector *sel)
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
        sel->buttons.at(static_cast<size_t>(i)) = 
            new ModeButton(x, y, buttons_lbl[i], 
                           core, static_cast<enum_mode>(i+1));
    }
    // Control buttons;
    x = w - (2 * ITEM_SIZE + 3 * BREAKUP);
    y = rows * shift + 3 * BREAKUP;
    sel->buttons.at(static_cast<size_t>(B_UP)) = 
        new BrightUpButton (x, y, "@8->", core);
    x -= shift; y += shift;
    sel->buttons.at(static_cast<size_t>(B_LEFT)) = 
        new BrowseLeftButton (x, y, "@undo", core);
    x += shift;
    sel->buttons.at(static_cast<size_t>(B_OK)) = 
        new OkButton (x, y, "@-2circle", core);
    x += shift;
    sel->buttons.at(static_cast<size_t>(B_RIGHT)) = 
        new BrowseRightButton (x, y, "@redo", core);
    x -= shift; y += shift;
    sel->buttons.at(static_cast<size_t>(B_DOWN)) = 
        new BrightDownButton (x, y, "@2->", core);
    win->end();
    return win;
}


////////////////////////////////////////////////////////////////////////////////
