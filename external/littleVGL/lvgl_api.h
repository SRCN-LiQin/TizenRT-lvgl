/**
 * @file lvgl_api.h
 * lvgl api for APP and driver
 */

#ifndef LVGL_API_H
#define LVGL_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/

/*Test misc. module version*/
#include "lvgl.h"

typedef void (*disp_flush_func)(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
				   const lv_color_t *color_p);

typedef void (*disp_fill_func)(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
				  lv_color_t color);
				 

void set_display_flush(disp_flush_func disp_flush);
void set_display_fill(disp_fill_func disp_fill);
void set_display_map(disp_flush_func disp_map);

int lvgl_init(void);


#ifdef __cplusplus
}
#endif

#endif /*LVGL_API_H*/
