#ifndef _LVGL_LCD_CONFIG_H
#define _LVGL_LCD_CONFIG_H

/* lvgl include */
#define CONFIG_LVGL_LCD_MISO_GPIO 27
#define CONFIG_LVGL_LCD_MOSI_GPIO 21
#define CONFIG_LVGL_LCD_CLK_GPIO 22
#define CONFIG_LVGL_LCD_CS_GPIO 5
#define CONFIG_LVGL_LCD_DC_GPIO 19
#define CONFIG_LVGL_LCD_RESET_GPIO 18
#define CONFIG_LVGL_LCD_BL_GPIO 23
#define CONFIG_LVGL_LCD_SPI_CLOCK 20000000
#define CONFIG_LVGL_LCD_SPI_NUM 1

#define CONFIG_LVGL_DISP_ROTATE_90 1



#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initialize display
 */
void lvgl_lcd_display_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _LVGL_LCD_CONFIG_H */