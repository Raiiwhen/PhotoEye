/*
 * Copyright (c) 2006-2020, TJ Team
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-17     ÌÆÎÄ¾ü       the first version
 */
#ifndef I2C_H_
#define I2C_H_

#define BSP_USING_I2C1

#define RT_I2C_WR                0x0000
#define RT_I2C_RD               (1u << 0)
#define RT_I2C_ADDR_10BIT       (1u << 2)  /* this is a ten bit chip address */
#define RT_I2C_NO_START         (1u << 4)
#define RT_I2C_IGNORE_NACK      (1u << 5)
#define RT_I2C_NO_READ_ACK      (1u << 6)  /* when I2C reading, we do not ACK */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef enum 
{
	I2C_OK 		= 	0,
	I2C_ERROR 	= 	1
}i2c_err_t;

typedef enum
{
	I2C_LOW  	= 	0,
	I2C_HIGH 	= 	1,
}i2c_polarity_t;

typedef enum
{
	I2C_OUT 	= 	0,
	I2C_IN  	= 	1
}i2c_mode_t;

typedef struct 
{
    void 			(*i2c_scl_out)(i2c_polarity_t polar);
    void 			(*i2c_sda_out)(i2c_polarity_t polar);
    i2c_polarity_t 	(*i2c_sda_in)(void);
	void			(*i2c_sda_mode)(i2c_mode_t mode);
	void			(*i2c_delay_us)(uint32_t value);
}i2c_bus_ops_t;

typedef struct
{
    uint16_t addr;
    uint16_t flags;
    uint16_t len;
    uint8_t  *buf;
}i2c_msg_t;

/*for i2c bus driver*/
struct i2c_bus_device
{
    uint32_t  timeout;
    uint32_t  retries;
    void *priv;
	const i2c_bus_ops_t * i2c_ops;
#ifdef USE_OS
	/* define OS safty needed */
#endif
};
typedef struct i2c_bus_device i2c_bus_device_t;

#ifdef BSP_USING_I2C1
extern i2c_bus_device_t i2c1_bus_device;
#endif

#ifdef BSP_USING_I2C2
extern i2c_bus_device_t i2c2_bus_device;
#endif

i2c_err_t i2c_xfer( i2c_bus_device_t *bus,
					i2c_msg_t msgs[],
					uint32_t num);

i2c_err_t i2c_bus_init(void);

#endif /* I2C_H_ */
