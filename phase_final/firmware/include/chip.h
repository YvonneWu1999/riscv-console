#include <stdint.h>
#ifndef CHIP_H
#define CHIP_H

// Adapted from
// https://stackoverflow.com/questions/58947716/how-to-interact-with-risc-v-csrs-by-using-gcc-c-code
__attribute__((always_inline)) inline uint32_t csr_mstatus_read(void) {
    uint32_t result;
    asm volatile("csrr %0, mstatus" : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline uint32_t csr_mcause_read(void) {
    uint32_t result;
    asm volatile("csrr %0, mcause" : "=r"(result));
    return result;
}

__attribute__((always_inline)) inline void csr_mstatus_write(uint32_t val) {
    asm volatile("csrw mstatus, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_write_mie(uint32_t val) {
    asm volatile("csrw mie, %0" : : "r"(val));
}

__attribute__((always_inline)) inline void csr_enable_interrupts(void) {
    asm volatile("csrsi mstatus, 0x8");
}

__attribute__((always_inline)) inline void csr_disable_interrupts(void) {
    asm volatile("csrci mstatus, 0x8");
}

#define MTIME_LOW (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH (*((volatile uint32_t *)0x40000014))
#define CONTROLLER (*((volatile uint32_t *)0x40000018))

// interrupt registers
#define INTERRUPT_ENABLE_REGISTER (*((volatile uint32_t *)0x40000000))
#define INTERRUPT_PENDING_REGISTER (*((volatile uint32_t *)0x40000004))

// video momory map
#define LARGE_DATA_ADDRESS ((volatile uint8_t *)0x50090000)
#define LARGE_PALETTE_ADDRESS ((volatile uint32_t *)0x500F1000)
#define LARGE_CONTROL_ADDRESS ((volatile uint32_t *)0x500F5B00)

#define MEDIUM_DATA_ADDRESS ((volatile uint8_t *)0x500D0000)
#define MEDIUM_PALETTE_ADDRESS ((volatile uint32_t *)0x500F2000)
#define MEDIUM_CONTROL_ADDRESS ((volatile uint32_t *)0x500F5F00)

#define SMALL_DATA_ADDRESS ((volatile uint8_t *)0x500E0000)
#define SMALL_PALETTE_ADDRESS ((volatile uint32_t *)0x500F3000)
#define SMALL_CONTROL_ADDRESS ((volatile uint32_t *)0x500F6300)

#define BACKGROUND_DATA_ADDRESS ((volatile uint8_t *)0x50000000)
#define BACKGROUND_PALETTE_ADDRESS ((volatile uint32_t *)0x500F0000)
#define BACKGROUND_CONTROL_ADDRESS ((volatile uint32_t *)0x500F5A00)

// mode control registor
#define MODE_CONTROL_REGISTOR (*((volatile uint32_t *)0x500F6780))
#endif