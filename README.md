# Generic Keypad Library

This project provides a generic keypad library for your MCU project.
Thanks to [Tilen Majerle](https://github.com/MaJerle) for [button manager library](https://github.com/MaJerle/lwbtn). 
I used this library to scan key inputs.

There is an example below on STM32 based MCU for 4x4 keypad module.
You can port this example any MCU platform whatever you want.

You can contribute to improving the library.

For contact: [twitter.com/esdevhk](twitter.com/esdevhk)
~~~C

/* input module */
#include "lwbtn/lwbtn.h"

/* GPIO module */
#include "gpio.h"

/* keypad module */
#include "keypad.h"

static void    gpio_set_output(key_gpio_t* io_handle);
static void    gpio_set_input(key_gpio_t* io_handle);
static uint8_t gpio_read(key_gpio_t* io_handle);
static void    gpio_write(key_gpio_t* io_handle, uint8_t state);

static void    keypad_key_event_callback(struct keypad* keypad_src, uint8_t key_char);

/* button instance */
lwbtn_t h_keypad_btn;
lwbtn_btn_t keypad_buttons[KEYPAD_COLUMN_COUNT_DEFAULT * KEYPAD_ROW_COUNT_DEFAULT];

/* keypad column pins */
key_gpio_t key_gpio_col[KEYPAD_COLUMN_COUNT_DEFAULT] = {
	{
		.gpio_port = GPIOE,
		.gpio_pin = GPIO_PIN_3,
	},
	{
		.gpio_port = GPIOF,
		.gpio_pin = GPIO_PIN_8,
	},
	{
		.gpio_port = GPIOF,
		.gpio_pin = GPIO_PIN_7,
	},
	{
		.gpio_port = GPIOF,
		.gpio_pin = GPIO_PIN_9,
	}	
};

/* keypad column row pins */
key_gpio_t key_gpio_row[KEYPAD_ROW_COUNT_DEFAULT] = {
	{
		.gpio_port = GPIOE,
		.gpio_pin = GPIO_PIN_2,
	},
	{
		.gpio_port = GPIOE,
		.gpio_pin = GPIO_PIN_4,
	},
	{
		.gpio_port = GPIOE,
		.gpio_pin = GPIO_PIN_5,
	},
	{
		.gpio_port = GPIOE,
		.gpio_pin = GPIO_PIN_6,
	}	
};

/* keypad mcu specific gpio driver functions */
keypad_gpio_drv_t keypad_gpio_drv = {
	.io_read = gpio_read,
	.io_write = gpio_write,
	.io_set_input = gpio_set_input,
	.io_set_output = gpio_set_output,
};

/* keypad button instance */
keypad_btn_t keypad_btn = {
	.h_keypad_btn = &h_keypad_btn,
	.keypad_buttons = keypad_buttons
};

/* keypad key mapping */
uint8_t keymap[KEYPAD_ROW_COUNT_DEFAULT][KEYPAD_COLUMN_COUNT_DEFAULT] =
{
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'},
};

keypad_t keypad;

void app_main(void)
{
	keypad.key_gpio_column = key_gpio_col;
	keypad.key_gpio_row = key_gpio_row;
	keypad.key_gpio_drv = keypad_gpio_drv;
	keypad.keymap = (uint8_t*)keymap;
	
	keypad_init(&keypad, &keypad_btn, KEYPAD_ROW_COUNT_DEFAULT, 
                                    KEYPAD_COLUMN_COUNT_DEFAULT, 
                                    keypad_key_event_callback);

	while(1)
	{
		keypad_scan(&keypad, HAL_GetTick());
	}
}

/* ------------------- MCU specific GPIO driver interface ------------------- */

static void gpio_write(key_gpio_t* io_handle, uint8_t state)
{
	HAL_GPIO_WritePin((GPIO_TypeDef*)io_handle->gpio_port, (uint16_t)io_handle->gpio_pin, (GPIO_PinState)state);
}

static void gpio_set_output(key_gpio_t* io_handle)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = io_handle->gpio_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init((GPIO_TypeDef*)io_handle->gpio_port, &GPIO_InitStruct);
}

static void gpio_set_input(key_gpio_t* io_handle)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = io_handle->gpio_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init((GPIO_TypeDef*)io_handle->gpio_port, &GPIO_InitStruct);
}

static uint8_t gpio_read(key_gpio_t* io_handle)
{
	return (uint8_t)HAL_GPIO_ReadPin((GPIO_TypeDef*)io_handle->gpio_port, io_handle->gpio_pin);
}

/* ------------------- Keypad key pressed callback handler ------------------- */
static void keypad_key_event_callback(struct keypad* keypad_src, uint8_t key_char)
{
	if(keypad_src == &keypad)
	{
		
	}
}

~~~
