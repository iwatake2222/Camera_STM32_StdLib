/*
 * oledSh1106Config.h
 *
 * Created: 2016-03-19 7:29:28 PM
 *  Author: take-iwiw
 */ 


#ifndef OLEDSH1106CONFIG_H_
#define OLEDSH1106CONFIG_H_

/*** config ***/
//#define USE_OUTPUT_DRAW_BUFFER

#define SLAVE_ADDR 0x78
#define I2C_SPEED_KHZ 400

#define I2C_TIMEOUT 1000

#define OLED_SH1106_WIDTH  128
#define OLED_SH1106_HEIGHT  64
#define OLED_SH1106_PAGE_SIZE 8
#define OLED_SH1106_PAGE    OLED_SH1106_HEIGHT/OLED_SH1106_PAGE_SIZE

#define DRAW_AREA_ORIGIN_X 0
#define DRAW_AREA_ORIGIN_Y 32
#define DRAW_AREA_WIDTH    OLED_SH1106_WIDTH
#define DRAW_AREA_HEIGHT   (OLED_SH1106_HEIGHT - DRAW_AREA_ORIGIN_Y)

/* text settings */
#define TEXT_AREA_ORIGIN_X 0
#define TEXT_AREA_ORIGIN_Y 0
// actual end position is the below number -1
#define TEXT_AREA_END_X 128
#define TEXT_AREA_END_Y 32
#define FONT_WIDTH 5
#define FONT_HEIGHT 8
#define FONT_DISPLAY_SIZE 1	// must be 2^x


/*** port map ***/
#define I2Cx                   I2C2
#define I2C_CLK                RCC_APB1Periph_I2C2
#define GPIO_SCL_CLK           RCC_AHB1Periph_GPIOB
#define GPIO_SDA_CLK           RCC_AHB1Periph_GPIOB
#define GPIO_SCL_PORT          GPIOB
#define GPIO_SCL_PIN           GPIO_Pin_10
#define GPIO_SCL_PIN_SOURCE    GPIO_PinSource10
#define GPIO_SCL_AF            GPIO_AF_I2C2
#define GPIO_SDA_PORT          GPIOB
#define GPIO_SDA_PIN           GPIO_Pin_11
#define GPIO_SDA_PIN_SOURCE    GPIO_PinSource11
#define GPIO_SDA_AF            GPIO_AF_I2C2


#endif /* OLEDSH1106CONFIG_H_ */
