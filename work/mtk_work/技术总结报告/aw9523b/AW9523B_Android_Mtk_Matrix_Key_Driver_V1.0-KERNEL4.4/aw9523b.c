/*
 * aw9523.c   aw9523 martix key
 *
 * Version: v1.0.0
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <linux/errno.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/of_gpio.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/leds.h>
#include <linux/pinctrl/consumer.h>
#include <linux/regulator/consumer.h>
#include <linux/dma-mapping.h>
#include <linux/hrtimer.h>  
#include <linux/leds.h>
#include <stddef.h>
#include <linux/aw9523b.h>

#define HRTIMER_FRAME	20
#define AW9523B_DRIVER_VERSION		"driver version: 1.0.0"

/*register list */
#define P0_INPUT		0x00
#define P1_INPUT 		0x01
#define P0_OUTPUT 		0x02
#define P1_OUTPUT 		0x03
#define P0_CONFIG		0x04
#define P1_CONFIG 		0x05
#define P0_INT			0x06
#define P1_INT			0x07
#define ID_REG			0x10
#define CTL_REG			0x11
#define P0_LED_MODE		0x12
#define P1_LED_MODE		0x13
#define P1_0_DIM0		0x20
#define P1_1_DIM0		0x21
#define P1_2_DIM0		0x22
#define P1_3_DIM0		0x23
#define P0_0_DIM0		0x24
#define P0_1_DIM0		0x25
#define P0_2_DIM0		0x26
#define P0_3_DIM0		0x27
#define P0_4_DIM0		0x28
#define P0_5_DIM0		0x29
#define P0_6_DIM0		0x2A
#define P0_7_DIM0		0x2B
#define P1_4_DIM0		0x2C
#define P1_5_DIM0		0x2D
#define P1_6_DIM0		0x2E
#define P1_7_DIM0		0x2F
#define SW_RSTN			0x7F


static KEY_STATE key_map[]={
//	name		code				val
	{"DEL",	    KEY_BACK,			0},
	{"BACK"	,	KEY_BACK,			0},
	{"HOME"	,	KEY_HOME,			0},
	{"HOME"	,	KEY_HOME,			0}, 
	{"BACK"	,	KEY_BACK,			0}, 
	{"Q"	,	KEY_Q,			    0}, 
	{"F1"	,	KEY_F1,			    0}, 
	{"F2"	,	KEY_F2,			    0}, 

	{"W"	,	KEY_W,				0}, 
	{"E"	,	KEY_E,				0}, 
	{"R"	,	KEY_R,				0}, 
	{"T"	,	KEY_T,				0}, 
	{"Y"    ,	KEY_Y,			    0}, 
	{"U"	,	KEY_U,				0}, 
	{"F3"	,	KEY_F3,			    0}, 
	{"F4"	,	KEY_F4,			    0}, 

	{"I"    ,	KEY_I,			    0}, 
	{"O"	,	KEY_O,				0}, 
	{"P"	,	KEY_P,				0}, 
	{"J"	,	KEY_J,				0}, 
	{"K"	,	KEY_K,				0}, 
	{"L"	,	KEY_L,				0}, 
	{"F5"	,	KEY_F5,			    0}, 
	{"F6"	,	KEY_F6,			    0}, 

    {"A"	,	KEY_A,				0}, 
	{"S"	,	KEY_S,				0}, 
	{"D"	,	KEY_D,				0}, 
	{"F"	,	KEY_F,				0}, 
	{"G"	,	KEY_G,				0}, 
	{"H"	,	KEY_H,				0}, 
	{"F7"	,	KEY_F7,			    0},
	{"F8"	,	KEY_F8,			    0}, 

    {"Caps Lock"	,	KEY_CAPSLOCK,	0},
	{"Z"	,	KEY_Z,			        0},
	{"X"	,	KEY_X,					0},
	{"C"	,	KEY_C, 			        0},
    {"V"	,	KEY_V,					0},
    {"B"	,	KEY_B,					0},
	{"F9"	,	KEY_F9,			    	0},
	{"F10"	,	KEY_F10,				0},

    {"N"	,	KEY_N,					0},
    {"M"	,	KEY_M,					0},
    {"Backspace"	,	KEY_BACKSPACE,	0},
    {"Down",	KEY_DOWN,				0},
	{"Up"	,	KEY_UP,					0},
    {"ENTER",	KEY_ENTER,				0},
	{"F11"	,	KEY_F11,			    0},
	{"F12"	,	KEY_F12,			    0},

    {"ALT"	,	KEY_ALTERASE,			0}, 
    {"."	,	KEY_DOT,				0},
    {"Left"	,	KEY_LEFT,				0}, 
    {"Right",	KEY_RIGHT,				0}, 
    {"Space"	,	KEY_SPACE,			0}, 
    {"Volup",	KEY_VOLUMEUP,			0}, 
    {"KEY_STOP"	,	KEY_STOP,			0},
    {"KEY_AGAIN",	KEY_AGAIN,			0},

    {"PROPS",	KEY_PROPS,				0},
    {"UNDO",	KEY_UNDO,				0},
    {"FRONT",	KEY_FRONT,				0},
    {"COPY"	,	KEY_COPY,				0},
    {"OPEN"	,	KEY_OPEN,				0},
    {"PASTE",	KEY_PASTE,				0},
    {"FIND"	,	KEY_FIND,				0},
    {"CUT"	,	KEY_CUT,				0},

};

/*********************************************************
 *
 * awxxxx i2c write/read byte
 *
 ********************************************************/
 //i2c_transfer will lock the adapter, so no need to add other lock in our code
static unsigned int i2c_write_byte(struct i2c_client *client,unsigned char reg_addr,unsigned char reg_data)
{
	int ret = 0;
	unsigned char wdbuf[2] = {0};

	struct i2c_msg msgs[] = {
		{
			.addr	= client->addr,
			.flags	= 0,
			.len	= 2,
			.buf	= wdbuf,
		},
	};

	if (NULL == client) {
		pr_err("msg %s i2c client is NULL\n", __func__);
		return -1;
	}
	wdbuf[0] = reg_addr;
	wdbuf[1] = reg_data;

	ret = i2c_transfer(client->adapter, msgs, 1);
	if (ret < 0) {
		pr_err("msg %s i2c write error: %d\n", __func__, ret);
	}

    return ret;

}


static unsigned int i2c_read_byte(struct i2c_client *client,unsigned char reg_addr,unsigned char *val)
{
	int ret = 0;
	unsigned char rdbuf[2] = {0};

	struct i2c_msg msgs[] = {
		{
			.addr	= client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= &reg_addr,
		},
		{
			.addr	= client->addr,
			.flags	= I2C_M_RD,
			.len	= 1,
			.buf	= rdbuf,
		},
	};

	if (NULL == client) {
		pr_err("msg %s i2c client is NULL\n", __func__);
		return -1;
	}

	ret = i2c_transfer(client->adapter, msgs, 2);
	if (ret < 0) {
		pr_err("msg %s i2c read error: %d\n", __func__, ret);
	}
	*val = rdbuf[0];

    return ret;
}


/*********************************************************
 *
 * awxxxx i2c write bytes
 *
 ********************************************************/
static int i2c_write_multi_byte(struct i2c_client *client, unsigned char reg_addr, unsigned char *buf, unsigned int len)
{
    int ret = 0;
    unsigned char *wdbuf;

    wdbuf = kmalloc(len+1, GFP_KERNEL);
    if (wdbuf == NULL) {
        pr_err("%s: can not allocate memory\n", __func__);
        return  -ENOMEM;
    }

    wdbuf[0] = reg_addr;
    memcpy(&wdbuf[1], buf, len);

    ret = i2c_master_send(client, wdbuf, len+1);
    if (ret < 0) {
        pr_err("%s: i2c master send error\n", __func__);
    }

    kfree(wdbuf);

    return ret;
}

