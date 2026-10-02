/*
 * Rad Pro
 * UI system
 *
 * (C) 2022-2026 Gissio
 *
 * License: MIT
 */

#include "../measurements/measurements.h"
#include "../peripherals/rtc.h"
#include "../system/power.h"
#include "../system/settings.h"
#include "../ui/draw.h"
#include "../ui/system.h"

static bool layoutInvalid = true;
static uint8_t paintedTheme = UINT8_MAX;
static uint32_t paintedHeader = UINT32_MAX;

void invalidateDisplayLayout(void)
{
    layoutInvalid = true;
}

void drawPowerOff(bool displayBatteryIcon)
{
    invalidateDisplayLayout();
    char buffer[16];

    setFillColor(COLOR_CONTAINER_BACKGROUND);
    if (displayBatteryIcon)
        setupBatteryIcon(buffer);
    else
        strclr(buffer);
    drawCenteredText(buffer,
                     &(mr_rectangle_t){0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT},
                     &(mr_point_t){(DISPLAY_WIDTH / 2), ((DISPLAY_HEIGHT - FONT_SYMBOLS_LINE_HEIGHT) / 2)});
}

static const ColorIndex alertLevelColorIndex[] = {
    COLOR_ELEMENT_NEUTRAL,
    COLOR_WARNING,
    COLOR_ALARM,
};

bool drawTitleBar(const char *title)
{
    char buffer[16];
    RTCDateTime dateTime;
    getDeviceDateTime(&dateTime);
    uint32_t header = ((dateTime.year >= RTC_YEAR_MIN) ?
                       dateTime.hour * 60 + dateTime.minute : 2047) |
                      ((uint32_t)getBatteryLevel() << 11) |
                      ((uint32_t)isBatteryCharging() << 14) |
                      ((uint32_t)isUSBPowered() << 15) |
                      ((uint32_t)getAlertLevel() << 16) |
                      ((uint32_t)isAlertEnabled() << 18) |
                      ((uint32_t)isAlertFlashing() << 19) |
                      ((uint32_t)isAlertPending() << 20) |
                      ((uint32_t)isLockModeEnabled() << 21) |
                      ((uint32_t)isSoundIconActive() << 22) |
                      ((uint32_t)settings.pulseSound << 23) |
                      ((uint32_t)settings.rtcTimeFormat << 24);
    bool full = layoutInvalid || (paintedTheme != settings.displayTheme);
    bool headerChanged = full || (paintedHeader != header);
    layoutInvalid = false;
    paintedTheme = settings.displayTheme;
    paintedHeader = header;

    setFillColor(COLOR_CONTAINER_BACKGROUND);
    if (!headerChanged)
        return false;

    mr_rectangle_t rectangle = {
        TITLEBAR_WIDTH,
        TITLEBAR_TOP,
        0,
        TITLEBAR_CONTENT_HEIGHT,
    };
    // Clear the content only when the layout or palette changes. The LCD has
    // no framebuffer, so clearing it on each sample visibly blanks the screen.
    if (full)
        drawRectangle(&contentRectangle);
    setFillColor(COLOR_CONTAINER_GLOBAL);
    drawRectangle(&(mr_rectangle_t){0, 0, DISPLAY_WIDTH, TITLEBAR_HEIGHT});

    // Time
    if (dateTime.year >= RTC_YEAR_MIN)
    {
        strclr(buffer);
        if (settings.rtcTimeFormat == RTC_TIMEFORMAT_24_HOUR)
            strcatUInt32(buffer, dateTime.hour, 2);
        else
        {
            uint32_t hour = dateTime.hour % 12;
            strcatUInt32(buffer, (hour == 0) ? 12 : hour, 1);
        }
        strcatChar(buffer, ':');
        strcatUInt32(buffer, dateTime.minute, 2);

        setFont(font_small);
        setStrokeColor(COLOR_ELEMENT_ACTIVE);
        drawRowRight(buffer, &rectangle);
    }

    // Battery icon
    setupBatteryIcon(buffer);
    drawRowRight(buffer, &rectangle);

    // Alert icon
    AlertLevel alertLevel = getAlertLevel();
    if (isAlertEnabled() || alertLevel)
    {
        bool notificationIconFilled;
        if (!isAlertFlashing())
        {
            alertLevel = ALERTLEVEL_NONE;
            notificationIconFilled = false;
        }
        else
            notificationIconFilled = isAlertPending();

        setStrokeColor(alertLevelColorIndex[alertLevel]);
        drawRowRight(notificationIconFilled ? ";" : ":", &rectangle);
    }

    // Lock icon
    setStrokeColor(COLOR_ELEMENT_NEUTRAL);
    if (isLockModeEnabled())
        drawRowRight("<", &rectangle);

    // Sound icon
    if (isSoundIconActive())
        drawRowRight(settings.pulseSound ? "9" : "8", &rectangle);

    // Title
    setFont(font_small);
    setStrokeColor(COLOR_ELEMENT_ACTIVE);
    drawRowLeft(title, &rectangle);

    // Shadow
#if TITLEBAR_SHADOW_HEIGHT > 0
    setFillColor(COLOR_CONTAINER_GLOBAL_SHADOW);
    drawRectangle(&(mr_rectangle_t){TITLEBAR_LEFT, TITLEBAR_BOTTOM - TITLEBAR_SHADOW_HEIGHT, TITLEBAR_WIDTH, 1});
#endif

    // Set background
    setFillColor(COLOR_CONTAINER_BACKGROUND);
    return full;
}

void drawSplash(const char *message)
{
    setFillColor(COLOR_CONTAINER_GLOBAL);

    drawCenteredMultilineText(&(mr_rectangle_t){0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT},
                              message);
}

void drawNotification(const char *title, const char *message)
{
    drawTitleBar(title);

    drawCenteredMultilineText(&contentRectangle, message);
}
