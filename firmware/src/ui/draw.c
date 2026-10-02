/*
 * Rad Pro
 * UI draw
 *
 * (C) 2022-2026 Gissio
 *
 * License: MIT
 */

#include <limits.h>

#include <mcu-renderer.h>

#include FONT_SYMBOLS
#include FONT_LARGE
#include FONT_MEDIUM
#include FONT_SMALL

#include "../system/cstring.h"
#include "../system/power.h"
#include "../system/settings.h"
#include "../ui/draw.h"

#if defined(DISPLAY_COLOR)

// Online color blender: https://pinetools.com/blend-colors
// Online RGB565 color picker: https://rgbcolorpicker.com/565

static const mr_color_t displayColors[][3] = {
    // Green, blue, orange: all three are dark terminal palettes.
    {mr_get_color(0xd7f5dd), mr_get_color(0xdbeeff), mr_get_color(0xffe8c3)}, // Active text
    {mr_get_color(0x85ac91), mr_get_color(0x91abc7), mr_get_color(0xc49e76)}, // Muted text
    {mr_get_color(0x0c1d14), mr_get_color(0x102033), mr_get_color(0x24170d)}, // Header/panel
    {mr_get_color(0x244b34), mr_get_color(0x254869), mr_get_color(0x614121)}, // Border
    {mr_get_color(0x07120c), mr_get_color(0x07121f), mr_get_color(0x160d07)}, // Background
    {mr_get_color(0xd7f5dd), mr_get_color(0xdbeeff), mr_get_color(0xffe8c3)}, // Menu text
    {mr_get_color(0x173c27), mr_get_color(0x193d5a), mr_get_color(0x54351b)}, // Selection
    {mr_get_color(0x77eda2), mr_get_color(0x71cfff), mr_get_color(0xffbc69)}, // Selection text
    {mr_get_color(0x77eda2), mr_get_color(0x71cfff), mr_get_color(0xffbc69)}, // Running
    {mr_get_color(0xffca62), mr_get_color(0xffca62), mr_get_color(0xffdc7a)}, // Warning
    {mr_get_color(0xff6374), mr_get_color(0xff6374), mr_get_color(0xff6374)}, // Alarm
    {mr_get_color(0x244b34), mr_get_color(0x254869), mr_get_color(0x614121)}, // Frame primary
    {mr_get_color(0x173c27), mr_get_color(0x193d5a), mr_get_color(0x54351b)}, // Frame tertiary
    {mr_get_color(0x85ac91), mr_get_color(0x91abc7), mr_get_color(0xc49e76)}, // Tick label
    {mr_get_color(0x77eda2), mr_get_color(0x71cfff), mr_get_color(0xffbc69)}, // Data accent
    {mr_get_color(0x173c27), mr_get_color(0x193d5a), mr_get_color(0x54351b)}, // Data accent bg
    {mr_get_color(0xffca62), mr_get_color(0xffca62), mr_get_color(0xffdc7a)}, // Warning data
    {mr_get_color(0x4d3317), mr_get_color(0x4d3317), mr_get_color(0x593d1a)}, // Warning bg
    {mr_get_color(0xff6374), mr_get_color(0xff6374), mr_get_color(0xff6374)}, // Alarm data
    {mr_get_color(0x4c1822), mr_get_color(0x4c1822), mr_get_color(0x4c1822)}, // Alarm bg

#if defined(GAME)
    // Selected enabled background [game square selected]
    {mr_get_color(0x2d548b),
     mr_get_color(0x4c6c9a),
     mr_get_color(0x1e3e32)},

    // Game square black
    {mr_get_color(0xb89070),
     mr_get_color(0xb89070),
     mr_get_color(0x48341f)},

    // Game square white
    {mr_get_color(0xd0b088),
     mr_get_color(0xd0b088),
     mr_get_color(0x78532c)},

    // Game piece black
    {mr_get_color(0x000000),
     mr_get_color(0x000000),
     mr_get_color(0x000000)},

    // Game piece white
    {mr_get_color(0xffffff),
     mr_get_color(0xffffff),
     mr_get_color(0xeaa75e)},
#endif
};

#endif

// Definitions

const mr_rectangle_t contentRectangle = {
    CONTENT_LEFT,
    CONTENT_TOP,
    CONTENT_WIDTH,
    CONTENT_HEIGHT,
};

mr_t mr;

// Low-level functions

mr_color_t getFillColor(ColorIndex colorIndex)
{
#if defined(DISPLAY_MONOCHROME)
    if ((colorIndex == COLOR_AMPLIFIED_ENABLED_BACKGROUND) ||
        (colorIndex == COLOR_INSTRUMENT_FRAME_TERTIARY) ||
        (colorIndex == COLOR_INSTRUMENT_TICK_MARK_LABEL_SECONDARY) ||
        (colorIndex == COLOR_INSTRUMENT_ENHANCED_SECONDARY) ||
        (colorIndex == COLOR_INSTRUMENT_ENHANCED_TERTIARY) ||
        (colorIndex == COLOR_GAME_SQUARE_BLACK))
        return 0xffff;
    else
        return 0x0000;
#elif defined(DISPLAY_COLOR)
    return displayColors[colorIndex][settings.displayTheme];
#endif
}

