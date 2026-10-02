/* GC-01 Pro RU — MIT */
#include "../system/session.h"
#include "../system/settings.h"
#include "../system/cstring.h"
#include "../peripherals/rtc.h"
#include "draw.h"
#include "system.h"
#include "view.h"
static uint8_t logPage;
static void row(unsigned y, const char *key, const char *value) {
    mr_rectangle_t box = {0, (int)y, DISPLAY_WIDTH/2, 30};
    setFont(font_small); setStrokeColor(COLOR_ELEMENT_NEUTRAL);
    drawText(key, &box, &(mr_point_t){12,0});
    box.x = DISPLAY_WIDTH/2;
    setStrokeColor(COLOR_INSTRUMENT_ENHANCED_SECONDARY);
    drawRightAlignedText(value, &box, &(mr_point_t){DISPLAY_WIDTH/2-12,0});
}
static void onSession(ViewEvent event) {
    if (event == EVENT_KEY_BACK) showSettingsMenu();
    if (event != EVENT_DRAW) return;
    drawTitleBar("Сеанс");
    const char *labels[] = {"мин / 1 с", "макс / 1 с", "среднее", "импульсы", "секунды"};
    for (unsigned i=0; i<5; ++i) {
        char value[32] = "";
        if (i<3 && !session.seconds) strcpy(value,"--");
        else if (i<3) strcatFloat(value, i==0 ? session.minimum : i==1 ? session.maximum :
                     (float)session.pulses/session.seconds, 2);
        else if (i==3) {
            if (session.pulses > UINT32_MAX) strcpy(value,">4G");
            else strcatUInt32(value, session.pulses, 1);
        } else strcatUInt32(value,session.seconds,1);
        row(CONTENT_TOP+10+i*30, labels[i], value);
    }
}
void showSessionView(void) { showView(onSession); }
static void onLog(ViewEvent event) {
    if (event == EVENT_KEY_BACK) showSettingsMenu();
    if (event == EVENT_KEY_UP && logPage) --logPage;
    if (event == EVENT_KEY_DOWN && (logPage+1)*5 < session.count) ++logPage;
    if (event != EVENT_DRAW) return;
    drawTitleBar("Журнал");
    for (unsigned i=0; i<5; ++i) {
        unsigned n = logPage*5+i;
        char key[24]="", value[32]="";
        if (n < session.count) {
            SessionEvent *e=&session.events[(session.head+SESSION_EVENTS-1-n)%SESSION_EVENTS];
            RTCDateTime dt; getDateTimeFromTime(e->time,&dt);
            strcatUInt32(key,dt.hour,2); strcat(key,":"); strcatUInt32(key,dt.minute,2);
            strcat(key,":"); strcatUInt32(key,dt.second,2);
            strcat(value,"R"); strcatUInt32(value,e->rate,1);
            strcat(value," D"); strcatUInt32(value,e->dose,1);
            strcat(value," F"); strcatUInt32(value,e->fault,1);
        } else if (!session.count && !i) strcpy(key,"Нет событий");
        row(CONTENT_TOP+10+i*30,key,value);
    }
}
void showEventLog(void) { logPage=0; showView(onLog); }