static unsigned int i2c_read_multi_byte(struct i2c_client *client,unsigned char reg_addr,unsigned char *buf, unsigned int len)
{
	int ret = 0;
	unsigned char *rdbuf = NULL;
	struct i2c_msg msgs[] = {
		{
			.addr	= client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= &reg_addr,
		},
		{
			.addr	= client->addr,
			.flags	= I2C_M_RD,
			.len	= len,
			//.buf	= rdbuf,
		},
	};

    rdbuf = kmalloc(len, GFP_KERNEL);
    if (rdbuf == NULL) {
        pr_err("%s: can not allocate memory\n", __func__);
        return  -ENOMEM;
    }
	
	msgs[1].buf = rdbuf;

	if (NULL == client) {
		pr_err("msg %s i2c client is NULL\n", __func__);
		return -1;
		kfree(rdbuf);
	}

	ret = i2c_transfer(client->adapter, msgs, sizeof(msgs)/sizeof(msgs[0]));
	if (ret < 0) {
		pr_err("msg %s i2c read error: %d\n", __func__, ret);
	}
	if (buf != NULL) {
		memcpy(buf, rdbuf, len);
	}
	kfree(rdbuf);
    return ret;
}
/******************************************************
 *
 * aw9523 chip ctrol interface 
 *
 ******************************************************/
 /* val 0 -> led, gpio -> 1 */
