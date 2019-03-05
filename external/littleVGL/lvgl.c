/*

 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <tinyara/clock.h>

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
	usleep(50*100);
    while (1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
		//lv_tick_inc(10);
		lv_task_handler();
        //printf("littlevgl_refresh_task\n");
		//usleep(10*1000);
    }

    return NULL;
}

/////////////////////////////////////////////////
#if 0
typedef struct {
	int32_t x;
	int32_t y;
	int32_t x2;
	int32_t y2;
	lv_color_t* buff;
}disp_item;

disp_item disp_data;
sem_t flush_sem;

static void flushing_thread(void *data)
{
printf("flushing_thread\n");
	sem_init(&flush_sem, 0, 0);
	sem_setprotocol(&flush_sem, SEM_PRIO_NONE);

	while (1) {
		sem_wait(&flush_sem);

uint32_t rcv = lv_tick_get();
printf("fl go %d\n",rcv);
		
		tizenrt_disp_flush(disp_data.x, disp_data.y, disp_data.x2, disp_data.y2, (const lv_color_t *)disp_data.buff);

rcv = lv_tick_get();
printf("fl end %d\n",rcv);


	}
	return ;
}

void flush_disp(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
uint32_t rcv = lv_tick_get();
printf("flush %d\n",rcv);
	disp_data.x = x1;
	disp_data.y = y1;
	disp_data.x2 = x2;
	disp_data.y2 = y2;
	disp_data.buff = color_p;

	sem_post(&flush_sem);
}
#endif
#if 0
#define DISP_QUEUE_SIZE 5
typedef struct {
	int16_t x;
	int16_t y;
	int16_t x2;
	int16_t y2;
	lv_color_t* buff;
}disp_queue_item;

disp_queue_item disp_q[DISP_QUEUE_SIZE];
int tail = 0;
sem_t qfull_sem;
sem_t qempty_sem;

void createq()
{
	sem_init(&qfull_sem, 0, 0);
	sem_setprotocol(&qfull_sem, SEM_PRIO_NONE);

	sem_init(&qempty_sem, 0, 0);
	sem_setprotocol(&qempty_sem, SEM_PRIO_NONE);
}
void addq(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
uint32_t add = clock_systimer();

	int semc = 0;
	if (tail == DISP_QUEUE_SIZE) {
printf("w full\n");
		sem_wait(&qfull_sem);
	}

	disp_q[tail] = (disp_queue_item){(int16_t)x1, (int16_t)y1, (int16_t)x2, (int16_t)y2, NULL};
	int len = (x2 - x1 + 1) * (y2 - y1 + 1) * sizeof(lv_color_t);
	disp_q[tail].buff = (lv_color_t*)malloc(len);

	memcpy(disp_q[tail].buff, color_p, len);
	tail++;
	if (tail == 1 && sem_getvalue(&qempty_sem, &semc)== OK && semc < 0) { //add first item and other task is waiting qempty_sem
printf("p emp\n");
		sem_post(&qempty_sem);
	}

printf("add %u, end %d\n", add, clock_systimer());
	lv_flush_ready();
}

bool rcvq(disp_queue_item *out)
{
	int i;
	int semc = 0;
	
	if (tail == 0) {
		return false;
	}

	memcpy(out,&disp_q[0], sizeof(disp_queue_item));
	for (i = 0; i < tail - 1; i++) {
		memcpy(&disp_q[i],&disp_q[i + 1], sizeof(disp_queue_item));
	}

	if (tail == DISP_QUEUE_SIZE && sem_getvalue(&qfull_sem, &semc)== OK && semc < 0) { //get last item, and other task is waiting qfull_sem
printf("p full\n");
		sem_post(&qfull_sem);
	}

	tail--;

	return true;
}

static void* queue_recieve_thread(void *data)
{
	disp_queue_item rcvitem;

	while (1) {
		if (rcvq(&rcvitem)) {
uint32_t rcv = clock_systimer();

			tizenrt_disp_flush(rcvitem.x, rcvitem.y, rcvitem.x2, rcvitem.y2, (const lv_color_t *)rcvitem.buff);
			free(rcvitem.buff);
			rcvitem.buff = NULL;
printf("rcv %u, rend %d\n",rcv, clock_systimer());

		} else {
printf("q empty\n");
			sem_wait(&qempty_sem);
			//usleep(50*1000);
		}
	}

	return NULL;
}
#endif

#if 0
#include "esp32_queue_api.h"
#include <fcntl.h>

#define DISP_QUEUE_SIZE 10
typedef struct {
	int16_t x;
	int16_t y;
	int16_t x2;
	int16_t y2;
	lv_color_t* buff;
}disp_queue_item;

queue_info_t *queue_handle = NULL;

static mqd_t open_mqueue( void )
{
	//queue_handle = (queue_info_t *)queue_create_wrapper(DISP_QUEUE_SIZE, sizeof(disp_queue_item));
	struct mq_attr attr;
	attr.mq_maxmsg = DISP_QUEUE_SIZE;
	attr.mq_msgsize = sizeof(disp_queue_item);
	attr.mq_flags = 0;

	mqd_t q = mq_open("dispmqueue", O_RDWR | O_CREAT, 0666, &attr);
	return q;
}

static void disp_mqueue_send(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p)
{
printf("s %u\n",lv_tick_get());
	//mqd_t queue_handle = open_mqueue();

	disp_queue_item newitem = {(int16_t)x1, (int16_t)y1, (int16_t)x2, (int16_t)y2, NULL};
	int len = (x2 - x1 + 1) * (y2 - y1 + 1) * sizeof(lv_color_t);
	newitem.buff = (lv_color_t*)malloc(len);
	memcpy(newitem.buff, color_p, len);

	if(mq_send(queue_handle, &newitem, sizeof(disp_queue_item), 0) != OK) {
		printf("queue_send failed\n");
	}
	//mq_close(queue_handle);

	lv_flush_ready();
printf("so %u\n",lv_tick_get());
}

static void queue_recieve_thread(void *data)
{
	mqd_t rcv_handle = open_mqueue();

	while (1) {
		disp_queue_item rcvitem;
		int len = sizeof(disp_queue_item);
		if (mq_receive(rcv_handle, &rcvitem, len, 0) == len) {
printf("recieve %d\n",lv_tick_get());
			//lcd_obj->drawBitmap(rcvitem.x, rcvitem.y, (const uint16_t *)rcvitem.buff, rcvitem.w, rcvitem.h);
			tizenrt_disp_flush(rcvitem.x, rcvitem.y, rcvitem.x2, rcvitem.y2, (const lv_color_t *)rcvitem.buff);
			free(rcvitem.buff);
			rcvitem.buff = NULL;
printf("ro %d\n",lv_tick_get());

		} else {
			printf("queue_recv_wrapper failed!\n");
			//usleep(50 * 1000);
		}
	}

	mq_close(rcv_handle);
	return;
}
#endif

pthread_t queue_rcv_handle;

int lvgl_init(void)
{
	//device initialize
	lvgl_lcd_display_init();
	
	if (!tizenrt_disp_flush && !tizenrt_disp_fill) {
		printf("Need to set_display_flush before lvgl_init.\n");
		return -1;
	}

	
#if 0
	//createq();
	//queue_handle = open_mqueue();
	pthread_attr_t attr;
	struct sched_param sparam;
	pthread_attr_init(&attr);
	sparam.sched_priority = 101; //default 100
	//pthread_attr_setschedparam(&attr, &sparam);
	//pthread_attr_setschedpolicy(&attr, SCHED_RR);
	pthread_attr_setstacksize(&attr, 4096);
	//if(pthread_create(&queue_rcv_handle, &attr, flushing_thread, NULL) != 0) {
	int queue_rcv_handle;
	queue_rcv_handle = task_create("display_q_reciever", 125, 4096, flushing_thread, NULL);
	if (queue_rcv_handle < 0) {
		printf("failed to create queue receiver task\n");
		return -1;
	}
	//pthread_setname_np(queue_rcv_handle, "disp_q_rcv");

#endif

	lv_init();

//#ifdef CONFIG_LVGL_FILESYSTEM
	lvgl_fs_init();
//#endif

	//lv_refr_set_round_cb(get_round_func());

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.disp_flush = tizenrt_disp_flush;//flush_disp;//addq;//disp_mqueue_send;
	disp_drv.disp_fill = tizenrt_disp_fill;
	disp_drv.disp_map = tizenrt_disp_map;
	if (lv_disp_drv_register(&disp_drv) == NULL) {
		printf("Failed to register display device.");
		return -1;
	}

	if (pthread_create(&render_thread_id, NULL, littlevgl_refresh_task, NULL) != 0) {
	//int lvgl_render_handle = task_create("lvgl_renderer", 126, 4096, littlevgl_refresh_task, NULL);
	//if (lvgl_render_handle < 0) {
		printf("failed to create refresh task\n");
		return -1;
	}
	pthread_setname_np(render_thread_id, "lvgl_renderer");

	return 0;
}

