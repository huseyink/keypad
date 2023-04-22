/**
 * \file            keypad.h
 * \brief           Generic keypad module
 */

/*
 * Copyright (c) 2023 Huseyin KOC
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * Author:          Huseyin KOC <kou.huseyinkoc@gmail.com>
 * Version:         v0.0.1
 */
 
#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include "keypad_opt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
/* Forward declarations */
struct lwbtn_btn;
struct lwbtn;
struct key_gpio;
struct keypad;

typedef void    (*fptr_gpio_set_output)(struct key_gpio* io_handle);
typedef void    (*fptr_gpio_set_input)(struct key_gpio*  io_handle);
typedef void    (*fptr_gpio_write)(struct key_gpio* io_handle, uint8_t state);
typedef uint8_t (*fptr_gpio_read)(struct key_gpio* io_handle);
typedef void    (*fptr_key_event_callback)(struct keypad*, uint8_t key_char);

typedef struct key_gpio{
	void*    gpio_port;
	uint32_t gpio_pin;
}key_gpio_t;

typedef struct {
	struct lwbtn*     h_keypad_btn;
	struct lwbtn_btn* keypad_buttons;
}keypad_btn_t;

typedef struct {
	fptr_gpio_set_input  io_set_input;
	fptr_gpio_set_output io_set_output;
	fptr_gpio_write      io_write;
	fptr_gpio_read       io_read;
}keypad_gpio_drv_t;

typedef struct keypad{
	uint8_t* keymap;
	key_gpio_t* key_gpio_row;
	key_gpio_t* key_gpio_column;
	keypad_btn_t* keypad_btn;
	fptr_key_event_callback key_event_callback;
	keypad_gpio_drv_t key_gpio_drv;
	uint8_t row_cnt;
	uint8_t column_cnt;
	char pressed_key;
}keypad_t;

uint8_t keypad_init(keypad_t* keypad, keypad_btn_t* keypad_btn, uint8_t row_cnt, uint8_t column_cnt, fptr_key_event_callback callback);
void    keypad_scan(keypad_t* keypad, uint32_t ms_time);
char    keypad_get_pressed_key(keypad_t* keypad);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* KEYPAD_H */




 