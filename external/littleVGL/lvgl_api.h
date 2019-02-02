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


void set_display_flush(void* disp_flush);
void set_display_fill(void* disp_fill);
void set_display_map(void* disp_map);

int lvgl_init(void);


#ifdef __cplusplus
}
#endif

#endif /*LVGL_API_H*/