void setFillColor(ColorIndex colorIndex)
{
#if defined(DISPLAY_MONOCHROME)
    mr_color_t fillColor = getFillColor(colorIndex);
    mr_color_t strokeColor = fillColor ? 0x0000 : 0xffff;

    mr_set_fill_color(&mr, fillColor);
    mr_set_stroke_color(&mr, strokeColor);
#elif defined(DISPLAY_COLOR)
    mr_set_fill_color(&mr, getFillColor(colorIndex));
#endif
}

void setStrokeColor(ColorIndex colorIndex)
{
#if defined(DISPLAY_COLOR)
    mr_set_stroke_color(&mr, getFillColor(colorIndex));
#endif
}

void drawRectangle(const mr_rectangle_t *rectangle)
{
    mr_draw_rectangle(&mr, rectangle);
}

#if defined(DISPLAY_MONOCHROME)
void ditherRectangle(const mr_rectangle_t *rectangle, bool dark)
{
    mr_rectangle_t r;
    r.width = 1;
    r.height = 1;

    setFillColor(COLOR_CONTAINER_GLOBAL);

    for (r.y = rectangle->y;
         r.y < (rectangle->y + rectangle->height);
         r.y++)
    {
        for (r.x = rectangle->x;
             r.x < (rectangle->x + rectangle->width);
             r.x++)
        {
            if (dark)
            {
                if ((r.x ^ r.y) & 0b1)
                    continue;
            }
            else
            {
                if (((r.x & r.y) & 0b1) == 0)
                    continue;
            }

            drawRectangle(&r);
        }
    }
}
#endif

void drawImage(const mr_rectangle_t *rectangle, const mr_color_t *imageBuffer)
{
    mr_draw_image(&mr, rectangle, imageBuffer);
}

void setFont(const uint8_t *font)
{
    mr_set_font(&mr, font);
}

uint16_t getTextWidth(const char *text)
{
    return mr_get_utf8_text_width(&mr, (const uint8_t *)text);
}

uint16_t getTextHeight(void)
{
    return mr_get_line_height(&mr);
}

typedef enum
{
    TEXTALIGNMENT_LEFTALIGNED,
    TEXTALIGNMENT_CENTERED,
    TEXTALIGNMENT_RIGHTALIGNED,
} TextAlignment;

static void drawAlignedText(const char *text, const mr_rectangle_t *rectangle, const mr_point_t *offset, TextAlignment textAlignment)
{
    char buffer[128];
    const char *str;
    int16_t width;

    switch (textAlignment)
    {
    case TEXTALIGNMENT_LEFTALIGNED:
        // | x-------->|
        width = rectangle->width - offset->x;

        break;

    case TEXTALIGNMENT_CENTERED:
        // |<----x---->|
        width = rectangle->width;

        break;

    case TEXTALIGNMENT_RIGHTALIGNED:
        // |<--------x |
        width = offset->x;

        break;

    default:
        width = 0;

        break;
    }

    if (getTextWidth(text) <= width)
        str = text;
    else
    {
        strcpy(buffer, text);
        for (uint32_t i = strlen(text); i > 0; i--)
        {
            // Skip UTF-8 continuation bytes
            if ((((uint8_t)buffer[i]) >> 6) == 0x02)
                continue;

            strcpy(buffer + i, getString(STRING_ELLIPSIS));
            if (getTextWidth(buffer) <= width)
                break;
        }
        str = buffer;
    }

    mr_point_t strOffset = *offset;
    switch (textAlignment)
    {
    case TEXTALIGNMENT_CENTERED:
        strOffset.x -= getTextWidth(str) / 2;

        break;

    case TEXTALIGNMENT_RIGHTALIGNED:
        strOffset.x -= getTextWidth(str);

        break;

    default:
        break;
    }

    mr_draw_utf8_text(&mr, (const uint8_t *)str, rectangle, &strOffset);
}

void drawText(const char *text, const mr_rectangle_t *rectangle, const mr_point_t *offset)
{
    drawAlignedText(text, rectangle, offset, TEXTALIGNMENT_LEFTALIGNED);
}

void drawCenteredText(const char *text, const mr_rectangle_t *rectangle, const mr_point_t *offset)
{
    drawAlignedText(text, rectangle, offset, TEXTALIGNMENT_CENTERED);
}

