#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_Off  Elf64_Off
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_Off  Elf32_Off 
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);



static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd=fs_open(filename,0,0);  //flags and mode are not used;
  
  Elf_Ehdr elf_header;
  
  fs_lseek(fd, 0, SEEK_SET);
  fs_read(fd,&elf_header,sizeof(elf_header));
  //ramdisk_read(&elf_header,0,sizeof(elf_header));
  
  assert(*(uintptr_t *)elf_header.e_ident == 0x464c457f); // To check the file being read is an ELF file.

  Elf_Off program_offset = elf_header.e_phoff;
  size_t program_entry_size=elf_header.e_phentsize;
  
  for(int i=0;i<elf_header.e_phnum;i++){
  	Elf_Phdr elf_phdr;
  	fs_lseek(fd,i*program_entry_size+program_offset,SEEK_SET);
  	fs_read(fd, &elf_phdr, sizeof(elf_phdr));
  	//ramdisk_read(&elf_phdr,i*program_entry_size+program_offset,sizeof(elf_phdr));
  	
  	void* vaddr_ptr;
  	switch(elf_phdr.p_type){
  		case PT_LOAD:{
  			vaddr_ptr=(void *)elf_phdr.p_vaddr;
  			fs_lseek(fd,elf_phdr.p_offset,SEEK_SET);
  			fs_read(fd,vaddr_ptr,elf_phdr.p_filesz);
  			//ramdisk_read(vaddr_ptr,elf_phdr.p_offset,elf_phdr.p_filesz);
  			memset(vaddr_ptr+elf_phdr.p_filesz,0,elf_phdr.p_memsz-elf_phdr.p_filesz);
  			break;
  		}
  		default: break;
  	}
  }
  return elf_header.e_entry;
}



void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = 0x%p", entry);
  ((void(*)())entry) ();
  assert(0);
}

