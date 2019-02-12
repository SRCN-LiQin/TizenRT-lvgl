// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* C Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* RTOS Includes */
#include <tinyara/config.h>
#include "lvgl_api.h"
#include "lvgl_disp_config.h"

/* SPI Includes */
#include "ILI9341.h"
#include "iot_lcd.h"
#include "lcd_adapter.h"



class CEspLcdAdapter : public CEspLcd
{
public:
    const uint16_t *pFrameBuffer = NULL;
    CEspLcdAdapter(lcd_conf_t *lcd_conf, int height = LCD_TFTHEIGHT, int width = LCD_TFTWIDTH, bool dma_en = true, int dma_word_size = 1024, int dma_chan = 1) : CEspLcd(lcd_conf, height, width, dma_en, dma_word_size, dma_chan)
    {
        /* Code here*/
    }
    void inline writeCmd(uint8_t cmd)
    {
        transmitCmd(cmd);
    }
    void inline writeData(uint16_t data)
    {
        transmitData(data);
    }
    void inline writeData(uint8_t data)
    {
        transmitData(data);
    }
    void inline writeData(uint16_t data, int point_num)
    {
        transmitData(data, point_num);
    }
    void inline writeCmdData(uint8_t cmd, uint32_t data)
    {
        transmitCmdData(cmd, data);
    }
    void inline writeData(uint8_t *data, uint16_t length)
    {
        transmitData(data, length);
    }
    void inline readData()
    {
        /* Code here*/
    }
};

static CEspLcdAdapter *lcd_obj = NULL;

#if 0
#if CONFIG_UGFX_DRIVER_AUTO_FLUSH_ENABLE
SemaphoreHandle_t flush_sem = NULL;
static uint16_t flush_width;
static uint16_t flush_height;

void board_lcd_flush_task(void *arg)
{
    portBASE_TYPE res;
    while (1) {
        res = xSemaphoreTake(flush_sem, portMAX_DELAY);
        if (res == pdTRUE) {
            lcd_obj->drawBitmap(0, 0, (const uint16_t *)lcd_obj->pFrameBuffer, flush_width, flush_height);
            vTaskDelay(CONFIG_UGFX_DRIVER_AUTO_FLUSH_INTERVAL / portTICK_RATE_MS);
        }
    }
}
#endif /* CONFIG_UGFX_DRIVER_AUTO_FLUSH_ENABLE */

#ifdef CONFIG_UGFX_GUI_ENABLE

void board_lcd_init()
{
    /*Initialize LCD*/
    lcd_conf_t lcd_pins = {
        .lcd_model = LCD_MOD_AUTO_DET,
        .pin_num_miso = CONFIG_UGFX_LCD_MISO_GPIO,
        .pin_num_mosi = CONFIG_UGFX_LCD_MOSI_GPIO,
        .pin_num_clk = CONFIG_UGFX_LCD_CLK_GPIO,
        .pin_num_cs = CONFIG_UGFX_LCD_CS_GPIO,
        .pin_num_dc = CONFIG_UGFX_LCD_DC_GPIO,
        .pin_num_rst = CONFIG_UGFX_LCD_RESET_GPIO,
        .pin_num_bckl = CONFIG_UGFX_LCD_BL_GPIO,
        .clk_freq = CONFIG_UGFX_LCD_SPI_CLOCK,
        .rst_active_level = 0,
        .bckl_active_level = CONFIG_UGFX_BCKL_ACTIVE_LEVEL,
        .spi_host = (spi_host_device_t)CONFIG_UGFX_LCD_SPI_NUM,
        .init_spi_bus = true,
    };

    if (lcd_obj == NULL) {
        lcd_obj = new CEspLcdAdapter(&lcd_pins);
    }
    lcd_obj->writeCmdData(ILI9341_MEMACCESS_REG, 0x80 | 0x08); // as default rotate

#if CONFIG_UGFX_DRIVER_AUTO_FLUSH_ENABLE
    // For framebuffer mode and flush
    if (flush_sem == NULL) {
        flush_sem = xSemaphoreCreateBinary();
    }
    xTaskCreate(board_lcd_flush_task, "flush_task", 1500, NULL, 5, NULL);
#endif
}

#endif

#endif //#if 0

void board_lcd_flush(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h)
{
#if CONFIG_UGFX_DRIVER_AUTO_FLUSH_ENABLE
    flush_width = w;
    flush_height = h;
    lcd_obj->pFrameBuffer = bitmap;
    xSemaphoreGive(flush_sem);
#else
    lcd_obj->drawBitmap(x, y, bitmap, w, h);
#endif
}

void board_lcd_blit_area(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h)
{
    lcd_obj->drawBitmap(x, y, bitmap, w, h);
}

void board_lcd_write_cmd(uint8_t cmd)
{
    lcd_obj->writeCmd(cmd);
}

void board_lcd_write_data(uint16_t data)
{
    lcd_obj->writeData(data);
}

void board_lcd_write_data_byte(uint8_t data)
{
    lcd_obj->writeData(data);
}

void board_lcd_write_data_byte_repeat(uint16_t data, int point_num)
{
    lcd_obj->writeData(data, point_num);
}

void board_lcd_write_cmddata(uint8_t cmd, uint32_t data)
{
    lcd_obj->writeCmdData(cmd, data);
}

void board_lcd_write_datas(uint8_t *data, uint16_t length)
{
    /* Code here*/
}

void board_lcd_set_backlight(uint16_t data)
{
    /* Code here*/
}

