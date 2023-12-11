#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip.h"
#include "syscall.h"

extern uint8_t _erodata[];
extern uint8_t _data[];
extern uint8_t _edata[];
extern uint8_t _sdata[];
extern uint8_t _esdata[];
extern uint8_t _bss[];
extern uint8_t _ebss[];

#define FMEM (*((volatile uint32_t *)0x00020000))
#define DMA_SRC (*((volatile uint32_t *)0x40000020))
#define DMA_DST (*((volatile uint32_t *)0x40000024))
#define DMA_CMD (*((volatile uint32_t *)0x40000028))
#define DMA_STS (*((volatile uint32_t *)0x4000002C))

void timer_interrupt();
void cmd_interrupt();
void video_interrupt();

void init(void) {
    uint8_t *Source = _erodata;
    uint8_t *Base = _data < _sdata ? _data : _sdata;
    uint8_t *End = _edata > _esdata ? _edata : _esdata;

    while (Base < End) {
        *Base++ = *Source++;
    }
    Base = _bss;
    End = _ebss;
    while (Base < End) {
        *Base++ = 0;
    }

    csr_write_mie(0x888);    // Enable all interrupt soruces
    csr_enable_interrupts(); // Global interrupt enable
    // Enable both CMIE and VIE (set the respective
    // bits to 1)
    INTERRUPT_ENABLE_REGISTER = INTERRUPT_ENABLE_REGISTER | (0x2);
    INTERRUPT_ENABLE_REGISTER = INTERRUPT_ENABLE_REGISTER | (0x4);
    MTIMECMP_LOW = 1;
    MTIMECMP_HIGH = 0;
    FMEM = 1;
}

extern volatile int global;
extern volatile uint32_t controller_status;
int video_interrupt_count = 0;
int cmd_interrupt_count = 0;
volatile char *VIDEO_MEMORY_FOR_INTERRUPT =
    (volatile char *)(0x50000000 + 0xF4800);
void c_interrupt_handler(void) {
    uint32_t mcause = csr_mcause_read();
    if (mcause == 0x80000007) {
        timer_interrupt();
    }
    if ((INTERRUPT_PENDING_REGISTER & 0x2)) {
        video_interrupt();
    }
    if ((INTERRUPT_PENDING_REGISTER & 0x4)) {
        cmd_interrupt();
    }
}

uint32_t c_system_call(uint32_t arg0, uint32_t arg1, uint32_t arg2,
                       uint32_t arg3, uint32_t arg4, uint32_t call) {
    if (1 == call) {
        return global;
    } else if (2 == call) {
        return CONTROLLER;
    } else if (3 == call) {
        return MTIMECMP_LOW;
    } else if (4 == call) {
        return FMEM;
    } else if (5 == call) {
        return enableTextMode();
    } else if (6 == call) {
        return enableGraphicsMode();
    } else if (7 == call) {
        return createSmallSprite(arg0);
    } else if (8 == call) {
        return setSmallPalette(arg0, arg1);
    } else if (9 == call) {
        return setSmallControl(arg0, arg1, arg2, arg3, arg4);
    } else if (10 == call) {
        return createMediumSprite(arg0);
    } else if (11 == call) {
        return setMediumPalette(arg0, arg1);
    } else if (12 == call) {
        return setMediumControl(arg0, arg1, arg2, arg3, arg4);
    } else if (13 == call) {
        return createLargeSprite(arg0);
    } else if (14 == call) {
        return setLargePalette(arg0, arg1);
    } else if (15 == call) {
        return setLargeControl(arg0, arg1, arg2, arg3, arg4);
    } else if (16 == call) {
        return createBackground(arg0);
    } else if (17 == call) {
        return setBackgroundPalette(arg0, arg1);
    } else if (18 == call) {
        return setBackgroundControl(arg0, arg1, arg2, arg3, arg4);
    } else if (19 == call) {
        return ((((uint64_t)MTIME_HIGH) << 32) | MTIME_LOW);
    } else if (20 == call) {
        return ((((uint64_t)MTIME_HIGH) << 32) | MTIME_LOW) - arg0;
    } else if (22 == call) {
        return cmd_interrupt_count;
    } else if (23 == call) {
        return video_interrupt_count;
    } else if (24 == call) {
        return getSmallSpriteCount();
    } else if (25 == call) {
        return getMediumSpriteCount();
    } else if (26 == call) {
        return getLargeSpriteCount();
    }
    return -1;
}

void timer_interrupt() {
    uint64_t NewCompare = (((uint64_t)MTIMECMP_HIGH) << 32) | MTIMECMP_LOW;
    NewCompare += 100;
    MTIMECMP_HIGH = NewCompare >> 32;
    MTIMECMP_LOW = NewCompare;
    global++;
}
void video_interrupt() {
    video_interrupt_count++;
    INTERRUPT_PENDING_REGISTER = INTERRUPT_PENDING_REGISTER | (0x2);
}
void cmd_interrupt() {
    cmd_interrupt_count++;
    INTERRUPT_PENDING_REGISTER = INTERRUPT_PENDING_REGISTER | (0x4);
}
