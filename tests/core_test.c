#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "../firmware/src/system/integrity.h"
#include "../firmware/src/system/session.h"
#include "../firmware/src/system/cmath.h"
int main(void) {
    assert(stateCRC("123456789",9)==0xcbf43926u);
    assert(!storageRangeValid(0x08000000,2,0x0800f000,0x08010000));
    assert(!storageRangeValid(0x08004000,2,0x0800f000,0x08010000));
    assert(storageRangeValid(0x0800fc00,1024,0x0800f000,0x08010000));
    assert(!storageRangeValid(0x0800fffe,4,0x0800f000,0x08010000));
    assert(!storageRangeValid(0x0800f000,0xffffffff,0x0800f000,0x08010000));
    assert(!storageRangeValid(0x0800f000,0,0x0800f000,0x08010000));
    unsigned char record[64]={0}; uint32_t crc=stateCRC(record,sizeof record);
    for (unsigned i=0;i<512;++i) {
        record[i/8]^=1u<<(i%8); assert(stateCRC(record,sizeof record)!=crc);
        record[i/8]^=1u<<(i%8);
    }
    sessionSample(1,0,0); sessionSample(1,2,2); sessionSample(1,7,7);
    assert(session.minimum==0 && session.maximum==7 && session.pulses==9 && session.seconds==3);
    sessionSample(0,99,99); sessionSample(1,99,NAN); assert(session.seconds==3);
    for (unsigned i=0;i<40;++i) sessionAlert(i,i%3,0,0);
    assert(session.count==16 && session.events[(session.head+15)%16].time==39);
    uint8_t head=session.head; sessionAlert(40,0,0,0); assert(head==session.head);
    assert(getConfidenceInterval(0)>2 && getConfidenceInterval(1)>2);
    float prev=getConfidenceInterval(1);
    for (unsigned n=2;n<=10000;++n) { float v=getConfidenceInterval(n);assert(v>0 && v<prev);prev=v; }
    assert(fabsf(getConfidenceInterval(10000)-0.0196917f)<0.00001f);
    assert(addClamped(0xfffffffeu,4)==0xffffffffu);
    puts("core: CRC, write boundaries, bit flips, session, ring, uncertainty — OK");
}
