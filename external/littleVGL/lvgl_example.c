/*

 */
#include <stdio.h>
#include <lvgl_api.h>

#define TEST_BAISC_TEXT 0
#define TEST_THEME 1

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);
void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);

static lv_obj_t *tabview = NULL;

static lv_obj_t *chart = NULL;
static lv_obj_t *gauge = NULL;
static lv_chart_series_t *series = NULL;
static lv_res_t on_led_switch_toggled(lv_obj_t *sw)
{
    printf("[LVGL example] Hello");
    return LV_RES_OK;
}

static pthread_t ui_thread_id;
static void* ui_refresh_task(void *arg)
{
	int i = 1;
	while (1) {
		sleep(10);
		lv_tabview_set_tab_act(tabview, i, true);
		i++;
		i %= 3;
	}

    return NULL;
}

int lvgl_main(int argc, FAR char *argv[])
{
	//set_display_flush(my_disp_flush);
	//set_display_fill(my_disp_fill);
	//set_display_map(my_disp_map);
	if(lvgl_init() != 0) {
		printf("lvgl_init failed!\n");
		return -1;
	}

#if TEST_BAISC_TEXT
	lv_style_t style;
	lv_obj_t *screen = lv_scr_act();
	lv_obj_t *hello_world_label = lv_label_create(screen, NULL);
	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_style_copy(&style, &lv_style_btn_rel);
	style.text.color = LV_COLOR_YELLOW;
	lv_label_set_style(hello_world_label, &style);

	lv_obj_t *count_label = lv_label_create(screen, NULL);
	lv_obj_align(count_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
	char count_str[16] = {0};
	int count = 1;
	while(count < 20) {
		printf("[lvgl_example]lv_label_set_text to %d\n",count);
		sprintf(count_str, "%d", count);
		lv_label_set_text(count_label, count_str);
		count++;
		sleep(2);
	}
#endif

#if TEST_THEME
    lv_obj_t *scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    lv_theme_t *th = lv_theme_alien_init(100, NULL);
    lv_theme_set_current(th);

    tabview = lv_tabview_create(lv_scr_act(), NULL);
	lv_obj_align(tabview, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, SYMBOL_LOOP);
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, SYMBOL_HOME);
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, SYMBOL_SETTINGS);
    lv_tabview_set_tab_act(tabview, 0, true); //false
	//lv_tabview_set_anim_time(tabview, 100); //default config 300

    chart = lv_chart_create(tab2, NULL);
    lv_obj_set_size(chart, 300, 150);
    lv_chart_set_point_count(chart, 10); //20
    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, (lv_chart_type_t)(LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE));
    lv_chart_set_series_opa(chart, LV_OPA_70);
    lv_chart_set_series_width(chart, 4);
    lv_chart_set_range(chart, 0, 100);
    series = lv_chart_add_series(chart, LV_COLOR_RED);

    static lv_color_t needle_colors[] = {LV_COLOR_BLUE, LV_COLOR_YELLOW};
    gauge = lv_gauge_create(tab1, NULL);
    lv_gauge_set_needle_count(gauge,
                              sizeof(needle_colors) / sizeof(needle_colors[0]), needle_colors);
    lv_obj_align(gauge, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_gauge_set_value(gauge, 0, 50);

    char name[10];
    int i;
    lv_obj_t *labels[3];
    lv_obj_t *switches[3];
    for (i = 0; i < 3; i++)
    {
        labels[i] = lv_label_create(tab3, NULL);
        sprintf(name, "LED%d", i + 1);
        lv_label_set_text(labels[i], name);
    }
    lv_obj_align(labels[0], NULL, LV_ALIGN_IN_TOP_MID, -40, 20);
    for (i = 1; i < 3; i++)
    {
        lv_obj_align(labels[i], labels[i - 1], LV_ALIGN_OUT_BOTTOM_MID, 0, 35);
    }
    for (i = 0; i < 3; i++)
    {
        switches[i] = lv_sw_create(tab3, NULL);
        lv_obj_align(switches[i], labels[i], LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        lv_sw_set_action(switches[i], on_led_switch_toggled);
    }

	if (pthread_create(&ui_thread_id, NULL, ui_refresh_task, NULL) != 0) {
		printf("failed to create refresh task\n");
		return -1;
	}
	pthread_setname_np(ui_thread_id, "UI_animation");
	pthread_detach(ui_thread_id);

#endif

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

