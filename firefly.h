#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>

// To set the priority on linux
#include <sched.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

#define RKIO_GPIO1_PHYS               0xFF7E0000
#define SZ_64K                        0x00010000

/* Offset of GPIO related registers */
#define GPIO_SWPORT_DR		0x00
#define GPIO_SWPORT_DDR		0x04
#define GPIO_INTEN		0x30
#define GPIO_INTMASK		0x34
#define GPIO_INTTYPE_LEVEL	0x38
#define GPIO_INT_POLARITY	0x3c
#define GPIO_INT_STATUS		0x40
#define GPIO_INT_RAWSTATUS	0x44
#define GPIO_DEBOUNCE		0x48
#define GPIO_PORTS_EOI		0x4c
#define GPIO_EXT_PORT		0x50
#define GPIO_LS_SYNC		0x60

#define dump_gpio(n, g)                                                        \
	do {                                                                   \
		printf(#n" ---------------------------------------------\n");  \
		printf("port:\t0x%08x\n", (unsigned int)(gpio->port));         \
		printf("pin:\t0x%08x\n", (unsigned int)(gpio->pin));           \
		printf("bit:\t0x%08x\n", (unsigned int)(gpio->bit));           \
		printf("address:\t0x%08x\n", (unsigned int)(gpio->address));   \
	while (0)

typedef enum {
	GPIO1, GPIO2, GPIO3, GPIO4, GPIO5, GPIO6, GPIO7, GPIO8
} gpio_port;

typedef enum {
        A0, A1, A2, A3, A4, A5, A6, A7,
        B0, B1, B2, B3, B4, B5, B6, B7,
        C0, C1, C2, C3, C4, C5, C6, C7,
        D0, D1, D2, D3, D4, D5, D6, D7
} gpio_pin;

typedef struct {
	gpio_port port;
	gpio_pin pin;
	unsigned bit;

	volatile unsigned *address;
} ff_gpio;

extern int mem_fd;

const ff_gpio* setup_gpio(gpio_port port, gpio_pin pin);
void gpio_input(const ff_gpio *gpio);
void gpio_output(const ff_gpio *gpio);
void gpio_high(const ff_gpio *gpio);
void gpio_low(const ff_gpio *gpio);