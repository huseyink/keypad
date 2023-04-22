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

/**
 * @brief Key press callback handler for a keypad button
 *
 * This function is called when a keypad button is pressed and released.
 *
 * @param lwobj  Pointer to the lightweight button object.
 * @param btn    Pointer to the button object in the keypad.
 * @param evt    The event that occurred
 *
 * @return void
 */
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
		keypad->pressed_key = (char)keypad->keymap[btn_idx];
		keypad->key_event_callback(keypad, keypad->keymap[btn_idx]);		
	}
}

/**
 * @brief Reads the state of a button in the keypad.
 *        This is a wrapper callback function to read GPIO status.
 * 
 * @param lwobj Pointer to the lightweight button object.
 * @param btn   Pointer to the button object in the keypad.
 * 
 * @return uint8_t 1 if the button is pressed, 0 otherwise.
 */
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
 
/**
 * @brief Initializes a keypad with the specified properties and assigns a key event callback.
 *
 * @param keypad     Pointer to a keypad_t struct.
 * @param keypad_btn Pointer to a keypad_btn_t struct.
 * @param row_cnt    Number of rows on the keypad.
 * @param column_cnt Number of columns on the keypad.
 * @param callback   Function pointer to the key event callback.
 *
 * @return Returns 1 if initialization was successful, 0 otherwise.
 */
uint8_t keypad_init(keypad_t* keypad, keypad_btn_t* keypad_btn, uint8_t row_cnt, uint8_t column_cnt, fptr_key_event_callback callback)
{
	if(keypad != NULL)
	{
	  keypad->row_cnt = row_cnt;
	  keypad->column_cnt = column_cnt;
	  keypad->keypad_btn = keypad_btn;
	  keypad->key_event_callback = callback;
	  
	  // assign keypad instance as arg to call event callback.
	  for(uint8_t i = 0; i < row_cnt * column_cnt; i++) {
	  	keypad->keypad_btn->keypad_buttons[i].arg = keypad;
	  }
	  
	  return lwbtn_init_ex(keypad_btn->h_keypad_btn, keypad_btn->keypad_buttons, row_cnt * column_cnt, keypad_get_key_state, keypad_key_callback_handler);	
	}

	return 0;
}

/**
 * @brief Processes the input for a keypad object.
 *
 * @param keypad  Pointer to a keypad_t struct.
 * @param ms_time The amount of time in milliseconds since the last scan.
 * 
 * @return void
 */
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
		
		// set row pin output as RESET
		keypad->key_gpio_drv.io_write(&key_gpio, 0);
		
		// set pin to high impedance input, Effectively ends row pulse.
		keypad->key_gpio_drv.io_set_input(&key_gpio);
	}
}

/**
 * @brief Returns the currently pressed key on the keypad.
 * 
 * @param keypad Pointer to a keypad_t struct.
 * 
 * @return Currently pressed key on the keypad
 */
char keypad_get_pressed_key(keypad_t* keypad)
{
	return keypad->pressed_key;
}
