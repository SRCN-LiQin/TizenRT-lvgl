/*

 */
#include <stdio.h>
#include <stdlib.h>
#include <lvgl_api.h>

#define TEST_BAISC_TEXT 0
#define TEST_THEME 1
#define TEST_LIST 1

void my_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);
void my_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void my_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);

#if TEST_THEME
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
		sleep(6);
		lv_tabview_set_tab_act(tabview, i, true);
		i++;
		i %= 3;
	}

    return NULL;
}
#endif

#if TEST_LIST
static lv_res_t roller_action(lv_obj_t * roller)
{
	lv_theme_t *th = lv_theme_alien_init(100, NULL);
	switch (lv_ddlist_get_selected(roller)) {
		case 0:
			th = lv_theme_default_init(100, NULL);
			break;

		case 1:
			th = lv_theme_alien_init(100, NULL);
			break;

		case 2:
			th = lv_theme_night_init(100, NULL);
			break;

		case 3:
			th = lv_theme_mono_init(100, NULL);
			break;

		case 4:
			th = lv_theme_material_init(100, NULL);
			break;

		case 5:
			th = lv_theme_zen_init(100, NULL);
			break;

		default:
			printf("[roller_action]unsupported\n");
			break;
	}
	lv_theme_set_current(th);
	return LV_RES_OK;
}

static lv_obj_t * ddlist = NULL;
static lv_obj_t * list = NULL;
static lv_obj_t * roller = NULL;
static pthread_t list_thread_id;
static void* list_refresh_task(void *arg)
{
	int i = 1;
	int up = 0;
	while (1) {
		sleep(3);
/* // for Drop Down list
		if (i%2) {
			lv_ddlist_close(ddlist, true);
		} else {
			lv_ddlist_open(ddlist, true);
			lv_ddlist_set_selected(ddlist, i%3);
		}
		i++;*/

	// for List and Roller animation
		i++;
		if (!up) {
			lv_list_up(list);
			if(i%7 == 0) up = 1;
		} else {
			lv_list_down(list);
			if (i%7 == 0) up = 0;
		}
		
		lv_roller_set_selected(roller, i % 7, true);
	}

    return NULL;
}

#endif

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
	while (count < 20) {
		printf("[lvgl_example]lv_label_set_text to %d\n",count);
		sprintf(count_str, "%d", count);
		lv_label_set_text(count_label, count_str);
		count++;
		sleep(2);
	}
#endif

