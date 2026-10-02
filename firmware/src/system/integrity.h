/* GC-01 Pro RU — MIT */
#ifndef GC01_INTEGRITY_H
#define GC01_INTEGRITY_H
#include <stdint.h>
#include <stdbool.h>
static inline uint32_t stateCRC(const void *data, uint32_t size) {
    const uint8_t *p = data;
    uint32_t crc = 0xffffffffu;
    while (size--) {
        crc ^= *p++;
        for (unsigned i = 0; i < 8; ++i)
            crc = (crc >> 1) ^ ((0u - (crc & 1u)) & 0xedb88320u);
    }
    return ~crc;
}
static inline bool storageRangeValid(uint32_t address, uint32_t count,
                                     uint32_t begin, uint32_t end) {
    return address >= begin && address < end && count > 0 && count <= end - address;
}
#endif
