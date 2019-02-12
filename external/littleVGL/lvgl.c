/*

 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <lvgl.h>
#include <lv_core/lv_refr.h>
#include "lvgl_fs.h"
#include "lvgl_api.h"
#include "lvgl_disp_config.h"

static pthread_t render_thread_id;

static disp_flush_func tizenrt_disp_flush = NULL;
static disp_fill_func tizenrt_disp_fill = NULL;
static disp_flush_func tizenrt_disp_map = NULL;

void set_display_flush(disp_flush_func disp_flush)
{
	tizenrt_disp_flush = disp_flush;
}

void set_display_fill(disp_fill_func disp_fill)
{
	tizenrt_disp_fill = disp_fill;
}

void set_display_map(disp_flush_func disp_map)
{
	tizenrt_disp_map = disp_map;
}

static void* littlevgl_refresh_task(void *arg)
{
    while (1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();
        //printf("littlevgl_refresh_task\n");
        usleep(10*1000);
        lv_tick_inc(10);
    }

    return NULL;
}

int lvgl_init(void)
{
	//device initialize
	lvgl_lcd_display_init();
	
	if (!tizenrt_disp_flush && !tizenrt_disp_fill) {
		printf("Need to set_display_flush before lvgl_init.\n");
		return -1;
	}

	lv_init();

//#ifdef CONFIG_LVGL_FILESYSTEM
	lvgl_fs_init();
//#endif

	//lv_refr_set_round_cb(get_round_func());

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.disp_flush = tizenrt_disp_flush;
	disp_drv.disp_fill = tizenrt_disp_fill;
	disp_drv.disp_map = tizenrt_disp_map;
	if (lv_disp_drv_register(&disp_drv) == NULL) {
		printf("Failed to register display device.");
		return -1;
	}

	if (pthread_create(&render_thread_id, NULL, littlevgl_refresh_task, NULL) != 0) {
		printf("failed to create refresh task\n");
		return -1;
	}
	pthread_setname_np(render_thread_id, "lvgl_renderer");

	return 0;
}