#if TEST_THEME
if (atoi(argv[1]) == 1) {
	lv_obj_t *scr = lv_obj_create(NULL, NULL);
	lv_scr_load(scr);

	lv_theme_t *th = lv_theme_alien_init(100, NULL);
	lv_theme_set_current(th);

	tabview = lv_tabview_create(lv_scr_act(), NULL);  //ANIMATION EXAMPLE
	lv_obj_align(tabview, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

	lv_obj_t *tab1 = lv_tabview_add_tab(tabview, SYMBOL_LOOP);
	lv_obj_t *tab2 = lv_tabview_add_tab(tabview, SYMBOL_HOME);
	lv_obj_t *tab3 = lv_tabview_add_tab(tabview, SYMBOL_SETTINGS);
	lv_tabview_set_tab_act(tabview, 0, true); //false
	lv_tabview_set_anim_time(tabview, 0); //default config 300

	chart = lv_chart_create(tab2, NULL);
	lv_obj_set_size(chart, 300, 150);
	lv_chart_set_point_count(chart, 10); //20
	lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_chart_set_type(chart, (lv_chart_type_t)(LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE));
	lv_chart_set_series_opa(chart, LV_OPA_70);
	lv_chart_set_series_width(chart, 4);
	lv_chart_set_range(chart, 0, 100);
	series = lv_chart_add_series(chart, LV_COLOR_RED);
	lv_coord_t chartV[10] = {70, 44, 26, 10, 66, 40, 30, 56, 80, 60,};
	lv_chart_set_points(chart, series, chartV);

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
	for (i = 0; i < 3; i++) {
		labels[i] = lv_label_create(tab3, NULL);
		sprintf(name, "LED%d", i + 1);
		lv_label_set_text(labels[i], name);
	}
	lv_obj_align(labels[0], NULL, LV_ALIGN_IN_TOP_MID, -40, 20);
	for (i = 1; i < 3; i++) {
		lv_obj_align(labels[i], labels[i - 1], LV_ALIGN_OUT_BOTTOM_MID, 0, 35);
	}
	for (i = 0; i < 3; i++) {
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
}
#endif
#if TEST_LIST
if(atoi(argv[1]) == 2) {
	lv_obj_t *scr = lv_obj_create(NULL, NULL);
	lv_scr_load(scr);

	lv_theme_t *th = lv_theme_alien_init(100, NULL);
	lv_theme_set_current(th);

	lv_obj_t * parent = scr;//lv_page_create(scr, NULL);
	//lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY);
//printf("screen x1 %d, y1 %d, x2 %d, y2 %d\n", parent->coords.x1, parent->coords.y1, parent->coords.x2, parent->coords.y2);

	static lv_style_t h_style;
	lv_style_copy(&h_style, &lv_style_transp);
	h_style.body.padding.inner = LV_DPI / 4;
	h_style.body.padding.hor = LV_DPI / 4;
	h_style.body.padding.ver = LV_DPI / 6;

	lv_obj_t * h = lv_cont_create(parent, NULL);
	lv_obj_set_style(h, &h_style);
	lv_obj_set_click(h, false);
	lv_obj_set_size(h, 320,240);// set size OR set fit + layout
	//lv_cont_set_fit(h, true, true);
	//lv_cont_set_layout(h, LV_LAYOUT_COL_M); // Mid or Left

/*  // 3 buttons
	lv_obj_t * btn = lv_btn_create(h, NULL);
	lv_btn_set_style(btn, LV_BTN_STYLE_REL, th->btn.rel);
	lv_btn_set_style(btn, LV_BTN_STYLE_PR, th->btn.pr);
	lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, th->btn.tgl_rel);
	lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, th->btn.tgl_pr);
	lv_btn_set_style(btn, LV_BTN_STYLE_INA, th->btn.ina);
	lv_btn_set_fit(btn, true, true);
	lv_btn_set_toggle(btn, true);
	lv_obj_t * btn_label = lv_label_create(btn, NULL);
	lv_label_set_text(btn_label, "Button");

	btn = lv_btn_create(h, btn);
	lv_btn_toggle(btn);
	btn_label = lv_label_create(btn, NULL);
	lv_label_set_text(btn_label, "Toggled");

	btn = lv_btn_create(h, btn);
	lv_btn_set_state(btn, LV_BTN_STATE_INA);
	btn_label = lv_label_create(btn, NULL);
	lv_label_set_text(btn_label, "Inactive");
*/
/* //3 lables and 1 btnm
	lv_obj_t * label = lv_label_create(h, NULL);
	lv_label_set_text(label, "Primary");
	lv_obj_set_style(label, th->label.prim);

	label = lv_label_create(h, NULL);
	lv_label_set_text(label, "Secondary");
	lv_obj_set_style(label, th->label.sec);

	label = lv_label_create(h, NULL);
	lv_label_set_text(label, "Hint");
	lv_obj_set_style(label, th->label.hint);

	static const char * btnm_str[] = {"1", "2", "3", SYMBOL_OK, SYMBOL_CLOSE, ""};
	lv_obj_t * btnm = lv_btnm_create(h, NULL);
	lv_obj_set_size(btnm, LV_HOR_RES / 4, 2 * LV_DPI / 3);
	lv_btnm_set_map(btnm, btnm_str);
	lv_btnm_set_toggle(btnm, true, 3);
*/

/*   //2 switches, 1 progress bar, and 1 slider
	lv_obj_t * sw_h = lv_cont_create(h, NULL);
	lv_cont_set_style(sw_h, &lv_style_transp);
	lv_cont_set_fit(sw_h, false, true);
	lv_obj_set_width(sw_h, LV_HOR_RES / 4);
	lv_cont_set_layout(sw_h, LV_LAYOUT_PRETTY);
	lv_obj_t * sw = lv_sw_create(sw_h, NULL);

	sw = lv_sw_create(sw_h, sw);
	lv_sw_on(sw);

	lv_obj_t * bar = lv_bar_create(h, NULL);
	lv_bar_set_value(bar, 70);

	lv_obj_t * slider = lv_slider_create(h, NULL);
	lv_bar_set_value(slider, 70);
*/

/*   //1 Line, 1 Text area, 2 CheckBoxes, 1 Drop down list - NO FIT, NO LAYOUT, SET POS by each widget
	lv_obj_t * line = lv_line_create(h, NULL);
	static const lv_point_t line_p[] = {{0, 0}, {LV_HOR_RES / 5, 0}};
	lv_line_set_points(line, line_p, 2);
	lv_line_set_style(line, th->line.decor);
	lv_obj_set_pos(line, 10,20);

	lv_obj_t * ta = lv_ta_create(h, NULL);
	lv_obj_set_style(ta, th->ta.oneline);
	lv_ta_set_text(ta, "Some text");
	lv_ta_set_one_line(ta, true);
	lv_obj_set_pos(ta, 10,40);

	lv_obj_t * cb = lv_cb_create(h, NULL);
	lv_obj_set_pos(cb, 10,120);

	cb = lv_cb_create(h, cb);
	lv_btn_set_state(cb, LV_BTN_STATE_TGL_REL);
	lv_obj_set_pos(cb, 10,160);


	ddlist = lv_ddlist_create(h, NULL); //ANIMATION EXAMPLE
	lv_ddlist_set_anim_time(ddlist, 100);
	lv_ddlist_open(ddlist, true);
	lv_ddlist_set_selected(ddlist, 1);
	lv_obj_set_pos(ddlist, 190, 120);
	if (pthread_create(&ui_thread_id, NULL, ui_refresh_task, NULL) != 0) {
		printf("failed to create refresh task\n");
		return -1;
	}
	pthread_setname_np(ui_thread_id, "UI_animation");
*/

	h = lv_cont_create(parent, h);
	// 1 List with 10 items, 1 Scroller with 7 items
	list = lv_list_create(h, NULL);
	lv_obj_t * list_btn;
	list_btn = lv_list_add(list, SYMBOL_GPS,  "GPS",  NULL);
	lv_obj_set_size(list, 100/*LV_HOR_RES / 4*/, 200/*LV_VER_RES / 2*/);
	lv_obj_set_pos(list, 20, 20);
	lv_btn_set_toggle(list_btn, true);
	lv_list_add(list, SYMBOL_WIFI, "WiFi", NULL);
	lv_list_add(list, SYMBOL_AUDIO, "Audio", NULL);
	lv_list_add(list, SYMBOL_VIDEO, "Video", NULL);
	lv_list_add(list, SYMBOL_CALL, "Call", NULL);
	lv_list_add(list, SYMBOL_BELL, "Bell", NULL);
	lv_list_add(list, SYMBOL_FILE, "File", NULL);
	lv_list_add(list, SYMBOL_EDIT, "Edit", NULL);
	lv_list_add(list, SYMBOL_CUT,  "Cut",  NULL);
	lv_list_add(list, SYMBOL_COPY, "Copy", NULL);
	lv_list_set_anim_time(list, 0); //NO AMINIATION
	lv_list_set_sb_mode(list, LV_SB_MODE_ON);

	roller = lv_roller_create(h, NULL); // ==> GOOD SCROLLER effect
	lv_roller_set_options(roller, "Monday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\nSunday");
	lv_roller_set_selected(roller, 3, true);
	lv_roller_set_visible_row_count(roller, 5);
	lv_ddlist_set_action(roller, roller_action);
	lv_ddlist_set_anim_time(roller, 100);
	lv_obj_set_pos(roller, 140, 20);
	
	if (pthread_create(&list_thread_id, NULL, list_refresh_task, NULL) != 0) {
		printf("failed to create refresh task\n");
		return -1;
	}
	pthread_setname_np(list_thread_id, "UI_LIST");
}
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

