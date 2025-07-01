#include <furi.h>
#include <gui/gui.h>

typedef struct {
    FuriTimer* ok_timer;
    bool running;
    Gui* gui;
    ViewPort* view_port;
} FreezeApp;

// Timer callback for spamming OK button
static void ok_timer_callback(void* ctx) {
    UNUSED(ctx);  // Mark context as unused
    
    // Create OK press event
    InputEvent press = {
        .key = InputKeyOk,
        .type = InputTypePress,
        .sequence = 0
    };
    
    // Create OK release event
    InputEvent release = {
        .key = InputKeyOk,
        .type = InputTypeRelease,
        .sequence = 0
    };
    
    // Inject both events directly to input queue
    FuriMessageQueue* input_queue = furi_record_open(RECORD_INPUT_EVENTS);
    furi_message_queue_put(input_queue, &press, FuriWaitForever);
    furi_message_queue_put(input_queue, &release, FuriWaitForever);
    furi_record_close(RECORD_INPUT_EVENTS);
}

int32_t freeze_app(void* p) {
    UNUSED(p);
    
    // Allocate application state
    FreezeApp* app = malloc(sizeof(FreezeApp));
    app->running = true;
    
    // Immediately hold Back button
    InputEvent back_press = {
        .key = InputKeyBack,
        .type = InputTypePress,
        .sequence = 0
    };
    FuriMessageQueue* input_queue = furi_record_open(RECORD_INPUT_EVENTS);
    furi_message_queue_put(input_queue, &back_press, FuriWaitForever);
    
    // Start OK spam after 500ms
    app->ok_timer = furi_timer_alloc(ok_timer_callback, FuriTimerTypePeriodic, NULL);
    furi_timer_start(app->ok_timer, 200);  // Spam every 200ms after initial delay
    
    // Create minimal GUI (required for app to run)
    app->view_port = view_port_alloc();
    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    return 0;
}
