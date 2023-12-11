#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syscall.h"
volatile int global = 42;

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xF4800);
volatile uint32_t *CartridgeStatus = (volatile uint32_t *)(0x4000001C);
typedef void (*FunctionPtr)(void);

#define FMEM (*((volatile uint32_t *)0x00020000))
#define DMA_SRC (*((volatile uint32_t *)0x40000020))
#define DMA_DST (*((volatile uint32_t *)0x40000024))
#define DMA_CMD (*((volatile uint32_t *)0x40000028))
#define DMA_STS (*((volatile uint32_t *)0x4000002C))

#define MTIME_LOW (*((volatile uint32_t *)0x40000008))
#define MTIME_HIGH (*((volatile uint32_t *)0x4000000C))
#define MTIMECMP_LOW (*((volatile uint32_t *)0x40000010))
#define MTIMECMP_HIGH (*((volatile uint32_t *)0x40000014))
#define CONTROLLER (*((volatile uint32_t *)0x40000018))

int main() {
    int a = 4;
    int b = 12;
    int last_global = 42;
    int x_pos = 12;

    char *Buffer = malloc(32);
    strcpy(Buffer, "OS STARTED");
    strcpy((char *)VIDEO_MEMORY, Buffer);

    while (1) {
        if (*CartridgeStatus & 0x1) {
            FunctionPtr Fun = (FunctionPtr)((*CartridgeStatus) & 0xFFFFFFFC);
            Fun();
        }
    }

    return 0;
}

extern char _heap_base[];
extern char _stack[];

char *_sbrk(int numbytes) {
    static char *heap_ptr = NULL;
    char *base;

    if (heap_ptr == NULL) {
        heap_ptr = (char *)&_heap_base;
    }

    if ((heap_ptr + numbytes) <= _stack) {
        base = heap_ptr;
        heap_ptr += numbytes;
        return (base);
    } else {
        // errno = ENOMEM;
        return NULL;
    }
}
