/*

 */
#include <stdio.h>
#include <lvgl_api.h>


void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);
void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);


int lvgl_main(int argc, FAR char *argv[])
{
	set_display_flush(my_disp_flush);
	set_display_fill(my_disp_fill);
	set_display_map(my_disp_map);

	if(lvgl_init() != 0) {
		printf("lvgl_init failed!\n");
		return -1;
	}

	lv_style_t style;
	lv_obj_t *screen = lv_scr_act();
	lv_obj_t *hello_world_label = lv_label_create(screen, NULL);
	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_style_copy(&style, &lv_style_plain);
	style.text.color = LV_COLOR_PURPLE;
	lv_label_set_style(hello_world_label, &style);

	lv_obj_t *count_label = lv_label_create(screen, NULL);
	lv_obj_align(count_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
	char count_str[16] = {0};
	int count = 1;
	while(1) {
		printf("lv_label_set_text to %d\n",count);
		sprintf(count_str, "%d", count);
		lv_label_set_text(count_label, count_str);
		count++;
		sleep(1);
	}

	return 0;
}


void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
	printf("my_disp_flush, x1 %d, y1 %d, x2 %d, y2 %d\n", x1, y1, x2, y2);
    int32_t x = 0;
    int32_t y = 0;

    for (y = y1; y <= y2; y++) {                        /*Pick the next row*/
        for (x = x1; x <= x2; x++) {                   /*Pick the a pixel in the row*/
            //ST7789H2_WritePixel(x, y, color_p->full);
            color_p++;
        }
    }

    lv_flush_ready();
}

void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
	printf("my_disp_fill, x1 %d, y1 %d, x2 %d, y2 %d\n", x1, y1, x2, y2);

    int32_t i = 0;
    int32_t j = 0;

    for (i = x1; i <= x2; i++) {
        for (j = y1; j <= y2; j++) {
            //ST7789H2_WritePixel(i, j, color.full);
        }
    }
}

void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
	printf("my_disp_map, x1 %d, y1 %d, x2 %d, y2 %d\n", x1, y1, x2, y2);

    int32_t i = 0;
    int32_t j = 0;

    for (i = x1; i <= x2; i++) {
        for (j = y1; j <= y2; j++) {
            //ST7789H2_WritePixel(i, j, color_p->full);
            color_p++;
        }
    }
}

