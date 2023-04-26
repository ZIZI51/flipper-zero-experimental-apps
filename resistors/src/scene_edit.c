#include "flipper.h"
#include "resistors_app.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_edit.h"

/* edit view scene */

const int resistor_image_top = 23;
const int resistor_image_le = 23;

const int resistor_image_left_R4 = 5;
const int resistor_image_top_R4 = 23;
const int resistor_arrow_left_R4 = 41;
const int resistor_arrow_top_R4 = 13;
const int arrow_positions_R4[] = {0, 13, 24, 36};

const int resistor_image_left_R5 = 5;
const int resistor_image_top_R5 = 23;
const int resistor_arrow_left_R5 = 48;
const int resistor_arrow_top_R5 = 13;
const int arrow_positions_R5[] = {0, 7, 15, 25, 32};

// Xx_Xx_Xx_Xx_-_Xx\0 = 17 characters
char blank_descriptor[17] = "                ";
char resistor_descriptor[17] = "                ";

char resistance_calculation[] = "uncalculated";

void update_resistor_descriptor(ResistorType bands, int resistor_bands[], char descriptor[]) {
    strcpy(descriptor, blank_descriptor);
    for(int i = 0; i < bands; i++) {
        int c = i * 3;
        if(i == bands - 1) {
            descriptor[c] = '-';
            c += 2;
        }
        switch(resistor_bands[i]) {
        case BandBlack:
            descriptor[c] = 'B';
            descriptor[c + 1] = 'l';
            break;
        case BandBrown:
            descriptor[c] = 'B';
            descriptor[c + 1] = 'r';
            break;
        case BandRed:
            descriptor[c] = 'R';
            descriptor[c + 1] = 'e';
            break;
        case BandOrange:
            descriptor[c] = 'O';
            descriptor[c + 1] = 'r';
            break;
        case BandYellow:
            descriptor[c] = 'Y';
            descriptor[c + 1] = 'e';
            break;
        case BandGreen:
            descriptor[c] = 'G';
            descriptor[c + 1] = 'r';
            break;
        case BandBlue:
            descriptor[c] = 'B';
            descriptor[c + 1] = 'l';
            break;
        case BandPurple:
            descriptor[c] = 'P';
            descriptor[c + 1] = 'u';
            break;
        case BandGray:
            descriptor[c] = 'G';
            descriptor[c + 1] = 'y';
            break;
        case BandWhite:
            descriptor[c] = 'W';
            descriptor[c + 1] = 'h';
            break;
        case BandGold:
            descriptor[c] = 'G';
            descriptor[c + 1] = 'o';
            break;
        case BandSilver:
            descriptor[c] = 'S';
            descriptor[c + 1] = 'i';
            break;
        }
    } // i
}

void resistors_edit_view_redraw_widget(App* app) {
    widget_reset(app->widget);

    int* arrow_positions;
    int arrow_left, arrow_top, image_left, image_top;
    Icon* icon;

    switch(app->state->resistor_type) {
    case Resistor4Band:
        arrow_left = resistor_arrow_left_R4;
        arrow_top = resistor_arrow_top_R4;
        arrow_positions = (int*)arrow_positions_R4;
        image_left = resistor_image_left_R4;
        image_top = resistor_image_top_R4;
        icon = (Icon*)&I_resistor_4;
        break;
    case Resistor5Band:
        arrow_left = resistor_arrow_left_R5;
        arrow_top = resistor_arrow_top_R5;
        arrow_positions = (int*)arrow_positions_R5;
        image_left = resistor_image_left_R5;
        image_top = resistor_image_top_R5;
        icon = (Icon*)&I_resistor_5;
        break;
    default:
        FURI_LOG_E(TAG, "Unrecognised resistor type in resistors_edit_view_redraw_widget");
        app_quit(app);
        return;
    }

    // render resistor graphic
    widget_add_icon_element(app->widget, image_left, image_top, icon);

    // render arrow
    if(app->state->edit_selection < app->state->resistor_type) {
        widget_add_icon_element(
            app->widget,
            arrow_left + *(arrow_positions + app->state->edit_selection),
            arrow_top,
            &I_arrow);
    }

    // render descriptor
    update_resistor_descriptor(
        app->state->resistor_type, app->state->resistor_bands, resistor_descriptor);
    widget_add_text_box_element(
        app->widget, 5, 50, 123, 16, AlignCenter, AlignBottom, resistor_descriptor, true);

    // render calculation
    widget_add_text_box_element(
        app->widget, 5, 2, 123, 10, AlignCenter, AlignCenter, resistance_calculation, true);

    // widget_add_button_element(app->widget, GuiButtonTypeCenter, buttonText, callback, app);
}

/** main menu events */

static bool widget_input_callback(InputEvent* input_event, void* context) {
    App* app = context;
    bool consumed = false;
    if(input_event->type == InputTypeShort) {
        switch(input_event->key) {
        case InputKeyRight:
            if(app->state->edit_selection < app->state->resistor_type - 1) {
                app->state->edit_selection += 1;
            }
            resistors_edit_view_redraw_widget(app);
            consumed = true;
            break;
        case InputKeyLeft:
            if(app->state->edit_selection > 0) {
                app->state->edit_selection -= 1;
            }
            resistors_edit_view_redraw_widget(app);
            consumed = true;
            break;
        default:
            consumed = false;
            break;
        }
    }
    return consumed;
}

/** edit view scene - resets the widget, and gives it content, callbacks and selection enums */
void resistors_edit_scene_on_enter(void* context) {
    App* app = context;
    resistors_edit_view_redraw_widget(app);
    view_set_context(widget_get_view(app->widget), app);
    view_set_input_callback(widget_get_view(app->widget), widget_input_callback);
    view_dispatcher_switch_to_view(app->view_dispatcher, ResistorsEditView);
}

/** edit view event handler - switches scene based on the event */
bool resistors_edit_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);
    // if(event.type == SceneManagerEventTypeCustom && event.event == ResistorsEditViewUpdateEvent) {
    //     ...
    //     consumed = true;
    // }
    return consumed;
}

void resistors_edit_scene_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}
