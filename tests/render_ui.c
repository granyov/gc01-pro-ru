/* Exact firmware renderer with synthetic fixtures, not a device emulator. MIT. */
#include <stdio.h>
#include <stdlib.h>
#include "../firmware/src/ui/draw.h"
#include "../firmware/src/ui/system.h"
#include "../firmware/src/ui/measurements.h"
#include "../firmware/src/system/settings.h"
#include "../firmware/src/system/session.h"
#include "../firmware/src/measurements/measurements.h"
#include "../firmware/src/peripherals/rtc.h"
extern mr_t mr;
Settings settings={.displayTheme=DISPLAY_THEME_DUSK,.pulseSound=true};
static OnViewEvent *view;
uint8_t getBatteryLevel(void){return 4;}
bool isBatteryCharging(void){return false;}
bool isUSBPowered(void){return false;}
bool isLockModeEnabled(void){return false;}
bool isSoundIconActive(void){return false;}
bool isAlertEnabled(void){return false;}
bool isAlertFlashing(void){return false;}
bool isAlertPending(void){return false;}
AlertLevel getAlertLevel(void){return ALERTLEVEL_NONE;}
void getDeviceDateTime(RTCDateTime *dt){*dt=(RTCDateTime){2026,10,1,14,32,0};}
void getDateTimeFromTime(uint32_t t,RTCDateTime *dt){*dt=(RTCDateTime){2026,10,1,(t/3600)%24,(t/60)%60,t%60};}
void showSettingsMenu(void){}
void showView(OnViewEvent *v){view=v;}
static mr_color_t pixels[320*240];
static void save(const char *name){
    char path[200];snprintf(path,sizeof path,"build/%s.ppm",name);FILE *f=fopen(path,"wb");
    if(!f)exit(1);fprintf(f,"P6\n320 240\n255\n");
    for(unsigned i=0;i<320*240;++i){unsigned v=pixels[i];fputc(((v>>11)&31)*255/31,f);fputc(((v>>5)&63)*255/63,f);fputc((v&31)*255/31,f);}fclose(f);
}
static void clear(void){setFillColor(COLOR_CONTAINER_BACKGROUND);drawRectangle(&(mr_rectangle_t){0,0,320,240});}
int main(void){
    mr_init(&mr);mr.display_width=320;mr.display_height=240;mr.buffer=pixels;
    mr.draw_rectangle_callback=mr_draw_rectangle_framebuffer_color;
    mr.draw_string_callback=mr_draw_string_framebuffer_color;
    clear();drawTitleBar("Измерение");
    drawConsoleDashboard("0.12","мкЗв/ч",0.18f,"18.4","0.31","2.41 мкЗв","0.56",MEASUREMENTSTYLE_NORMAL);
    save("measurement");
    clear();drawTitleBar("Измерение");
    drawConsoleDashboard("12345.6","мкЗв/ч",0.02f,"740736","12345.6","1.23 мЗв","12345.6",MEASUREMENTSTYLE_ALARM);
    save("high-range");
    clear();drawTitleBar("Доза");drawMeasurementValue("2.41","мкЗв",0.05f,MEASUREMENTSTYLE_NORMAL);
    drawMeasurementInfo("Время","12:48:32","",MEASUREMENTSTYLE_NORMAL);save("dose");
    clear();drawTitleBar("Тревога");drawMeasurementValue("12.5","мкЗв/ч",0.06f,MEASUREMENTSTYLE_ALARM);
    drawMeasurementAlert("Тревога");save("alarm");
    sessionSample(1,0,0);sessionSample(59,18,2);clear();showSessionView();view(EVENT_DRAW);save("session");
    sessionAlert(52320,1,0,0);sessionAlert(52330,2,0,0);sessionAlert(52340,0,0,0);
    clear();showEventLog();view(EVENT_DRAW);save("events");
}
