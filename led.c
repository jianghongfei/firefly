#include "firefly.h"

int mem_fd;

void init(void)
{
	if (mem_fd != 0) {
		return;
	}

	/* Open /dev/mem */
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("Cannot open /dev/mem\n");
		exit(EXIT_FAILURE);
	}
}

const ff_gpio* setup_gpio(gpio_port port, gpio_pin pin)
{
	init();

	void *map = mmap(
		NULL,
		SZ_64K,	// Map length
		// Enable reading & writting to mapped memory
		PROT_READ | PROT_WRITE,
		MAP_SHARED,	// Shared with other processes
		mem_fd,	// File to map
		// Offset to GPIO peripheral
		0xFF780000 + port * SZ_64K
	);

	if (map == MAP_FAILED) {
		printf("mmap error %d\n", (unsigned)map);
		exit(EXIT_FAILURE);
	}

	ff_gpio *gpio = malloc(sizeof(ff_gpio));
	gpio->port = port;
	gpio->pin = pin;
	gpio->bit = (0x1 << pin);
	gpio->address = (volatile unsigned *)map;

	dump_gpio(setup, gpio);
	return gpio;
}

void gpio_input(const ff_gpio *gpio)
{
	*(gpio->address + GPIO_SWPORT_DDR) &= ~(gpio->bit);
	printf("input\n");
}

void gpio_output(const ff_gpio *gpio)
{
	*(gpio->address + GPIO_SWPORT_DDR) |= gpio->bit;
	printf("output\n");
}


void gpio_high(const ff_gpio *gpio)
{
	*(gpio->address + GPIO_SWPORT_DR) |= gpio->bit;
	printf("on\n");
}

void gpio_low(const ff_gpio *gpio)
{
	*(gpio->address + GPIO_SWPORT_DR) &= ~(gpio->bit);
	printf("off\n");
}

void blink(const ff_gpio *gpio)
{
	int i;

	printf("blink\n");

	gpio_input(gpio);
	gpio_output(gpio);

	printf("Start loop\n");
	for (i = 0; i < 10; ++i)
	{
		gpio_high(gpio);
		sleep(1);
		gpio_low(gpio);
		sleep(1);
	}
}

int main(int argc, char **args)
{
	const ff_gpio *gpio = setup_gpio(GPIO7, A3);
	dump_gpio(main, gpio);

	blink(gpio);

	close(mem_fd);

	if (gpio->address != NULL)
		free(gpio->address);
	free(gpio);

	return 0;
}
