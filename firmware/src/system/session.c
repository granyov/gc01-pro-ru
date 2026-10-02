/* GC-01 Pro RU — MIT */
#include "session.h"
Session session;
void sessionSample(uint32_t seconds, uint32_t pulses, float cps) {
    if (!seconds || !(cps >= 0)) return;
    if (!session.seconds || cps < session.minimum) session.minimum = cps;
    if (!session.seconds || cps > session.maximum) session.maximum = cps;
    session.seconds += seconds;
    session.pulses += pulses;
}
void sessionAlert(uint32_t time, uint8_t rate, uint8_t dose, uint8_t fault) {
    if (rate == session.rate && dose == session.dose && fault == session.fault) return;
    session.rate = rate; session.dose = dose; session.fault = fault;
    session.events[session.head] = (SessionEvent){time, rate, dose, fault};
    session.head = (session.head + 1) % SESSION_EVENTS;
    if (session.count < SESSION_EVENTS) ++session.count;
}