void board_lcd_set_orientation(uint16_t orientation)
{
    switch (orientation) {
    case 0:
        board_lcd_write_cmd(ILI9341_MEMACCESS_REG);
        board_lcd_write_data_byte(0x80 | 0x08);
        break;
    case 90:
        board_lcd_write_cmd(ILI9341_MEMACCESS_REG);
        board_lcd_write_data_byte(0x20 | 0x08);
        break;
    case 180:
        board_lcd_write_cmd(ILI9341_MEMACCESS_REG);
        board_lcd_write_data_byte(0x40 | 0x08);
        break;
    case 270:
        board_lcd_write_cmd(ILI9341_MEMACCESS_REG);
        board_lcd_write_data_byte(0xE0 | 0x08);
        break;
    default:
        break;
    }
}

#ifdef CONFIG_GRAPHIC_LVGL

/* lvgl include */
#include "lvgl_disp_config.h"

/*Write the internal buffer (VDB) to the display. 'lv_flush_ready()' has to be called when finished*/
void ex_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    lcd_obj->drawBitmap((int16_t)x1, (int16_t)y1, (const uint16_t *)color_p, (int16_t)(x2 - x1 + 1), (int16_t)(y2 - y1 + 1));
    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_flush_ready();
}

/*Fill an area with a color on the display*/
void ex_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    lcd_obj->fillRect((int16_t)x1, (int16_t)y1, (int16_t)(x2 - x1 + 1), (int16_t)(y2 - y1 + 1), (uint16_t)color.full);
}

/*Write pixel map (e.g. image) to the display*/
void ex_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
    lcd_obj->drawBitmap((int16_t)x1, (int16_t)y1, (const uint16_t *)color_p, (int16_t)(x2 - x1 + 1), (int16_t)(y2 - y1 + 1));
}

void lvgl_lcd_display_init()
{
    /*Initialize LCD*/
    lcd_conf_t lcd_pins = {
        .lcd_model = LCD_MOD_ILI9341,
        .pin_num_miso = CONFIG_LVGL_LCD_MISO_GPIO, //repalced by CONFIG_GPIO_SPI2_MISO_PIN
        .pin_num_mosi = CONFIG_LVGL_LCD_MOSI_GPIO, //replaced by CONFIG_GPIO_SPI2_MOSI_PIN
        .pin_num_clk = CONFIG_LVGL_LCD_CLK_GPIO, //repalced by CONFIG_GPIO_SPI2_CLK_PIN
        .pin_num_cs = CONFIG_LVGL_LCD_CS_GPIO, 
        .pin_num_dc = CONFIG_LVGL_LCD_DC_GPIO,
        .pin_num_rst = CONFIG_LVGL_LCD_RESET_GPIO,
        .pin_num_bckl = CONFIG_LVGL_LCD_BL_GPIO,
        .clk_freq = CONFIG_LVGL_LCD_SPI_CLOCK,
        .rst_active_level = 0,
        .bckl_active_level = 0,//CONFIG_LVGL_BCKL_ACTIVE_LEVEL, seems 0 or 1
        .spi_host = 2, //(spi_host_device_t)CONFIG_LVGL_LCD_SPI_NUM, means HSPI, replaced by spi port para ->ESP32  HSPI_PORT
        .init_spi_bus = true,
    };

    if (lcd_obj == NULL) {
#ifdef CONFIG_SPI2_DMA_CHANNEL
        lcd_obj = new CEspLcdAdapter(&lcd_pins, LV_VER_RES, LV_HOR_RES, true, CONFIG_SPI_DMA_MAX_DATALEN, CONFIG_SPI2_DMA_CHANNEL);
#else
		lcd_obj = new CEspLcdAdapter(&lcd_pins, LV_VER_RES, LV_HOR_RES);
#endif
    }

#ifdef CONFIG_LVGL_DISP_ROTATE_0
    board_lcd_write_cmd(ILI9341_MEMACCESS_REG);
    board_lcd_write_data_byte(0x80 | 0x08);
    printf("lvgl_example", "CONFIG_LVGL_DISP_ROTATE_0");
#elif defined(CONFIG_LVGL_DISP_ROTATE_90)
    board_lcd_write_cmd(ILI9341_MEMACCESS_REG);
    board_lcd_write_data_byte(0x20 | 0x08);
    printf("lvgl_example", "CONFIG_LVGL_DISP_ROTATE_90");
#elif defined(CONFIG_LVGL_DISP_ROTATE_180)
    board_lcd_write_cmd(ILI9341_MEMACCESS_REG);
    board_lcd_write_data_byte(0x40 | 0x08);
    printf("lvgl_example", "CONFIG_LVGL_DISP_ROTATE_180");
#elif defined(CONFIG_LVGL_DISP_ROTATE_270)
    board_lcd_write_cmd(ILI9341_MEMACCESS_REG);
    board_lcd_write_data_byte(0xE0 | 0x08);
    printf("lvgl_example", "CONFIG_LVGL_DISP_ROTATE_270");
#endif

    /* Set up the functions to access to your display */
    if (LV_VDB_SIZE != 0) {
    	set_display_flush(ex_disp_flush);
        //disp_drv.disp_flush = ex_disp_flush; /*Used in buffered mode (LV_VDB_SIZE != 0  in lv_conf.h)*/
    } else if (LV_VDB_SIZE == 0) {
    	set_display_fill(ex_disp_fill);
    	set_display_map(ex_disp_map);
        //disp_drv.disp_fill = ex_disp_fill; /*Used in unbuffered mode (LV_VDB_SIZE == 0  in lv_conf.h)*/
        //disp_drv.disp_map = ex_disp_map;   /*Used in unbuffered mode (LV_VDB_SIZE == 0  in lv_conf.h)*/
    }

    return;
}

#endif /* CONFIG_LVGL_GUI_ENABLE */
