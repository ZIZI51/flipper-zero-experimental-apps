#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

/* generated by fbt from .png files in images folder */
#include <resistors_icons.h>

/** The current scene */
typedef enum { ResistorsMainMenuScene, ResistorsSceneCount } ResistorsScene;

/** The current view */
typedef enum { ResistorsSubmenuView } ResistorsView;

/** The app structure - contains a SceneManager, ViewDispatcher, menu */
typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    // Widget* widget;
    // TextInput* text_input;
} App;

/* main menu scene */

/** main menu options enum */
typedef enum {
    ResistorsMainMenuScene3BarResistor,
    ResistorsMainMenuScene4BarResistor,
} ResistorsMainMenuSceneIndex;

/** main menu events */
typedef enum {
    ResistorsMainMenuScene3BarSelectionEvent,
    ResistorsMainMenuScene4BarSelectionEvent,
} ResistorsMainMenuEvent;

/** main menu callback - sends custom events to the scene manager based on the selection */
void resistors_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case ResistorsMainMenuScene3BarResistor:
        scene_manager_handle_custom_event(
            app->scene_manager, ResistorsMainMenuScene3BarSelectionEvent);
        break;
    case ResistorsMainMenuScene4BarResistor:
        scene_manager_handle_custom_event(
            app->scene_manager, ResistorsMainMenuScene4BarSelectionEvent);
        break;
    }
}
/** main menu scene - resets the submenu, and gives it content, callbacks and selection enums */
void resistors_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Resistors");
    submenu_add_item(
        app->submenu,
        "3-bar resistor",
        ResistorsMainMenuScene3BarResistor,
        resistors_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "4-bar resistor",
        ResistorsMainMenuScene4BarResistor,
        resistors_menu_callback,
        app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ResistorsSubmenuView);
}

/** main menu event handler - switches scene based on the event */
bool resistors_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case ResistorsMainMenuScene3BarSelectionEvent:
            scene_manager_next_scene(
                app->scene_manager, ResistorsMainMenuScene); // TODO: pick a different scene
            consumed = true;
            break;
        case ResistorsMainMenuScene4BarSelectionEvent:
            scene_manager_next_scene(
                app->scene_manager, ResistorsMainMenuScene); // TODO: pick a different scene
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}

void resistors_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}

// TODO: stub out all other scenes

/* handlers */

/** collection of all scene on_enter handlers */
void (*const resistors_scene_on_enter_handlers[])(void*) = {
    resistors_main_menu_scene_on_enter
    // TODO: add on enter handlers
};

/** collection of all scene on event handlers */
bool (*const resistors_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    resistors_main_menu_scene_on_event
    // TODO: add other on event handlers
};

/** collection of all scene on exit handlers */
void (*const resistors_scene_on_exit_handlers[])(void*) = {
    resistors_main_menu_scene_on_exit
    // TODO: add other event handlers
};

/* handlers */

/** collection of all on_enter, on_event, on_exit handlers */
static const SceneManagerHandlers resistors_scene_manager_handlers = {
    .on_enter_handlers = resistors_scene_on_enter_handlers,
    .on_event_handlers = resistors_scene_on_event_handlers,
    .on_exit_handlers = resistors_scene_on_exit_handlers,
    .scene_num = ResistorsSceneCount, // the last value in the enum == the number of scenes
};

/* callbacks */

static bool resistors_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool resistors_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

/* alloc and free */

static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->scene_manager = scene_manager_alloc(&resistors_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, resistors_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, resistors_back_event_callback);
    app->submenu = submenu_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher, ResistorsSubmenuView, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    // TODO: add other views
    return app;
}

static void app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, ResistorsSubmenuView);
    // TODO: remove other views
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    text_input_free(app->text_input);
    free(app);
}

/* entrypoint */

int32_t resistors_app(void* p) {
    UNUSED(p);
    FURI_LOG_I("TEST", "resistors app launched");
    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, ResistorsMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}
