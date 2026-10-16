// Screen Addict Freezer - windowed version (Windows only).
// The console version starts in src/main.cpp; both share the same vault.dat.
#include "gui_app.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int showCommand) {
    GuiApp app(instance);
    return app.run(showCommand);
}
