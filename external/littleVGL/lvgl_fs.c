/*
 
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <lvgl.h>

/* Stub for LVGL ufs (ram based FS) init function
 * as we use Zephyr FS API instead
 */
void lv_ufs_init(void)
{
}

bool lvgl_fs_ready(void)
{
	return true;
}

static lv_fs_res_t errno_to_lv_fs_res(int err)
{
	switch (err) {
	case 0:
		return LV_FS_RES_OK;
	case -EIO:
		/*Low level hardware error*/
		return LV_FS_RES_HW_ERR;
	case -EBADF:
		/*Error in the file system structure */
		return LV_FS_RES_FS_ERR;
	case -ENOENT:
		/*Driver, file or directory is not exists*/
		return LV_FS_RES_NOT_EX;
	case -EFBIG:
		/*Disk full*/
		return LV_FS_RES_FULL;
	case -EACCES:
		/*Access denied. Check 'fs_open' modes and write protect*/
		return LV_FS_RES_DENIED;
	case -EBUSY:
		/*The file system now can't handle it, try later*/
		return LV_FS_RES_BUSY;
	case -ENOMEM:
		/*Not enough memory for an internal operation*/
		return LV_FS_RES_OUT_OF_MEM;
	case -EINVAL:
		/*Invalid parameter among arguments*/
		return LV_FS_RES_INV_PARAM;
	default:
		return LV_FS_RES_UNKNOWN;
	}
}

static lv_fs_res_t lvgl_fs_open(void *file, const char *path,
		lv_fs_mode_t mode)
{
	int err = 0;

	int oflag = 0;
	switch(mode) {
		case LV_FS_MODE_WR:
			oflag = O_RDWR;
			break;
		case LV_FS_MODE_RD:
			oflag = O_RDONLY;
			break;
		default:
			return LV_FS_RES_INV_PARAM;
	}

	int *pfd = (int*)file;
	*pfd = open(path, oflag);
	if(*pfd < 0)
	{
		err = get_errno();
	}
	
	return errno_to_lv_fs_res(err);
}

static lv_fs_res_t lvgl_fs_close(void *file)
{
	int *pfd = (int*)file;

	close(*pfd);
	return LV_FS_RES_OK;
}

static lv_fs_res_t lvgl_fs_remove(const char *path)
{
	int err = 0;

	if (unlink(path) != OK)
	{
		err = get_errno();
	}
	return errno_to_lv_fs_res(err);
}

static lv_fs_res_t lvgl_fs_read(void *file, void *buf, uint32_t btr, uint32_t *br)
{
	int result;

	int *pfd = (int*)file;
	result = read(*pfd, buf, btr);
	if (result > 0) {
		if (br != NULL) {
			*br = result;
		}
		result = 0;
	} else {
		if (br != NULL) {
			*br = 0;
		}
		result = get_errno();
	}
	return errno_to_lv_fs_res(result);
}

static lv_fs_res_t lvgl_fs_write(void *file, const void *buf, uint32_t btw, uint32_t *bw)
{
	int result;

	int *pfd = (int*)file;
	result = write(*pfd, buf, btw);
	if (result == btw) {
		if (bw != NULL) {
			*bw = btw;
		}
		result = 0;
	} else if (result < 0) {
		if (bw != NULL) {
			*bw = 0;
		}
		result = get_errno();
	} else {
		if (bw != NULL) {
			*bw = result;
		}
		result = -EFBIG;
	}
	return errno_to_lv_fs_res(result);
}

static lv_fs_res_t lvgl_fs_seek(void *file, uint32_t pos)
{
	int err = 0;
	
	int *pfd = (int*)file;
	if (lseek(*pfd, pos, SEEK_SET) == -1) {
		err = get_errno();
	}
	return errno_to_lv_fs_res(err);
}

static lv_fs_res_t lvgl_fs_tell(void *file, uint32_t *pos_p)
{
	int err = 0;
	int *pfd = (int*)file;

	//we use POSIX fs API here, no std io buff, just get current fs pos
//	FILE *f = fdopen(*pfd, 'r');
//	long len = (uint32_t*)ftell(f);
	long len = lseek(*pfd, 0, SEEK_CUR);
	if (len != -1) {
		*pos_p = (uint32_t)len;
		return LV_FS_RES_OK;
	}
	else {
		err = get_errno();
	}
	
	return errno_to_lv_fs_res(err);;
}

static lv_fs_res_t lvgl_fs_trunc(void *file)
{
	/*Not used in lvgl*/
	return LV_FS_RES_NOT_IMP;
}

static lv_fs_res_t lvgl_fs_size(void *file, uint32_t *fsize)
{
	int err = 0;
	off_t org_pos;

	/* LVGL does not provided path but pointer to file struct as such
	 * we can not use fs_stat, instead use a combination of fs_tell and
	 * fs_seek to get the files size.
	 */
	int *pfd = (int*)file;

	org_pos = lseek(*pfd, 0, SEEK_CUR);

	err = lseek(*pfd, 0, SEEK_END);
	if (err == -1) {
		*fsize = 0;
		err = get_errno();
		return errno_to_lv_fs_res(err);
	}

	*fsize = err + 1;

	lseek(*pfd, org_pos, SEEK_SET);

	return LV_FS_RES_OK;
}

static lv_fs_res_t lvgl_fs_rename(const char *from, const char *to)
{
	int err;

	err = rename(from, to);
	return errno_to_lv_fs_res(err);
}

static lv_fs_res_t lvgl_fs_free(uint32_t *total_p, uint32_t *free_p)
{
	/* We have no easy way of telling the total file system size.
	 * Zephyr can only return this information per mount point.
	 */
	return LV_FS_RES_NOT_IMP;
}

static lv_fs_res_t lvgl_fs_dir_open(void *dir, const char *path)
{
	/*Currently not used in lvgl*/
	return LV_FS_RES_NOT_IMP;
}

static lv_fs_res_t lvgl_fs_dir_read(void *dir, char *fn)
{
	/* LVGL expects a string as return parameter but the format of the
	 * string is not documented.
	 */
	return LV_FS_RES_NOT_IMP;
}

static lv_fs_res_t lvgl_fs_dir_close(void *dir)
{
	/*Currently not used in lvgl*/
	return LV_FS_RES_NOT_IMP;
}


void lvgl_fs_init(void)
{
	lv_fs_drv_t fs_drv;

	memset(&fs_drv, 0, sizeof(lv_fs_drv_t));

	fs_drv.file_size = sizeof(int);  //sizeof of file struct, ony save fd here
	fs_drv.rddir_size = sizeof(void*);
	fs_drv.letter = '/';
	fs_drv.ready = lvgl_fs_ready;

	fs_drv.open = lvgl_fs_open;
	fs_drv.close = lvgl_fs_close;
	fs_drv.remove = lvgl_fs_remove;
	fs_drv.read = lvgl_fs_read;
	fs_drv.write = lvgl_fs_write;
	fs_drv.seek = lvgl_fs_seek;
	fs_drv.tell = lvgl_fs_tell;
	fs_drv.trunc = lvgl_fs_trunc;
	fs_drv.size = lvgl_fs_size;
	//fs_drv.rename = lvgl_fs_rename;
	fs_drv.free = lvgl_fs_free;

	fs_drv.dir_open = lvgl_fs_dir_open;
	fs_drv.dir_read = lvgl_fs_dir_read;
	fs_drv.dir_close = lvgl_fs_dir_close;

	lv_fs_add_drv(&fs_drv);
}

