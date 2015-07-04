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

int mem_fd;
void *gpio_map;

volatile unsigned *gpio;

void input(int pin)
{
	*(gpio + 1) &= ~(0x1 << pin);
}

void output(int pin)
{
	*(gpio + 1) |= (0x1 << pin);
}

void high(int pin)
{
	*gpio |= (0x1 << pin);
}

void low(int pin)
{
	*gpio &= ~(0x1 << pin);
}

void blink(int pin)
{
	int i;

	input(pin);
	output(pin);
	for (i = 0; i < 10; i++) {
		high(pin);
		sleep(1);
		low(pin);
		sleep(1);
	}
}

int main(int argc, char **argv)
{
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("Cannot open /dev/mem\n");
		exit(-1);
	}

	gpio_map = mmap(NULL,
			4096,
			PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0XFF7E0000);
	close(mem_fd);

	if (gpio_map == MAP_FAILED) {
		printf("mmap error %d\n", (int)gpio_map);
		exit(-1);
	}

	gpio = (volatile unsigned *)gpio_map;

	blink(3);

	return 0;
}
