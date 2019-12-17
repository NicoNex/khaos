#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t term_y;
static size_t term_x;
static uint8_t term_color;
static uint16_t* terminal_buffer;

void terminal_initialize(void) {
	term_y = 0;
	term_x = 0;
	term_color = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
	terminal_buffer = VGA_MEMORY;

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t i = y * VGA_WIDTH + x;
			terminal_buffer[i] = vga_entry(' ', term_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	term_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll_down(void) {
	const size_t max = VGA_WIDTH * (VGA_HEIGHT - 1);
	const uint16_t ws = vga_entry(' ', term_color);

	for (size_t i = 0; i < max; i++) {
		const size_t shift = i + VGA_WIDTH;
		terminal_buffer[i] = terminal_buffer[shift];
		terminal_buffer[shift] = ws;
	}
}

void terminal_putchar(char c) {
	unsigned char uc = c;

	switch (uc) {
	case '\n':
		term_x = 0;
		if (term_y < VGA_HEIGHT - 1)
			term_y++;
		else
			terminal_scroll_down();
		break;

	default:
		terminal_putentryat(uc, term_color, term_x, term_y);
		term_x++;
		break;
	}

	if (term_x == VGA_WIDTH) {
		if (term_y == VGA_HEIGHT - 1) {
			terminal_scroll_down();
		} else {
			term_y++;
		}
		term_x = 0;
	}
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
