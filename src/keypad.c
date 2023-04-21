/**
 * \file            keypad.c
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
 
#include "keypad.h"
#include "lwbtn/lwbtn.h"

static void keypad_key_callback_handler(struct lwbtn* lwobj, struct lwbtn_btn* btn, lwbtn_evt_t evt);
static uint8_t keypad_get_key_state(struct lwbtn* lwobj, struct lwbtn_btn* btn);


static void keypad_key_callback_handler(struct lwbtn* lwobj, struct lwbtn_btn* btn, lwbtn_evt_t evt)
{
	(void)lwobj;
	
	keypad_t* keypad = (keypad_t*)(btn->arg);
	
	uint8_t btn_idx = 0;
	for(btn_idx = 0; btn_idx < keypad->row_cnt * keypad->column_cnt; btn_idx++)
	{
		if(btn == &keypad->keypad_btn->keypad_buttons[btn_idx])
			break;
	}
	
	if(evt == LWBTN_EVT_ONRELEASE) {
		keypad->key_event_callback(keypad, keypad->keymap[btn_idx]);		
	}
	
}

static uint8_t keypad_get_key_state(struct lwbtn* lwobj, struct lwbtn_btn* btn)
{
	(void)lwobj;
	
	keypad_t* keypad = (keypad_t*)(btn->arg);
	
	uint8_t btn_idx = 0;
	for(btn_idx = 0; btn_idx < keypad->row_cnt * keypad->column_cnt; btn_idx++)
	{
		if(btn == &keypad->keypad_btn->keypad_buttons[btn_idx])
			break;
	}
	
	return keypad->key_gpio_drv.io_read(&keypad->key_gpio_column[btn_idx % 4]);
}
 
void keypad_init(keypad_t* keypad, keypad_btn_t* keypad_btn, uint8_t row_cnt, uint8_t column_cnt, fptr_key_event_callback callback)
{
	keypad->row_cnt = row_cnt;
	keypad->column_cnt = column_cnt;
	keypad->keypad_btn = keypad_btn;
	keypad->key_event_callback = callback;
	
	// set button arg as column gpio port and pin.
	for(uint8_t i = 0; i < row_cnt * column_cnt; i++) {
		keypad->keypad_btn->keypad_buttons[i].arg = keypad;
	}
	
	lwbtn_init_ex(keypad_btn->h_keypad_btn, keypad_btn->keypad_buttons, row_cnt * column_cnt, keypad_get_key_state, keypad_key_callback_handler);
}

void keypad_scan(keypad_t* keypad, uint32_t ms_u32)
{
	for(uint8_t row = 0; row < keypad->row_cnt; row++)
	{
		key_gpio_t key_gpio = keypad->key_gpio_row[row];
		
		// set row pin as ouput
		keypad->key_gpio_drv.io_set_output(&key_gpio);
		
		// set row pin output as SET
		keypad->key_gpio_drv.io_write(&key_gpio, 1);
		
		for(uint8_t column = 0; column < keypad->column_cnt; column++)
		{
			// process button's input
			lwbtn_process_btn_ex(keypad->keypad_btn->h_keypad_btn, &keypad->keypad_btn->keypad_buttons[(row * keypad->column_cnt) + column], ms_u32);
		}
		
		// set row pin output as SET
		keypad->key_gpio_drv.io_write(&key_gpio, 0);
		
		// set pin to high impedance input, Effectively ends row pulse.
		keypad->key_gpio_drv.io_set_input(&key_gpio);
	}
	
}
 
 
 
 
 