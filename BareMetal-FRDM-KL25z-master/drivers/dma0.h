#ifndef DMA0_H
#define DMA0_H

#include <stdint.h>

void configure_dma0(void *from, void *to, uint8_t length);

#endif