void drawRightAlignedText(const char *text, const mr_rectangle_t *rectangle, const mr_point_t *offset)
{
    drawAlignedText(text, rectangle, offset, TEXTALIGNMENT_RIGHTALIGNED);
}

static const char *getWrappedTextLine(const char *src, char *line)
{
    bool firstWord = true;
    char *dest = line;

    while (true)
    {
        uint8_t c = (uint8_t)*src;

        if (c == '\0')
            break;

        if (c == '\n')
        {
            src++;
            break;
        }

        if (c <= ' ')
        {
            src++;
            continue;
        }

        // Word
        const char *srcWord = src;
        char *destWord = dest;

        while (*(const uint8_t *)srcWord > ' ')
            srcWord++;

        size_t wordLength = srcWord - src;

        if (!firstWord)
            *destWord++ = ' ';

        memcpy(destWord, src, wordLength);
        destWord += wordLength;
        *destWord = '\0';

        if (!firstWord && getTextWidth(line) > CONTENT_MULTILINE_WIDTH)
            break;

        src = srcWord;
        dest = destWord;
        firstWord = false;
    }

    dest[0] = '\0';

    return src;
}

void drawCenteredMultilineText(const mr_rectangle_t *rectangle, const char *text)
{
    setFont(font_small);
    setStrokeColor(COLOR_ELEMENT_ACTIVE);

    char line[128];

    const char *src = text;
    uint32_t lineNum = 0;

    while (true)
    {
        src = getWrappedTextLine(src, line);

        lineNum++;

        if (src[0] == '\0')
            break;

        if (lineNum >= CONTENT_MULTILINE_MAX)
            break;
    }

    src = (const char *)text;
    uint32_t y = rectangle->y;

    for (uint32_t i = 0; i < lineNum; i++)
    {
        src = getWrappedTextLine(src, line);

        mr_rectangle_t lineRectangle = {rectangle->x, y, rectangle->width, FONT_SMALL_LINE_HEIGHT};
        mr_point_t lineOffset = {rectangle->width / 2, 0};

        // First line
        if (i == 0)
        {
            uint32_t top = (rectangle->height - lineNum * FONT_SMALL_LINE_HEIGHT) / 2;

            lineRectangle.height += top;
            lineOffset.y += top;
            y += top;
        }

        // Empty line
        if (line[0] == '\0')
            setStrokeColor(COLOR_ELEMENT_NEUTRAL);

        // Last line
        if (i == (lineNum - 1))
            lineRectangle.height = rectangle->y + rectangle->height - lineRectangle.y;

        drawCenteredText(line, &lineRectangle, &lineOffset);

        y += FONT_SMALL_LINE_HEIGHT;
    }
}

void drawRowLeft(const char *text, mr_rectangle_t *rectangle)
{
    rectangle->width = rectangle->x - CONTENT_LEFT;
    rectangle->x = CONTENT_LEFT;
    mr_point_t offset = {PADDING_LEFT, (rectangle->height - getTextHeight()) / 2};
    drawText(text, rectangle, &offset);
}

void drawRowRight(const char *text, mr_rectangle_t *rectangle)
{
    uint32_t width = getTextWidth(text) + PADDING_RIGHT;
    rectangle->width = width;
    rectangle->x -= width;
    mr_point_t offset = {0, (rectangle->height - getTextHeight()) / 2};
    drawText(text, rectangle, &offset);
}

static const uint8_t chargingIcons[] = {5, 6, 6, 6, 7};

void setupBatteryIcon(char *text)
{
    ColorIndex colorIndex;
    uint8_t level = getBatteryLevel();

    if (isBatteryCharging())
    {
        colorIndex = COLOR_RUNNING;
        level = chargingIcons[level];
    }
    else
    {
        if (isUSBPowered())
            colorIndex = COLOR_RUNNING;
        else if (level == 0)
            colorIndex = COLOR_ALARM;
        else
            colorIndex = COLOR_ELEMENT_NEUTRAL;
    }

    strclr(text);
    strcatChar(text, '0' + level);
    setFont(font_symbols);
    setStrokeColor(colorIndex);
}

void drawValueAndUnit(const char *valueString, const char *unitString, const mr_rectangle_t *rectangle, const mr_point_t *offset, bool isHold)
{
    mr_rectangle_t partialRectangle = *rectangle;
    partialRectangle.width = offset->x + getTextWidth(valueString);
    setStrokeColor(isHold ? COLOR_ELEMENT_ACTIVE : COLOR_INSTRUMENT_ENHANCED_SECONDARY);
    drawText(valueString, &partialRectangle, offset);

    partialRectangle.x += partialRectangle.width;
    partialRectangle.width = rectangle->width - partialRectangle.width;
    mr_point_t partialOffset = {0, offset->y};
    setStrokeColor(COLOR_ELEMENT_NEUTRAL);
    drawText(unitString, &partialRectangle, &partialOffset);
}
