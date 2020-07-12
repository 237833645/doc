/*
 * aw9523b.h   aw9523b martix key
 *
 * Copyright (c) 2017 AWINIC Technology CO., LTD
 *
 *  Author: <luofuhong@awinic.com.cn>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */


#ifndef _AW9523_H_
#define _AW9523_H_

#define AW9523_ID 0x23
#define AW9523_KEY_PORT_MAX (0x10) // 16
#define AW9523_LED_MAX_NUMS (0x10) // 16

#define AWINIC_DEBUG		1
#if (defined (AWINIC_DEBUG)  &&  (AWINIC_DEBUG != 0))
#define AW_DEBUG(fmt, args...) \
	do {	\
		pr_info(fmt, ##args); \
	} while(0)
#else
#define AW_DEBUG(fmt, ...) \
	do {	\
	} while(0)
#endif
struct aw9523_key;
struct aw9523_led;
struct aw9523_gpio;

struct aw9523 {
    struct i2c_client *client;
    struct device *dev;
	int irq_gpio;
	int irq_num;
    int rst_gpio;
    unsigned char chipid;
	bool key_feature_enable;
	bool led_feature_enable;
	bool gpio_feature_enable;
	struct aw9523_key *key_data;
	struct aw9523_led *led_data;
	struct aw9523_gpio *gpio_data;
};

typedef struct{
	char name[10];
	int key_code;
	int key_val;
}KEY_STATE;


struct aw9523_key{
	unsigned int key_mask;
	unsigned int input_port_nums;
	unsigned int output_port_nums;
	unsigned int input_port_mask;
	unsigned int output_port_mask;
	unsigned int new_input_state;
	unsigned int old_input_state;
	unsigned int *new_output_state;
	unsigned int *old_output_state;
	unsigned int *def_output_state;
	struct input_dev *input;

	unsigned int debounce_delay;
	struct delayed_work int_work;
	struct hrtimer key_timer;
	struct work_struct key_work;
	KEY_STATE *keymap;
	int keymap_len;
	struct aw9523 *priv;	
};

struct aw9523_single_led{
	struct led_classdev cdev;
	struct work_struct brightness_work;
	unsigned int idx;
	unsigned cur_brightness;
	unsigned int max_brightness;
	struct aw9523 *priv;
};


struct aw9523_led{
	unsigned int led_mask;
	int imax;
	int led_nums;
	struct aw9523_single_led * single_led_data;
};

enum aw9523_gpio_dir{
	AW9523_GPIO_INPUT = 0,
	AW9523_GPIO_OUTPUT = 1,
};

enum aw9523_gpio_val{
	AW9523_GPIO_HIGH = 1,
	AW9523_GPIO_LOW = 0,
};

enum aw9523_gpio_output_mode{
	AW9523_OPEN_DRAIN_OUTPUT = 0,
	AW9523_PUSH_PULL_OUTPUT = 1,
};

struct aw9523_singel_gpio{
	unsigned int gpio_idx;
	enum aw9523_gpio_dir gpio_direction;
	enum aw9523_gpio_val state;
	struct aw9523 *priv;
};


struct aw9523_gpio{
	unsigned int gpio_mask;
	unsigned int gpio_num;
	enum aw9523_gpio_output_mode output_mode;
	struct aw9523_singel_gpio *single_gpio_data;
};
#endif
