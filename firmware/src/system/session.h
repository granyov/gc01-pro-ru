/* GC-01 Pro RU — MIT. Volatile session statistics; units are raw CPS. */
#ifndef GC01_SESSION_H
#define GC01_SESSION_H
#include <stdint.h>
#define SESSION_EVENTS 16

typedef struct { uint32_t time; uint8_t rate, dose, fault; } SessionEvent;
typedef struct {
    float minimum, maximum;
    uint32_t seconds;
    uint64_t pulses;
    SessionEvent events[SESSION_EVENTS];
    uint8_t head, count, rate, dose, fault;
} Session;
extern Session session;
void sessionSample(uint32_t seconds, uint32_t pulses, float cps);
void sessionAlert(uint32_t time, uint8_t rate, uint8_t dose, uint8_t fault);
void showSessionView(void);
void showEventLog(void);
#endif