static void aw9523_set_port_mode_by_mask(struct aw9523 *p_aw9523, unsigned int mask, unsigned int val)
{
	unsigned char reg_val [2] ={0};
	int i = 0;
	i2c_read_multi_byte(p_aw9523->client, P0_LED_MODE, &reg_val[0], sizeof(reg_val)/sizeof(reg_val[0]));
	for (i = 0; i < AW9523_KEY_PORT_MAX; i++) {
		if (mask & (0x01 << i)) {
			if (val) {
				if(i < 8) {
					reg_val[0] |=  0x1 << i;
				} else {
					reg_val[1] |=  0x1<<(i - 8);
				}
			} else {
				if (i < 8) {
					reg_val[0] &=  ~(0x1 << i);
				} else {
					reg_val[1] &=  ~(0x1 << (i - 8));
				}
			}
		}
	}
	i2c_write_multi_byte(p_aw9523->client, P0_LED_MODE, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
}

/* val 0 -> OD 1-> push pull */
static void aw9523_set_port_output_mode(struct aw9523 *p_aw9523, unsigned char val)
{
	unsigned char reg_val [1] = {0};
	i2c_read_byte(p_aw9523->client, CTL_REG, &reg_val[0]); //p0_0 -- p0_7
	if (val) {
		reg_val[0] |=  0x01 << 4;
	} else {
		reg_val[0] &= ~(0x01 << 4);
	}
	i2c_write_byte(p_aw9523->client, CTL_REG, reg_val[0]);
}


/* val 1-> output  0->input */ 
static void aw9523_set_port_direction_by_mask(struct aw9523 *p_aw9523, unsigned int mask, int val){
	unsigned char reg_val [2] = {0};
	int i = 0;
	i2c_read_multi_byte(p_aw9523->client, P0_CONFIG, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
	/* 0-output 1-input */
	for (i = 0; i < AW9523_KEY_PORT_MAX; i++) {
		if (mask & (0x01 << i)) {
			if (!val) {
				if (i < 8) {
					reg_val[0] |=  0x1 << i;
				} else {
					reg_val[1] |=  0x1 << (i - 8);
				}
			} else {
				if (i < 8) {
					reg_val[0] &=  ~(0x1 << i);
				} else {
					reg_val[1] &=  ~(0x1 << (i - 8));
				}
			}
		}
	}
	i2c_write_multi_byte(p_aw9523->client, P0_CONFIG, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
}


/* val 1-> output high  0->output low */ 
static void aw9523_set_port_output_state_by_mask(struct aw9523 *p_aw9523, unsigned int mask, int val){
	unsigned char reg_val [2] ={0};
	int i = 0;
	i2c_read_multi_byte(p_aw9523->client, P0_OUTPUT, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
	for (i = 0; i < AW9523_KEY_PORT_MAX; i++) {
		if (mask & (0x01 << i)) {
			if (val) {
				if (i < 8) {
					reg_val[0] |=  0x1 << i;
				} else {
					reg_val[1] |=  0x1 << (i - 8);
				}
			} else {
				if(i < 8) {
					reg_val[0] &=  ~(0x1 << i);
				} else {
					reg_val[1] &=  ~(0x1 << (i - 8));
				}
			}
		}
	}
	i2c_write_multi_byte(p_aw9523->client, P0_OUTPUT, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
}

#if 0 

/* val 1-> output high  0->output low */ 
static unsigned int aw9523_get_port_output_state_by_mask(struct aw9523 *p_aw9523, unsigned int mask, int val){
	unsigned char reg_val [2] ={0};
	i2c_write_byte(p_aw9523->client, P0_OUTPUT, reg_val[0]);
	i2c_write_byte(p_aw9523->client, P1_OUTPUT, reg_val[1]);
	return (reg_val[0]&0x0f)|((reg_val[1]<<8)&0xFF00);
}

/* val 1-> output high  0->output low */ 
static unsigned int aw9523_get_port_input_state(struct aw9523 *p_aw9523){
	unsigned char reg_val [2] ={0};
	i2c_read_multi_byte(p_aw9523->client, P0_INPUT, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
	return reg_val[0] | (reg_val[1] << 8);
}

#endif
/* val 1 -> enable 0 - > disable */
static void aw9523_enbale_interrupt_by_mask(struct aw9523 *p_aw9523, unsigned int mask, unsigned int val){
	unsigned char reg_val [2] = {0};
	int i = 0;
	i2c_read_multi_byte(p_aw9523->client, P0_INT, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
	/* 0 enable 1 disable */
	for (i = 0; i < AW9523_KEY_PORT_MAX; i++) {
		if (mask & (0x01<<i)) {
			if (!val) {
				if (i < 8) {
					reg_val[0] |=  0x1 << i;
				} else {
					reg_val[1] |=  0x1 << (i - 8);
				}
			} else {
				if(i < 8) {
					reg_val[0] &=  ~(0x1 << i);
				} else {
					reg_val[1] &=  ~(0x1 << (i - 8));
				}
			}
		}
	}
	i2c_write_multi_byte(p_aw9523->client, P0_INT, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
}

static void aw9523_clear_interrupt(struct aw9523 *p_aw9523){
	unsigned char reg_val [2] = {0};
	i2c_read_byte(p_aw9523->client, P0_INPUT, &reg_val[0]);
	i2c_read_byte(p_aw9523->client, P1_INPUT, &reg_val[1]);
}



/* val 0x00-0x03 */
static void aw9523_set_led_imax(struct aw9523 *p_aw9523, unsigned char val){
	unsigned char reg_val [1] = {0};
	i2c_read_byte(p_aw9523->client, CTL_REG, &reg_val[0]);
	reg_val[0] &= ~0x03;
	reg_val[0] |= (val & 0x03);
	i2c_write_byte(p_aw9523->client, CTL_REG, reg_val[0]);
}

static void aw9523_set_dim_by_mask(struct aw9523 *p_aw9523, unsigned int mask, unsigned int val){
	int i = 0;
	for (i = 0; i < AW9523_LED_MAX_NUMS; i++) {
		if (mask & (0x1 << i)) {
			if (i < 8){
				i2c_write_byte(p_aw9523->client, P0_0_DIM0 + i, val);
			}
			else if (i > 11) {
				i2c_write_byte(p_aw9523->client, P1_4_DIM0+ (i - 12), val);
			}
			else {
				i2c_write_byte(p_aw9523->client, P1_0_DIM0 + (i - 8), val);
			}
		}
	}
}
static int aw9523_reset(struct aw9523 *p_aw9523)
{
	int ret = 0;
	struct device_node *node = p_aw9523->dev->of_node;
	p_aw9523->rst_gpio = of_get_named_gpio(node, "reset-gpio", 0);	
	if ((!gpio_is_valid(p_aw9523->rst_gpio))) {
		dev_err(p_aw9523->dev, "%s: dts don't provide reset-gpio\n",__func__);
		return -EINVAL;
	}

	ret = gpio_request(p_aw9523->rst_gpio, "aw9523-reset");
	if (ret) {
		dev_err(&p_aw9523->client->dev, "%s: unable to request gpio [%d]\n",__func__, p_aw9523->rst_gpio);
		return ret;
	}

	ret = gpio_direction_output(p_aw9523->rst_gpio, 1);
	if (ret) {
		gpio_free(p_aw9523->rst_gpio);
		dev_err(p_aw9523->dev, "%s: unable to set direction of gpio\n",__func__);
		return ret;
	}
	gpio_set_value(p_aw9523->rst_gpio, 1);
	msleep(1);
	gpio_set_value(p_aw9523->rst_gpio, 0);
	msleep(1);
	gpio_set_value(p_aw9523->rst_gpio, 1);
	return ret;


}


static int aw9523_parse_dt_for_feature_enable(struct aw9523 *p_aw9523)
{
	int ret = 0;
	unsigned int val = 0;
	struct device_node *np = NULL;
	if (p_aw9523 == NULL || p_aw9523->dev == NULL) {
		dev_err(p_aw9523->dev, "%s: error p_aw9523 is NULL\n", __func__);
		return -EINVAL;
	}
	np = p_aw9523->dev->of_node;
	
	p_aw9523->key_feature_enable = false;
	ret = of_property_read_u32(np, "aw9523,key_enable", &val);
    if (ret) {
        dev_err(p_aw9523->dev, "%s: no aw9523,key_enable provided, The default function is not key\n", __func__);
    } else {
        AW_DEBUG("%s:aw9523,key_enable provided ok, val = %d\n", __func__, val);
		if(1 == val) {
			p_aw9523->key_feature_enable = true;
		}
    }

	p_aw9523->led_feature_enable = false;
	ret = of_property_read_u32(np, "aw9523,led_enable", &val);
	if (ret < 0) {
		dev_err(p_aw9523->dev, "%s: no aw9523,led_enable provided, The default function is not LED\n", __func__);
	} else {
		AW_DEBUG("%s:aw9523,led_enable provided ok, val = %d\n", __func__, val);
		if(1 == val) {
			p_aw9523->led_feature_enable = true;
		}
	}

	p_aw9523->gpio_feature_enable = false;
	ret = of_property_read_u32(np, "aw9523,gpio_enable", &val);
	if (ret < 0) {
		dev_err(p_aw9523->dev, "%s: aw9523,gpio_enable provided, The default function is not GPIO\n", __func__);
	} else {
		AW_DEBUG("%s:aw9523,gpio_enable provided ok, val = %d\n", __func__, val);
		if (1 == val) {
			p_aw9523->gpio_feature_enable = true;
		}
	}
	return ret;
}


/*********************************************************
 *
 * aw9523 reg
 *
 ********************************************************/
static ssize_t aw9523_get_reg(struct device* cd,struct device_attribute *attr, char* buf)
{
	unsigned char reg_val = 0;
	unsigned char i = 0;
	int j = 0;
	ssize_t len = 0;
	struct aw9523 *p_aw9523 = dev_get_drvdata(cd);
	struct i2c_client *client = p_aw9523->client;
	for (j = 0; j < 0x30; j++) {
		i2c_read_byte(client, j, &reg_val);
		len += snprintf(buf + len, PAGE_SIZE-len, "reg%2x = 0x%2x, \n", j, reg_val);
		switch(j) {
		case P0_LED_MODE:
			for (i = 0; i < 8; i++) {
				if (reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d gpio mode\n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d led mode\n", i);
				}
			}
			break;
		case P1_LED_MODE:
			for (i = 0; i < 8; i++) {
				if (reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d gpio mode\n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d led mode\n", i);
				}
			}
			len += snprintf(buf + len, PAGE_SIZE - len, "\r\n");
			break;
		case P0_CONFIG:
			for (i = 0; i < 8; i++) {
				if (reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d input mode \n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d output mode\n", i);
				}
			}
			break;
		case P1_CONFIG:
			for (i = 0; i < 8; i++) {
				if (reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d input mode\n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d output mode\n", i);
				}
			}
			len += snprintf(buf + len, PAGE_SIZE - len, "\r\n");
			break;	
		case P0_INT:
			for (i = 0; i < 8; i++) {
				if (reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d interrrupt disable\n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d interrrupt enable\n", i);
				}
			}
			break;
		case P1_INT:
			for (i = 0; i < 8; i++) {
				if (reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d interrrupt disable\n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d interrrupt enable\n", i);
				}
			}
			len += snprintf(buf + len, PAGE_SIZE - len, "\r\n");
			break;
		case P0_OUTPUT:
			for (i = 0; i < 8; i++) {
				if (reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d output high\n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d output low\n", i);
				}
			}
			break;
		case P1_OUTPUT:
			for (i = 0; i < 8; i++) {
				if(reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d output high\n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d output low\n", i);
				}
			}
			len += snprintf(buf + len, PAGE_SIZE - len, "\r\n");
			break;

		case P0_INPUT:
			for (i = 0; i < 8; i++) {
				if (reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d input high\n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P0_%d intpu low\n", i);
				}
			}
			break;
		case P1_INPUT:
			for (i = 0; i < 8; i++) {
				if (reg_val & (0x01 << i)) {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d input high\n", i);
				} else {
					len += snprintf(buf + len, PAGE_SIZE - len, "P1_%d input low\n", i);
				}
			}
			len += snprintf(buf + len, PAGE_SIZE - len, "\r\n");
			break;
		case CTL_REG:
			if (reg_val & (0x1 << 7)) {
				len += snprintf(buf + len, PAGE_SIZE - len, "gpio Pull-Push mode\n");
			} else {
				len += snprintf(buf + len, PAGE_SIZE - len, "gpio OD mode\n");
			}
			len += snprintf(buf + len, PAGE_SIZE - len, "\r\n");
			break;
		default:
			break;
		}	

	}

	return len;
}

static ssize_t aw9523_set_reg(struct device* cd, struct device_attribute *attr, const char* buf, size_t len)
{
	unsigned int databuf[2];
	struct aw9523 *p_aw9523 = dev_get_drvdata(cd);
	struct i2c_client *client = p_aw9523->client;
	if (2 == sscanf(buf,"%x %x",&databuf[0], &databuf[1])) {
		i2c_write_byte(client,databuf[0], databuf[1]);
	}
	return len;
}

static DEVICE_ATTR(reg, 0660, aw9523_get_reg,  aw9523_set_reg);
static int aw9523_create_sysfs(struct i2c_client *client)
{
	int err;
	struct device *dev = &(client->dev);
	err = device_create_file(dev, &dev_attr_reg);
	return err;
}

/*********************************************************
 *
 * aw9523 check chipid
 *
 ********************************************************/
static int aw9523_read_chipid(struct i2c_client *client)
{
	unsigned char val;
	unsigned char cnt;

	for (cnt = 5; cnt > 0; cnt--) {
		i2c_read_byte(client, ID_REG, &val);
		AW_DEBUG("aw9523_read_chipid val=0x%x\n",val);//0x80:aw9523a,0x23:aw9523b
		if (val == AW9523_ID) {
			return 0;
		}
		msleep(5);
	}
	return -EINVAL;
}

/*********************************************************
 *
 * aw9523 key feature 
 *
 ********************************************************/
static int aw9523_parse_dt_for_key(struct aw9523_key *p_key_data, struct device_node *np)
{
	int ret = 0;
	int i = 0;
	struct aw9523 *p_aw9523 = p_key_data->priv;
	ret = of_property_read_u32(np, "aw9523,input_port_mask", &p_key_data->input_port_mask);
	if (ret) {
		dev_err(p_aw9523->dev, "%s: no aw9523,input_port_mask, abort\n", __func__);
		return ret;
	}
	ret = of_property_read_u32(np, "aw9523,output_port_mask", &p_key_data->output_port_mask);
	if (ret) {
		dev_err(p_aw9523->dev, "%s, no aw9523,input_port_mask, abort\n", __func__);
		return ret;
	}
	p_key_data->input_port_nums = 0;
	for (i = 0; i < AW9523_KEY_PORT_MAX; i++) {
		if (p_key_data->input_port_mask & (0x01 << i)) {
			p_key_data->input_port_nums++;
		}
	}

	p_key_data->output_port_nums = 0;
	for (i = 0; i < AW9523_KEY_PORT_MAX; i++) {
		if (p_key_data->output_port_mask & (0x01<<i)) {
			p_key_data->output_port_nums++;
		}
	}
	p_key_data->key_mask =  p_key_data->input_port_mask | p_key_data->output_port_mask;
	AW_DEBUG("aw9523 key input_port_mask = 0x%x, input_num  = %d\n", p_key_data->input_port_mask, 
																	p_key_data->input_port_nums);
	AW_DEBUG("aw9523 key output_port_mask = 0x%x, output_nmu = %d\n", p_key_data->output_port_mask,
																	 p_key_data->output_port_nums);
	return 0;	
}


irqreturn_t aw9523_irq_func(int irq, void *key_data)
{
	struct aw9523_key *p_key_data =(struct aw9523_key *)key_data;
	struct aw9523 *p_aw9523 = p_key_data->priv;
	AW_DEBUG("%s enter\r\n", __func__);
	
	disable_irq_nosync(p_key_data->priv->irq_num);
	aw9523_enbale_interrupt_by_mask(p_aw9523, p_key_data->input_port_mask, 0);//disable aw9523b input interrupt
	schedule_delayed_work(&p_key_data->int_work, msecs_to_jiffies(p_key_data->debounce_delay));
	return 0;
}

static int aw9523_irq_register(struct aw9523 *p_aw9523)
{
	int ret = 0;
	struct device_node *np = p_aw9523->dev->of_node;
	p_aw9523->irq_gpio = of_get_named_gpio(np, "irq-gpio", 0);
	if (p_aw9523->irq_gpio < 0) {
		dev_err(p_aw9523->dev, "%s: get irq gpio failed\r\n", __func__);
		return -1;
	}
	ret = devm_gpio_request(p_aw9523->dev, p_aw9523->irq_gpio, "aw9523 irq gpio");
	if (ret) {
		dev_err(p_aw9523->dev, "%s: devm_gpio_request irq gpio failed\r\n", __func__);
		return -1;
	}
	gpio_direction_input(p_aw9523->irq_gpio);
	p_aw9523->irq_num = gpio_to_irq(p_aw9523->irq_gpio);
	if (p_aw9523->irq_num < 0) {
		ret = p_aw9523->irq_num;
		dev_err(p_aw9523->dev, "%s gpio to irq failed\r\n", __func__);
		goto err;
	}
	
	AW_DEBUG("aw9523 irq num=%d\n", p_aw9523->irq_num);
	ret = devm_request_threaded_irq(p_aw9523->dev, p_aw9523->irq_num, NULL,
									aw9523_irq_func, IRQF_TRIGGER_LOW | IRQF_ONESHOT,
									"aw9523_irq", p_aw9523->key_data);	
	if (ret < 0) {
		dev_err(p_aw9523->dev, "%s register irq failed\r\n", __func__);
		goto err;
	}
	enable_irq_wake(p_aw9523->irq_num);
	device_init_wakeup(p_aw9523->dev, 1);
	return 0;
err:
	devm_gpio_free(p_aw9523->dev, p_aw9523->irq_gpio);
	return ret;
}

static int aw9523_input_dev_register(struct aw9523 *p_aw9523){

	int ret = 0;
	int i = 0;
	struct aw9523_key * p_key_data= p_aw9523->key_data;
	p_key_data->input = input_allocate_device();
	if (p_key_data->input == NULL) {
		ret = -ENOMEM;
		dev_err(p_aw9523->dev, "%s: failed to allocate input device\n", __func__);
		return ret;
	}	
	p_key_data->input->name = "aw9523-key";
	p_key_data->input->dev.parent = p_aw9523->dev;
	p_key_data->keymap_len = sizeof(key_map)/sizeof(KEY_STATE);
	p_key_data->keymap = (KEY_STATE *)&key_map;
	input_set_drvdata(p_key_data->input, p_key_data);

	__set_bit(EV_KEY, p_key_data->input->evbit);
	__set_bit(EV_SYN, p_key_data->input->evbit);
	for (i = 0; i < p_key_data->keymap_len; i++) {
		__set_bit(p_key_data->keymap[i].key_code & KEY_MAX, p_key_data->input->keybit);
	}
	
	ret = input_register_device(p_key_data->input);
	if (ret) {
		input_free_device(p_key_data->input);
		dev_err(p_aw9523->dev, "%s: failed to allocate input device\n", __func__);
		return ret;
	}
	return 0;
}

void aw9523_int_work(struct work_struct *work)
{
	struct delayed_work *p_delayed_work = container_of(work, struct delayed_work, work);
	struct aw9523_key *p_key_data = container_of(p_delayed_work,struct aw9523_key, int_work);
	AW_DEBUG("%s enter, %d\r\n", __func__, __LINE__);
	schedule_work(&p_key_data->key_work);
}

static void aw9523_key_work(struct work_struct *work)
{
	int i = 0;
	int j = 0;
	int real_idx = 0;
	int real_row = 0;
	int real_col = 0;
	int key_code = 0;
	int key_val = 0;
	unsigned int input_val = 0;
	struct aw9523_key *p_key_data = container_of(work,struct aw9523_key, key_work);	
	struct aw9523 *p_aw9523 = p_key_data->priv;
	unsigned int *new_state = p_key_data->new_output_state;
	unsigned int *old_state = p_key_data->old_output_state;
	unsigned char reg_val[2] = {0};

	AW_DEBUG("%s enter\n", __func__);
	for (i = 0, real_idx = 0; i < AW9523_KEY_PORT_MAX; i++) {
		if (p_key_data->output_port_mask & (0x01 << i)) {
			i2c_read_multi_byte(p_aw9523->client, P0_CONFIG, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
			input_val = reg_val[0] | (reg_val[1] << 8);
			input_val |=  p_key_data->output_port_mask;
			input_val &= ~(0x01 << i);
			i2c_write_multi_byte(p_aw9523->client, P0_CONFIG, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
			i2c_read_multi_byte(p_aw9523->client, P0_INPUT, reg_val, sizeof(reg_val)/sizeof(reg_val[0]));
			new_state[real_idx] = (reg_val[0] | (reg_val[1] << 8)) & p_key_data->input_port_mask;
			real_idx++;
		}
	}

	// key state change
	if (memcmp(&new_state[0], &old_state[0], p_key_data->output_port_nums * sizeof(unsigned int))) { 
		// stage changed
		for (i = 0, real_col = 0; i < AW9523_KEY_PORT_MAX; i++) {
			if (p_key_data->output_port_mask & (0x01 << i)) {
				if (new_state[real_col] != old_state[real_col]) {
					for (j = 0, real_row = 0;j < AW9523_KEY_PORT_MAX; j++) {
						if (p_key_data->input_port_mask & (0x01 << j)) {
							if ((new_state[real_col] & (0x01 << j)) != (old_state[real_col] & (0x01<<j))) {
								key_code = p_key_data->keymap[real_row * p_key_data->output_port_nums + real_col].key_code;
								key_val = (old_state[real_col] & (0x01 << j)) ? 1 : 0;// press or release
								AW_DEBUG("aw9523 report: key_code = %d, key_val = %d\n", key_code, key_val);
								AW_DEBUG("aw9523 real_row = %d, real_col = %d\n", real_row, real_col);
								input_report_key(p_key_data->input, key_code, key_val);
								input_sync(p_key_data->input);
							}
							real_row++;
						}
					}
				}
				real_col++;
			}
		}
			
	}
	memcpy(&old_state[0], &new_state[0], p_key_data->output_port_nums * sizeof(unsigned int));

	// debug info
	for (i = 0, real_idx = 0; i < AW9523_KEY_PORT_MAX; i++) {
		if (p_key_data->output_port_mask & (0x01 << i)) {
			AW_DEBUG("new-state[%d] = 0x%x, old_state[%d] = 0x%x\r\n", real_idx, new_state[real_idx], real_idx, old_state[real_idx]);
			real_idx++;
		}
	}

	// all key release
	if (!memcmp(&new_state[0], &p_key_data->def_output_state[0], p_key_data->output_port_nums * sizeof(unsigned int))) {
		aw9523_set_port_direction_by_mask(p_aw9523, p_key_data->output_port_mask, 1); // set output mode
		aw9523_clear_interrupt(p_aw9523); // clear inputerrupt 
		enable_irq(p_aw9523->irq_num);
		aw9523_enbale_interrupt_by_mask(p_aw9523, p_key_data->input_port_mask, 1);//enable input interrupt
		return;
	}
	hrtimer_start(&p_key_data->key_timer, ktime_set(0,(1000/HRTIMER_FRAME)*1000), HRTIMER_MODE_REL);
		
}

enum hrtimer_restart aw9523_timer_func(struct hrtimer *p_hrtimer)
{
	struct aw9523_key *p_key_data = container_of(p_hrtimer,struct aw9523_key, key_timer);
	schedule_work(&p_key_data->key_work);
	return HRTIMER_NORESTART;
}


static void aw9523_key_chip_init(struct aw9523_key *p_key_data)
{
	unsigned int all_mask = 0;
	struct aw9523 *p_aw9523 = p_key_data->priv;
	disable_irq_nosync(p_key_data->priv->irq_num);
	all_mask = p_key_data->input_port_mask|p_key_data->output_port_mask;
	aw9523_set_port_mode_by_mask(p_aw9523, all_mask, 1); //set  gpio mode
	aw9523_set_port_direction_by_mask(p_aw9523, p_key_data->input_port_mask, 0); // input mode
	aw9523_set_port_direction_by_mask(p_aw9523, p_key_data->output_port_mask, 1); // output mode
	aw9523_set_port_output_mode(p_aw9523, 1); //set output port pull push mode
	
	aw9523_set_port_output_state_by_mask(p_aw9523, p_key_data->output_port_mask, 0);//set output low 

	aw9523_enbale_interrupt_by_mask(p_aw9523, p_key_data->input_port_mask, 1); //enable input interrupt
	aw9523_enbale_interrupt_by_mask(p_aw9523, p_key_data->output_port_mask, 0); //disale output interrupt
	// clear inputerrupt 
	aw9523_clear_interrupt(p_aw9523);
	enable_irq(p_key_data->priv->irq_num);
}

static void aw9523_key_free_all_resource(struct aw9523 *p_aw9523)
{
	if (p_aw9523->key_feature_enable) {
		devm_kfree(p_aw9523->dev,p_aw9523->key_data->new_output_state);
		devm_kfree(p_aw9523->dev,p_aw9523->key_data->old_output_state);
		devm_kfree(p_aw9523->dev,p_aw9523->key_data->def_output_state);
		input_unregister_device(p_aw9523->key_data->input);
		input_free_device(p_aw9523->key_data->input);
		devm_gpio_free(p_aw9523->dev, p_aw9523->irq_gpio);
		devm_kfree(p_aw9523->dev,p_aw9523->key_data);
	}	
}



static int aw9523_key_feature_init(struct aw9523 *p_aw9523)
{
	int ret = 0;
	int i = 0;
	struct aw9523_key *p_key_data = NULL;
	struct device_node *key_node = NULL;
	AW_DEBUG("%s enter\r\n", __func__);
	p_key_data = devm_kzalloc(p_aw9523->dev, sizeof(struct aw9523_key), GFP_KERNEL);
	if ( p_key_data == NULL) {
		dev_err(p_aw9523->dev, "%s: malloc memory failed\n", __func__);
		return -ENOMEM;
	}
	p_aw9523->key_data = p_key_data;
	p_key_data->priv = p_aw9523;
	key_node = of_find_node_by_name(p_aw9523->dev->of_node, "aw9523,key");
	if (key_node == NULL) {
		dev_err(p_aw9523->dev, "%s: can't find aw9523,key node return failed\n", __func__);
		ret = -EINVAL;
		goto err_id;
	}
	ret = aw9523_parse_dt_for_key(p_key_data, key_node);
	if (ret) {
		dev_err(p_aw9523->dev, "aw9523_parse_dt_for_key failed, check dts\n");
		goto err_id;
	}
	p_key_data->old_output_state = devm_kzalloc(p_aw9523->dev, 
												sizeof(unsigned int) * p_key_data->output_port_nums , GFP_KERNEL);
	if (p_key_data->old_output_state == NULL) {
		dev_err(p_aw9523->dev, "%s:p_key_data->old_output_state malloc memory failed\r\n", __func__);
		goto err_id;
	}
	p_key_data->new_output_state = devm_kzalloc(p_aw9523->dev, 
												sizeof(unsigned int) * p_key_data->output_port_nums, GFP_KERNEL);
	if (p_key_data->new_output_state == NULL) {
		dev_err(p_aw9523->dev, "%s:p_key_data->new_output_state malloc memory failed\r\n", __func__);
		goto free_old_output;
	}
	p_key_data->def_output_state = devm_kzalloc(p_aw9523->dev, 
												sizeof(unsigned int) * p_key_data->output_port_nums, GFP_KERNEL);
	if (p_key_data->def_output_state  == NULL) {
		dev_err(p_aw9523->dev, "%s:p_key_data->def_output_state malloc memory failed\r\n", __func__);
		ret = -ENOMEM;
		goto free_new_output;
	}
	for (i = 0; i < p_key_data->output_port_nums; i++) {
		p_key_data->new_output_state[i] = p_key_data->input_port_mask;
		p_key_data->old_output_state[i] = p_key_data->input_port_mask;
		p_key_data->def_output_state[i] = p_key_data->input_port_mask;
	}

	ret = aw9523_input_dev_register(p_aw9523);
	if (ret < 0) {
		dev_err(p_aw9523->dev, "%s input dev register failed\r\n", __func__);
		goto free_def_output;
	}
	
	p_key_data->debounce_delay = 1;
	INIT_DELAYED_WORK(&p_key_data->int_work, aw9523_int_work);
	INIT_WORK(&p_key_data->key_work, aw9523_key_work);
	hrtimer_init(&p_key_data->key_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	p_key_data->key_timer.function = aw9523_timer_func;
	ret = aw9523_irq_register(p_aw9523);
	if (ret < 0) {
		dev_err(p_aw9523->dev, "%s irq register failed\r\n", __func__);
		goto input_dev;
	}
	aw9523_key_chip_init(p_key_data);
	return 0;
input_dev:
	input_unregister_device(p_key_data->input);
free_def_output:
	devm_kfree(p_aw9523->dev, p_key_data->def_output_state);
free_new_output:
	devm_kfree(p_aw9523->dev, p_key_data->new_output_state);
free_old_output:
	devm_kfree(p_aw9523->dev, p_key_data->old_output_state);
err_id:
	devm_kfree(p_aw9523->dev, p_aw9523->key_data);
	return ret;
}



/******************************************************
 *
 * led feature start
 *
 ******************************************************/
/* led sys node */
/* echo imax > blink_param */
static ssize_t aw9523_imax_store(struct device* dev, struct device_attribute *attr,
                const char* buf, size_t len)
{
    unsigned int databuf[1] = {0};
    struct aw9523 *p_aw9523 = dev_get_drvdata(dev);
	struct aw9523_led *p_led_data = p_aw9523->led_data;
	sscanf(buf,"%x",&databuf[0]);
	p_led_data->imax = databuf[0] & 0x03;
	aw9523_set_led_imax(p_aw9523, p_led_data->imax);
    return len;
}

static ssize_t aw9523_imax_show(struct device* dev,struct device_attribute *attr, char* buf)
{
    ssize_t len = 0;
    len += snprintf(buf+len, PAGE_SIZE-len, "echo imax> blink_param\n");
    return len;
}

static DEVICE_ATTR(imax, S_IWUSR | S_IRUGO, aw9523_imax_show, aw9523_imax_store);
static struct attribute *aw9523_led_attributes[] = {
    &dev_attr_imax.attr,
    NULL
};

static struct attribute_group aw9523_led_attribute_group = {
    .attrs = aw9523_led_attributes
};


static int aw9523_parse_dt_for_led_global_param(struct device_node *led_node, 
																  struct aw9523 *p_aw9523,
																  struct aw9523_led *p_led_data)
{
	int ret = 0;
	ret = of_property_read_u32(led_node, "aw9523,default_imax", &p_led_data->imax);
	if (ret < 0) {
		dev_err(p_aw9523->dev, "%s: no aw9523,default_imax, abort\n", __func__);
		return ret;
	}		
	return 0;
}

static void aw9523_brightness_work(struct work_struct *work){
	struct aw9523_single_led * p_single_led_data = container_of(work,struct aw9523_single_led, brightness_work);
	struct aw9523 * p_aw9523 = p_single_led_data->priv;
	
	AW_DEBUG("%s enter, %d 0x%x\r\n", __func__, __LINE__, p_single_led_data->idx);
	aw9523_set_dim_by_mask(p_aw9523, p_single_led_data->idx, p_single_led_data->cur_brightness);
}




void aw9523_brightness_set(struct led_classdev *led_cdev,enum led_brightness brightness)
{
	struct aw9523_single_led * p_single_led_data = container_of(led_cdev,struct aw9523_single_led, cdev);
	if (brightness > p_single_led_data->max_brightness) {
		brightness = p_single_led_data->max_brightness;
	}
	p_single_led_data->cdev.brightness = brightness;
	p_single_led_data->cur_brightness = brightness;
	schedule_work(&p_single_led_data->brightness_work);
}

enum led_brightness aw9523_brightness_get(struct led_classdev *led_cdev)
{
	struct aw9523_single_led * p_single_led_data = container_of(led_cdev,struct aw9523_single_led, cdev);
	return p_single_led_data->cur_brightness;
}


static int aw9523_parse_for_single_led(struct device_node *led_node, 
													struct aw9523 *p_aw9523, 
													struct aw9523_led *p_led_data)
{
	int ret = 0;
	int i = 0;
	int j = 0;
	struct device_node *temp = NULL;
	unsigned int idx_count = 0;
	unsigned int *idx_array = NULL;
	struct aw9523_single_led * p_single_led_data = p_led_data->single_led_data;
	
	AW_DEBUG("%s enter, %d\r\n", __func__, __LINE__);
	if((p_led_data == NULL) || (p_led_data->single_led_data == NULL)) {
		return -EINVAL;
	}
	for_each_child_of_node(led_node, temp) {
		ret = of_property_read_string(temp, "aw9523,name", &p_single_led_data[i].cdev.name);
		if (ret) {
			dev_err(p_aw9523->dev, "%s:dts parse aw9523,name failed, idx = %d\n", __func__, i);
			goto err_id;
		}	
		ret = of_property_read_u32(temp, "aw9523,idx_count", &idx_count);
		if (ret < 0) {
			dev_err(p_aw9523->dev, "%s:dts parse aw9523,idx_count, idx = %d\n", __func__, i);
			goto err_id;
		}	
		idx_array = kzalloc(sizeof(unsigned int)*idx_count, GFP_KERNEL);
		if (idx_array == NULL) {
			dev_err(p_aw9523->dev, "%s:kmalloc failed\r\n", __func__);
			ret = -ENOMEM;
			goto err_id;
		}
		ret = of_property_read_u32_array(temp, "aw9523,idx", idx_array, idx_count);
		if (ret < 0) {
			dev_err(p_aw9523->dev, "%s:dts parse aw9523,idx failed, idx = %d\n", __func__, i);
			kfree(idx_array);
			goto err_id;
		}
		for (j = 0; j < idx_count; j++) {
			if (idx_array[j] < AW9523_LED_MAX_NUMS) {
				p_single_led_data[i].idx |= (0x01 << idx_array[j]);
			} else {
				kfree(idx_array);
				dev_err(p_aw9523->dev, "%s: dts err: aw9523,idx parameter unavailable, idx =  %d\n", __func__, i);
				goto err_id;
			}
		}
		kfree(idx_array);
		ret = of_property_read_u32(temp, "aw9523,default_brightness", &p_single_led_data[i].cur_brightness);
		if (ret < 0) {
			dev_err(p_aw9523->dev, "%s:dts parse aw9523,default_brightness failed, idx = %d\n", __func__, i);
			goto err_id;
		}
		ret = of_property_read_u32(temp, "aw9523,max_brightness", &p_single_led_data[i].max_brightness);
		if (ret < 0) {
			dev_err(p_aw9523->dev, "%s:dts parse aw9523,idx failed, idx = %d\n", __func__, i);
			goto err_id;
		}
		p_led_data->led_mask |= p_single_led_data[i].idx;
		AW_DEBUG("aw9523 single led: i = %d, name = %s, idx_count = %d, idx = 0x%x\n", i, 
																		  p_single_led_data[i].cdev.name,
																		  idx_count,
																		  p_single_led_data[i].idx);
		i++;
	}

	AW_DEBUG("aw9523 led_maks = 0x%x\n", p_led_data->led_mask);
	
	/* register led classdev */
	for (i = 0; i < p_led_data->led_nums; i++) {
		p_single_led_data[i].cdev.max_brightness = p_single_led_data[i].max_brightness;
		p_single_led_data[i].cdev.brightness_set = aw9523_brightness_set;
		p_single_led_data[i].cdev.brightness_get = aw9523_brightness_get;
		INIT_WORK(&p_single_led_data[i].brightness_work, aw9523_brightness_work);
		p_single_led_data[i].priv = p_aw9523;
		ret = led_classdev_register(p_aw9523->dev, &p_single_led_data[i].cdev);
		if (ret) {
			dev_err(p_aw9523->dev, "led classdev register failed, idx = %d\r\n", i);
			goto free_led_classdev;
		}
	}
	return 0;
free_led_classdev:
	for (i--; i > 0 || i == 0; i--) {
		led_classdev_unregister(&p_single_led_data[i].cdev);
	}
err_id:
	return ret;
}



static int aw9523_led_chip_init(struct aw9523 *p_aw9523, struct aw9523_led *p_led_data)
{
	int i = 0;
	aw9523_set_port_mode_by_mask(p_aw9523, p_led_data->led_mask, 0); // set led mode
	aw9523_set_led_imax(p_aw9523, p_led_data->imax); // set led breath level range
	for (i = 0; i < p_led_data->led_nums; i++) {
		aw9523_set_dim_by_mask(p_aw9523, p_led_data->single_led_data[i].idx, 
							   p_led_data->single_led_data[i].cur_brightness);
	}
	return 0;
}

static void aw9523_led_free_all_resource(struct aw9523 *p_aw9523)
{
	int i = 0;
	if (p_aw9523->led_feature_enable) {
		for (i = 0; i < p_aw9523->led_data->led_nums; i++) {
			led_classdev_unregister(&p_aw9523->led_data->single_led_data[i].cdev);
		}
		devm_kfree(p_aw9523->dev,p_aw9523->led_data);
	}
}

static int aw9523_led_feature_init(struct aw9523 *p_aw9523){
	int ret = 0;
	int i = 0;
	struct device_node *led_node = NULL;
	struct aw9523_led *p_led_data = NULL;
	
	AW_DEBUG("%s enter\r\n", __func__);
	p_led_data = devm_kzalloc(p_aw9523->dev, sizeof(struct aw9523_led), GFP_KERNEL);
	if (p_led_data == NULL) {
		dev_err(p_aw9523->dev, "%s: malloc memory failed\r\n", __func__);
		return -ENOMEM;
	}
	p_aw9523->led_data = p_led_data;
	led_node = of_find_node_by_name(p_aw9523->dev->of_node, "aw9523,led");
	if (led_node == NULL) {
		dev_err(p_aw9523->dev, "%s: can't find aw9523,led node return failed\r\n", __func__);
		ret = -EINVAL;
		goto err_id;
	}
	ret = aw9523_parse_dt_for_led_global_param(led_node, p_aw9523, p_led_data);
	if (ret) {
		dev_err(p_aw9523->dev, "%s parse dt for led golbal param failed\r\n", __func__);
		goto err_id;
	}
	p_led_data->led_nums = of_get_child_count(led_node);
	AW_DEBUG("%s enter, %d led nums = %d\r\n", __func__, __LINE__,p_led_data->led_nums);
	
	p_led_data->single_led_data = devm_kzalloc(p_aw9523->dev, 
											   sizeof(struct aw9523_single_led) * p_led_data->led_nums, 
											   GFP_KERNEL);
	if (p_led_data->single_led_data == NULL) {
		dev_err(p_aw9523->dev, "%s: malloc memory failed\r\n", __func__);
		ret = -ENOMEM;
		goto err_id;
	}
	ret = aw9523_parse_for_single_led(led_node, p_aw9523, p_led_data);
	if (ret < 0) {
		dev_err(p_aw9523->dev, "aw9523_parse_single_led failed\r\n");
		goto free_single_led;
	}
    ret = sysfs_create_group(&p_aw9523->dev->kobj,&aw9523_led_attribute_group);
    if (ret) {
        dev_err(p_aw9523->dev, "led sysfs failed ret: %d\n", ret);
        goto free_led_class;
    }
	aw9523_led_chip_init(p_aw9523, p_led_data);
	return 0;
	
free_led_class:
	for (i = 0; i < p_led_data->led_nums; i++) {
		led_classdev_unregister(&p_led_data->single_led_data[i].cdev);
	}
free_single_led:
	devm_kfree(p_aw9523->dev, p_led_data->single_led_data);
err_id:
	devm_kfree(p_aw9523->dev, p_led_data);
	return ret;

}


/******************************************************
 *
 * gpio feature start
 *
 ******************************************************/
 /* gpio sys node */

/* echo gpio_idx dirction state > aw9523_gpio */
 static ssize_t aw9523_gpio_store(struct device* dev, struct device_attribute *attr,
				 const char* buf, size_t len)
 {
	unsigned int databuf[3];
	int i = 0;
	struct aw9523 *p_aw9523 = dev_get_drvdata(dev);
	struct aw9523_gpio *p_gpio_data =  p_aw9523->gpio_data; 
	struct aw9523_singel_gpio *p_single_gpio_data =  p_gpio_data->single_gpio_data;
	sscanf(buf,"%d %d %d",&databuf[0], &databuf[1], &databuf[2]);
	AW_DEBUG("aw9523 gpio cmd param: %d %d %d\n", databuf[0], databuf[1], databuf[2]);
	if (p_gpio_data->gpio_mask & (0x01 << databuf[0])) {
		for (i = 0; i < p_gpio_data->gpio_num; i++) {
			if (p_single_gpio_data[i].gpio_idx == databuf[0]) {
				if (p_single_gpio_data[i].gpio_direction != databuf[1]) {
					p_single_gpio_data[i].gpio_direction = databuf[1];
					aw9523_set_port_direction_by_mask(p_aw9523, 0x1 << p_single_gpio_data[i].gpio_idx, 
													  p_single_gpio_data[i].gpio_direction);			
				}
				if (p_single_gpio_data[i].gpio_direction == 0x01) { // output
					if (p_single_gpio_data[i].state != databuf[2]) {
						p_single_gpio_data[i].state = databuf[2];
						aw9523_set_port_output_state_by_mask(p_aw9523, 
															 0x1 << p_single_gpio_data[i].gpio_idx,
															 p_single_gpio_data[i].state);
					}
				}
			}
		}
		
	}
	return len;
 }
 
 static ssize_t aw9523_gpio_show(struct device* dev,struct device_attribute *attr, char* buf)
 {
	 ssize_t len = 0;
	 int i = 0;
	 struct aw9523 *p_aw9523 = dev_get_drvdata(dev);
	 struct aw9523_singel_gpio *p_single_gpio_data = p_aw9523->gpio_data->single_gpio_data; 
	 len += snprintf(buf+len, PAGE_SIZE-len, "Uasge: echo gpio_idx dirction state > aw9523_gpio\n"); 
	 for (i = 0; i < p_aw9523->gpio_data->gpio_num; i++) {
	 	len += snprintf(buf+len, PAGE_SIZE-len, "aw9523 gpio idx = %d, dir = %d, state = %d", p_single_gpio_data[i].gpio_idx, 
													p_single_gpio_data[i].gpio_direction, 
													p_single_gpio_data[i].state);
	 }
	 return len;
 }
 
 
 static DEVICE_ATTR(aw9523_gpio, S_IWUSR | S_IRUGO, aw9523_gpio_show, aw9523_gpio_store);
 static struct attribute *aw9523_gpio_attributes[] = {
	 &dev_attr_aw9523_gpio.attr,
	 NULL
 };
 static struct attribute_group aw9523_gpio_attribute_group = {
	 .attrs = aw9523_gpio_attributes
 };




 static int aw9523_parse_for_single_gpio(struct device_node *gpio_node, 
 													   struct aw9523 *p_aw9523, 
 													   struct aw9523_gpio *p_gpio_data)
 {
	 int ret = 0;
	 struct device_node *temp = NULL;
	 int i = 0;
	 struct aw9523_singel_gpio * p_single_gpio_data = p_gpio_data->single_gpio_data;
	 if ((p_gpio_data == NULL) || (p_gpio_data->single_gpio_data == NULL)) {
		 return -1;
	 }
	 for_each_child_of_node(gpio_node, temp) {
		 ret = of_property_read_u32(temp, "aw9523,gpio_idx", &p_single_gpio_data[i].gpio_idx);
		 if (ret < 0) {
			 dev_err(p_aw9523->dev, "%s: no aw9523,gpio_idx, abort\n", __func__);
			 goto err_id;
		 }		 
		 ret = of_property_read_u32(temp, "aw9523,gpio_dir", &p_single_gpio_data[i].gpio_direction);
		 if (ret < 0) {
			 dev_err(p_aw9523->dev, "%s, no aw9523,gpio_dir\n", __func__);
			 goto err_id;
		 }
		 ret = of_property_read_u32(temp, "aw9523,gpio_default_val", &p_single_gpio_data[i].state);
		 if (ret < 0) {
			 dev_err(p_aw9523->dev, "%s, no aw9523,gpio_default_val, abort\n", __func__);
			 goto err_id;
		 }
		 p_gpio_data->gpio_mask |= 0x01<<p_single_gpio_data[i].gpio_idx;
		 AW_DEBUG("idx = %d, aw9523,gpio_idx = %d\n", i, p_single_gpio_data[i].gpio_idx);
		 i++;
		
	 }
	 AW_DEBUG("%s gpio_mask = 0x%x\r\n",__func__, p_gpio_data->gpio_mask);
	 return 0;
 err_id:
	 return ret;
 }

 static void aw9523_gpio_chip_init(struct aw9523 *p_aw9523, struct aw9523_gpio *p_gpio_data)
 {
 	int i = 0;
	struct aw9523_singel_gpio *p_single_gpio_data = p_gpio_data->single_gpio_data;
	aw9523_set_port_mode_by_mask(p_aw9523, p_gpio_data->gpio_mask, 1); // gpio mode
	aw9523_set_port_output_mode(p_aw9523, p_gpio_data->output_mode); // OD or pull push mode
	for (i = 0; i < p_gpio_data->gpio_num; i++) {
		aw9523_set_port_direction_by_mask(p_aw9523, 0x1 << p_single_gpio_data[i].gpio_idx, 
										  p_single_gpio_data[i].gpio_direction);
		if (p_single_gpio_data[i].gpio_direction) { // output
			aw9523_set_port_output_state_by_mask(p_aw9523, 0x1<<p_single_gpio_data[i].gpio_idx, 
												 p_single_gpio_data[i].state);
		}
	}
}

static void aw9523_gpio_free_all_resource(struct aw9523 *p_aw9523)
{
	if (p_aw9523->gpio_feature_enable) {
		devm_kfree(p_aw9523->dev, p_aw9523->gpio_data->single_gpio_data);
		devm_kfree(p_aw9523->dev,p_aw9523->gpio_data);
	}
}

static int aw9523_gpio_feature_init(struct aw9523 *p_aw9523)
{
	int ret = 0;
	int i = 0;
	struct device_node *gpio_node = NULL;
	struct aw9523_gpio *p_gpio_data = NULL;
	int gpio_num = 0;
	p_gpio_data = devm_kzalloc(p_aw9523->dev, sizeof(struct aw9523_gpio), GFP_KERNEL);
	if (p_gpio_data == NULL) {
		dev_err(p_aw9523->dev, "%s: malloc memory failed\r\n", __func__);
		return -ENOMEM;
	}
	p_aw9523->gpio_data = p_gpio_data;

	gpio_node = of_find_node_by_name(p_aw9523->dev->of_node, "aw9523,gpio");
	if (gpio_node == NULL) {
		dev_err(p_aw9523->dev, "%s: can't find aw9523,gpio return failed\r\n", __func__);
		ret = -1;
		goto err_id;
	}
	ret = of_property_read_u32(gpio_node, "aw9523,gpio_mode", &p_gpio_data->output_mode);
	if (ret < 0) {
		dev_err(p_aw9523->dev, "%s: no aw9523,gpio_mode, abort\n", __func__);
		goto err_id;
	}	
	gpio_num = of_get_child_count(gpio_node);
	p_gpio_data->gpio_num = gpio_num;
	p_gpio_data->single_gpio_data = devm_kzalloc(p_aw9523->dev, sizeof(struct aw9523_singel_gpio) * gpio_num, GFP_KERNEL);
	if (p_gpio_data->single_gpio_data == NULL) {
		dev_err(p_aw9523->dev, "%s: malloc memory failed\r\n", __func__);
		ret = -ENOMEM;
		goto err_id;
	}	
	ret = aw9523_parse_for_single_gpio(gpio_node, p_aw9523, p_gpio_data);
	if (ret) {
		dev_err(p_aw9523->dev, "aw9523_parse_single_led failed\r\n");
		goto free_mem;
	}
	for (i = 0; i < gpio_num; i++) {
		p_gpio_data->single_gpio_data[i].priv = p_aw9523;
	}
	ret = sysfs_create_group(&p_aw9523->dev->kobj,
	        &aw9523_gpio_attribute_group);
	if (ret) {
	    dev_err(p_aw9523->dev, "led sysfs failed ret: %d\n", ret);
	    goto free_mem;
	}
	aw9523_gpio_chip_init(p_aw9523, p_gpio_data);
	return 0;
free_mem:
	devm_kfree(p_aw9523->dev, p_gpio_data->single_gpio_data);
	devm_kfree(p_aw9523->dev, p_aw9523->gpio_data);
err_id:
	return ret;

}
/*********************************************************
 *
 * aw9523 driver
 *
 ********************************************************/
static int aw9523_key_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct aw9523 *p_aw9523 = NULL;
	int ret = 0;
	AW_DEBUG("%s enter,%s\n", __func__, AW9523B_DRIVER_VERSION);
	if (!i2c_check_functionality(client->adapter,I2C_FUNC_I2C)) {
		dev_err(&client->dev, "I2C transfer not Supported\n");
		return -EIO;
	}
	
	p_aw9523 = devm_kzalloc(&client->dev,sizeof(struct aw9523), GFP_KERNEL);
	if (!p_aw9523) {
		dev_err(&client->dev, "Failed to allocate memory\n");
		return -ENOMEM;
	}
	p_aw9523->dev = &client->dev;
	p_aw9523->client = client;
	i2c_set_clientdata(client, p_aw9523);
	
	ret = aw9523_reset(p_aw9523);
	if (ret) {
		goto err_free_mem;
	}
	if (aw9523_read_chipid(client)) {
		dev_err(&client->dev, "%s: read_chipid error\n", __func__);
		goto err_free_rst;
	}
	
	/* create debug dev node - reg */
	aw9523_create_sysfs(client);

	aw9523_parse_dt_for_feature_enable(p_aw9523);
	
	if (p_aw9523->key_feature_enable) {
		// key init
		ret = aw9523_key_feature_init(p_aw9523);
		if (ret) {
			dev_err(p_aw9523->dev, "aw9523 key feature init failed \r\n");
			goto err_free_rst;
		}
	}

	if (p_aw9523->led_feature_enable) {
	  	// led init
		ret = aw9523_led_feature_init(p_aw9523);
		if (ret) {
			dev_err(p_aw9523->dev, "aw9523 led feature init failed \r\n");
			goto free_key;
		}
	}

	if (p_aw9523->gpio_feature_enable) {
	  	// gpio init
		ret = aw9523_gpio_feature_init(p_aw9523);
		if (ret) {
			dev_err(p_aw9523->dev, "aw9523 gpio feature init failed \r\n");
			goto free_led;
		}
	}
    AW_DEBUG("aw9523_key_i2c_probe success!\n");
	return 0;
free_led:
	aw9523_led_free_all_resource(p_aw9523);
free_key:
	aw9523_key_free_all_resource(p_aw9523);
err_free_rst:
	gpio_free(p_aw9523->rst_gpio);
err_free_mem:
	devm_kfree(&client->dev,p_aw9523);
    AW_DEBUG("aw9523_key_i2c_probe fail!\n");
	return ret;
}

static int aw9523_key_i2c_remove(struct i2c_client *client)
{
	struct aw9523 *p_aw9523 = i2c_get_clientdata(client);
	aw9523_key_free_all_resource(p_aw9523);
	aw9523_led_free_all_resource(p_aw9523);
	aw9523_gpio_free_all_resource(p_aw9523);
	devm_kfree(p_aw9523->dev,p_aw9523);
	return 0;
}

static const struct of_device_id aw9523_keypad_of_match[] = {
	{ .compatible = "awinic,aw9523b",},
	{},
};

static const struct i2c_device_id aw9523_key_i2c_id[] = {
	{"awinic,aw9523b", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, aw9523_key_i2c_id);

static struct i2c_driver aw9523_key_i2c_driver = {
	.driver = {
		.name = "aw9523-key",
		.owner = THIS_MODULE,
		.of_match_table = aw9523_keypad_of_match,
	},
	.probe = aw9523_key_i2c_probe,
	.remove = aw9523_key_i2c_remove,
	.id_table = aw9523_key_i2c_id,
};

static int __init aw9523_key_i2c_init(void)
{
	int ret = 0;

	AW_DEBUG("%s enter\n", __func__);

	ret = i2c_add_driver(&aw9523_key_i2c_driver);
	if (ret) {
		pr_err("fail to add aw9523 device into i2c\n");
		return ret;
	}

	AW_DEBUG("%s exit\n", __func__);

	return 0;
}
module_init(aw9523_key_i2c_init);

static void __exit aw9523_key_i2c_exit(void)
{
	AW_DEBUG("%s enter\n", __func__);

	i2c_del_driver(&aw9523_key_i2c_driver);

	AW_DEBUG("%s exit\n", __func__);
}
module_exit(aw9523_key_i2c_exit);

MODULE_AUTHOR("luofuhong@awinic.com.cn");
MODULE_DESCRIPTION("AW9523B driver");
MODULE_LICENSE("GPL");
