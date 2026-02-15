#ifndef RENDERER_H
#define RENDERER_H

#include "model.h"

// Initialize the e-ink display.
// Call once in setup().
void displaySetup();

// Render the full board layout on the e-ink display.
// fullRefresh: true = full screen refresh (clears ghosting),
//              false = partial refresh (faster, less flicker).
// wifiConnected: true = show WiFi-OK indicator, false = show WiFi-down indicator.
// See: spec/UX.md for layout and state definitions.
void displayRender(const BoardData& data, bool fullRefresh, bool wifiConnected);

// Show a simple error/status message (for boot, WiFi errors, etc.)
void displayShowMessage(const char* msg);

#endif // RENDERER_H
