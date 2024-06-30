#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf,size_t offset, size_t len);


int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);


size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}


size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB]     = {"/dev/fb", 0, 0, invalid_read, fb_write},
  				{"/dev/events", 0, 0, events_read, invalid_write},
  				{"/proc/dispinfo",0,0,dispinfo_read, invalid_write},
#include "files.h"
};

#define table_size (sizeof(file_table)/sizeof(Finfo))

int fs_open(const char *pathname, int flags, int mode){  //To obtain the fd of the targeted file.
	for(int i=0;i<table_size; i++){
		if(strcmp(file_table[i].name,pathname)==0){
			file_table[i].open_offset = 0;
			return i;
		}
	}
	assert(0);
}

size_t fs_read(int fd, void *buf, size_t len){
	Finfo *finfo = &file_table[fd];   //get the addr of file_table[fd].
	size_t ret=0;
	
	
	if(finfo->open_offset>finfo->size){
		assert(0);
		return ret;
	}
	
	if(finfo->read){
    	ret = finfo->read(buf, finfo->open_offset, len);
    	finfo->open_offset += ret;
	}else{
		if(finfo->open_offset + len > finfo->size){  //To judge whether the file is overflow.
			//printf("Read file overflow error\n");
			ret=ramdisk_read(buf,finfo->disk_offset + finfo->open_offset,finfo->size-finfo->open_offset);
			finfo->open_offset += ret;
		}else{
			ret = ramdisk_read(buf,finfo->disk_offset + finfo->open_offset, len);
			finfo->open_offset += ret;
		}
	}
	return ret;
}

int fs_close(int fd){
	Finfo *finfo = &file_table[fd];
	finfo->open_offset = 0;
	return 0;
}


size_t fs_write(int fd, const void *buf, size_t len){
	Finfo *finfo = &file_table[fd];
	size_t ret=0;
	
	//printf("fd: %d ,finfo->open_offset: %d, len: %d, finfo->size: %d",fd, finfo->open_offset,len,finfo->size);
	
	//assert(finfo->open_offset + len <= finfo->size);
	
	if(finfo->write){
		ret= finfo->write(buf,finfo->open_offset, len);
		finfo->open_offset += len;
	}else{
		//Note that we have already promise that the file will not be over-written.
		ret= ramdisk_write(buf, finfo->disk_offset+finfo->open_offset, len);
		finfo->open_offset+=len;
	}
	return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence){
	Finfo * finfo = &file_table[fd];
	
	switch(whence){
		case SEEK_SET:
			assert(offset<=finfo->size);
			finfo->open_offset = offset;
			break;
		case SEEK_CUR:
			assert(finfo->open_offset+offset<=finfo->size);
			finfo->open_offset += offset; 
			break;
		case SEEK_END:
			assert(offset<=finfo->size);
			finfo->open_offset = finfo->size + offset;
 			break;
		default: 
			printf("Invalid whence!\n");
			assert(0);
			break;
	}
	return finfo->open_offset;
}


void init_fs() {
  // TODO: initialize the size of /dev/fb
  
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  
  assert(w!=0 && h!=0);
  
  file_table[FD_FB].size = w * h * sizeof(uint32_t);
  
}
