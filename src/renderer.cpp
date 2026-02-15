// nextbus-esp32-ink -- e-ink display renderer
// Library: GxEPD2 with GxEPD2_213_GDEY0213B74 (exact panel match)
// Rotation: setRotation(1) for landscape -- handled by SSD1680 hardware

#ifndef UNIT_TEST

#include "renderer.h"
#include "config.h"
#include <SPI.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

// --- Display instance ---
// GxEPD2_213_GDEY0213B74: GDEY0213B74 122x250, SSD1680
static GxEPD2_BW<GxEPD2_213_GDEY0213B74, GxEPD2_213_GDEY0213B74::HEIGHT>
    display(GxEPD2_213_GDEY0213B74(EINK_CS, EINK_DC, EINK_RST, EINK_BUSY));

// --- Layout constants (landscape: 250 wide x 122 tall) ---
static const int SCREEN_W = 250;
static const int SCREEN_H = 122;

static const int HEADER_H   = 18;
static const int FOOTER_H   = 16;
static const int ROW_H      = 44;
static const int ROW_Y_START = HEADER_H;

static const int BAR_X      = 10;
static const int BAR_W      = 230;
static const int BAR_H      = 8;
static const int BAR_Y_OFF  = 32;

// --- Drawing helpers ---

static void drawHeader(const BoardData& data, bool wifiConnected) {
    display.setFont(&FreeSansBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(4, 14);
    display.print("NextBus");

    int indicatorX = SCREEN_W - 50;
    int indicatorY = 9;
    if (wifiConnected) {
        display.fillCircle(indicatorX, indicatorY, 4, GxEPD_BLACK);
    } else {
        display.drawCircle(indicatorX, indicatorY, 4, GxEPD_BLACK);
    }

    if (data.as_of[0] != '\0') {
        display.setFont(&FreeSans9pt7b);
        const char* t = strchr(data.as_of, 'T');
        if (t && strlen(t) >= 6) {
            char timeBuf[6];
            strncpy(timeBuf, t + 1, 5);
            timeBuf[5] = '\0';
            int16_t x1, y1; uint16_t w, h;
            display.getTextBounds(timeBuf, 0, 0, &x1, &y1, &w, &h);
            display.setCursor(SCREEN_W - w - 4, 14);
            display.print(timeBuf);
        }
    }

    display.drawLine(0, HEADER_H - 1, SCREEN_W - 1, HEADER_H - 1, GxEPD_BLACK);
}

static void drawRow(const BoardItem& item, int rowIndex) {
    int yBase = ROW_Y_START + (rowIndex * ROW_H);

    display.setFont(&FreeSans9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(4, yBase + 14);

    if (item.status == Status::NoService) {
        display.print(item.label);
        const char* s = "No svc";
        int16_t x1, y1; uint16_t w, h;
        display.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
        display.setCursor(SCREEN_W - w - 4, yBase + 14);
        display.print(s);
        return;
    }

    if (item.status == Status::Error || item.status == Status::Unknown) {
        display.print(item.label);
        const char* s = "Err";
        int16_t x1, y1; uint16_t w, h;
        display.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
        display.setCursor(SCREEN_W - w - 4, yBase + 14);
        display.print(s);
        return;
    }

    // Label
    char labelBuf[24];
    strncpy(labelBuf, item.label, sizeof(labelBuf) - 1);
    labelBuf[sizeof(labelBuf) - 1] = '\0';
    display.print(labelBuf);

    // Minutes (right-aligned, bold)
    char minBuf[12];
    snprintf(minBuf, sizeof(minBuf), "%dm", item.arrival.minutes);
    if (item.status == Status::Stale) {
        snprintf(minBuf, sizeof(minBuf), "%dm*", item.arrival.minutes);
    }
    display.setFont(&FreeSansBold9pt7b);
    int16_t x1, y1; uint16_t tw, th;
    display.getTextBounds(minBuf, 0, 0, &x1, &y1, &tw, &th);
    display.setCursor(SCREEN_W - tw - 4, yBase + 14);
    display.print(minBuf);

    // Approach bar
    int barY = yBase + BAR_Y_OFF;
    display.drawRect(BAR_X, barY, BAR_W, BAR_H, GxEPD_BLACK);

    float fill = barFill(item.arrival.minutes, MAX_WINDOW_MINUTES);
    int fillW = (int)(fill * (BAR_W - 2));
    if (fillW > 0) {
        display.fillRect(BAR_X + 1, barY + 1, fillW, BAR_H - 2, GxEPD_BLACK);
    }

    float threshFill = barFill(LEAVE_THRESHOLD_MINUTES, MAX_WINDOW_MINUTES);
    int tickX = BAR_X + (int)(threshFill * (BAR_W - 2));
    display.drawLine(tickX, barY - 2, tickX, barY + BAR_H + 1, GxEPD_BLACK);
}

static void drawFooter(const BoardData& data) {
    int footerY = SCREEN_H - FOOTER_H;
    display.drawLine(0, footerY, SCREEN_W - 1, footerY, GxEPD_BLACK);

    display.setFont(NULL);  // Default small font
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(4, footerY + 4);

    bool goAlert = false;
    for (int i = 0; i < data.item_count; i++) {
        if ((data.items[i].status == Status::Ok || data.items[i].status == Status::Stale)
            && data.items[i].arrival.valid
            && shouldLeave(data.items[i].arrival.minutes, LEAVE_THRESHOLD_MINUTES)) {
            goAlert = true;
            break;
        }
    }

    if (goAlert) {
        display.print("  GO! Leave now!");
    } else {
        char buf[32];
        snprintf(buf, sizeof(buf), "  Leave <= %dm", LEAVE_THRESHOLD_MINUTES);
        display.print(buf);
    }
}

// --- Public API ---

void displaySetup() {
    // XIAO ESP32-C3: must set SPI pins explicitly before display.init()
    SPI.begin(EINK_SCK, EINK_MISO, EINK_MOSI, EINK_CS);

    display.init(115200);
    display.setRotation(1);  // Landscape (250x122) -- handled by SSD1680 hardware
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.display(false);  // Full refresh to clear

#ifdef DEBUG
    Serial.println("[Display] GxEPD2 GDEY0213B74, landscape 250x122");
#endif
}

void displayRender(const BoardData& data, bool fullRefresh, bool wifiConnected) {
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        drawHeader(data, wifiConnected);

        for (int i = 0; i < data.item_count && i < 2; i++) {
            drawRow(data.items[i], i);
        }

        if (data.item_count == 0) {
            display.setFont(&FreeSans9pt7b);
            display.setCursor(10, SCREEN_H / 2);
            display.print("No stops configured");
        }

        drawFooter(data);
    } while (display.nextPage());
}

void displayShowMessage(const char* msg) {
    display.setFullWindow();
    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        display.setFont(&FreeSansBold9pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(10, SCREEN_H / 2 + 5);
        display.print(msg);
    } while (display.nextPage());
}

#endif // UNIT_TEST
