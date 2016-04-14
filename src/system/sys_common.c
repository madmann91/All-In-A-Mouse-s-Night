#include "sys_common.h"
#include "fs_private.h"

#ifdef WIN32 //Win32 wrapper
static HANDLE sys_heap;

int sys_init(void)
{
	sys_heap=GetProcessHeap();
	if(!sys_heap)
		return 0;
	return 1;
}

void sys_quit(void)
{
	//Do nothing
}

void* sys_alloc(unsigned int size)
{
	return HeapAlloc(sys_heap,0,size);
}

void* sys_realloc(void* ptr,unsigned int size)
{
	if(ptr)
		return HeapReAlloc(sys_heap,0,ptr,size);
	else
		return HeapAlloc(sys_heap,0,size);
}

void sys_free(void* ptr)
{
	HeapFree(sys_heap,0,ptr);
}

sys_file sys_open(const char* filename,int mode)
{
	static const DWORD access[]={GENERIC_READ,GENERIC_WRITE};
	static const DWORD creation[]={OPEN_EXISTING,OPEN_ALWAYS};
	
	return CreateFile(filename,access[mode],FILE_SHARE_READ,NULL,creation[mode],FILE_ATTRIBUTE_NORMAL,NULL);
}

unsigned long sys_read(void* buffer,unsigned int size,sys_file handle)
{
	DWORD read;
	if(!ReadFile(handle,buffer,size,&read,NULL))
		return 0;
	return (unsigned long)read;
}

unsigned long sys_write(const void* buffer,unsigned int size,sys_file handle)
{
	DWORD written;
	if(!WriteFile(handle,buffer,size,&written,NULL))
		return 0;
	return (unsigned long)written;
}

unsigned long sys_tell(sys_file handle)
{
	static const LARGE_INTEGER zero={0,0};
	LARGE_INTEGER pos;

	SetFilePointerEx(handle,zero,&pos,FILE_CURRENT);
	return (unsigned long)pos.LowPart;
}

int sys_seek(sys_file handle,long offset,int origin)
{
	static const DWORD method[]={FILE_BEGIN,FILE_CURRENT,FILE_END};
	return (SetFilePointer(handle,offset,NULL,method[origin])!=INVALID_SET_FILE_POINTER);
}

unsigned int sys_size(sys_file handle)
{
	return GetFileSize(handle,NULL);
}

int sys_close(sys_file handle)
{
	return (CloseHandle(handle)==TRUE);
}

int sys_isvalid(sys_file handle)
{
	return (handle!=INVALID_HANDLE_VALUE);
}

char** sys_listfiles(const char* path,const char* ext,unsigned int* count)
{
	char findstr[FS_MAX_PATH];
	char* list[FS_MAX_LIST];
	char** result;
	WIN32_FIND_DATA data;
	HANDLE handle;
	unsigned int i,j;

	sprintf(findstr,"%s*%s",path,ext);

	handle=FindFirstFile(findstr,&data);
	if(handle==INVALID_HANDLE_VALUE)
	{
		//Return an empty list
		*count=0;
		result=(char**)sys_alloc(sizeof(char*));
		*result=0;
		return result;
	}

	i=0;
	do
	{
		if(data.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY)
			continue;

		list[i]=(char*)sys_alloc(sizeof(char)*(strlen(data.cFileName)+1));
		strcpy(list[i],data.cFileName);
		i++;
	}while(FindNextFile(handle,&data));

	FindClose(handle);

	*count=i;
	result=(char**)sys_alloc(sizeof(char*)*(i+1));
	for(j=0;j<i;j++)
		result[j]=list[j];
	result[i]=0;
	return result;
}

char** sys_listpaths(const char* filter,unsigned int* count)
{
	char* list[FS_MAX_LIST];
	char** result;
	WIN32_FIND_DATA data;
	HANDLE handle;
	unsigned int i,j;

	handle=FindFirstFile(filter,&data);
	if(handle==INVALID_HANDLE_VALUE)
	{
		//Return an empty list
		*count=0;
		result=(char**)sys_alloc(sizeof(char*));
		*result=0;
		return result;
	}

	i=0;
	do
	{
		if(data.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY)
			continue;

		list[i]=sys_alloc(sizeof(char)*(strlen(data.cFileName)+1));
		strcpy(list[i],data.cFileName);
		i++;
	}while(FindNextFile(handle,&data));

	FindClose(handle);

	*count=i;
	result=(char**)sys_alloc(sizeof(char*)*(i+1));
	for(j=0;j<i;j++)
		result[j]=list[j];
	result[i]=0;
	return result;
}

void sys_freelist(char** list)
{
	unsigned int i;
	for(i=0;list[i];i++)
		sys_free(list[i]);
	sys_free(list);
}

#else //Default POSIX wrapper (CURRENTLY NOT TESTED)
static long tell(sys_file handle) {
	return lseek(handle,0,SEEK_CUR);
}

int sys_init(void)
{
	return 1;
}

void sys_quit(void)
{
	//Do nothing
}

void* sys_alloc(unsigned int size)
{
	return malloc(size);
}

void* sys_realloc(void* ptr,unsigned int size)
{
	if(ptr)
		return realloc(ptr,size);
	else
		return malloc(size);
}

void sys_free(void* ptr)
{
	free(ptr);
}

sys_file sys_open(const char* filename,int mode)
{
	int flags[]={O_RDONLY,O_WRONLY};
	return open(filename,flags[mode]);
}

unsigned long sys_read(void* buffer,unsigned int size,sys_file handle)
{
	long c=(long)read(handle,buffer,size);
	return (c>0)?c:0;
}

unsigned long sys_write(const void* buffer,unsigned int size,sys_file handle)
{
	long c=(long)write(handle,buffer,size);
	return (c>0)?c:0;
}

unsigned long sys_tell(sys_file handle)
{
	return (unsigned long)tell(handle);
}

int sys_seek(sys_file handle,long offset,int origin)
{
	static const int org[]={SEEK_SET,SEEK_CUR,SEEK_END};
	return (lseek(handle,offset,org[origin])!=-1L);
}

unsigned int sys_size(sys_file handle)
{
	long pos,size;
	pos=tell(handle);
	lseek(handle,0,SEEK_END);
	size=tell(handle);
	lseek(handle,pos,SEEK_SET);
	return size;
}

int sys_close(sys_file handle)
{
	return (close(handle)==0);
}

int sys_isvalid(sys_file handle)
{
	return (handle!=-1);
}

char** sys_listfiles(const char* path,const char* ext,unsigned int* count)
{
	char findstr[FS_MAX_PATH];
	char* list[FS_MAX_LIST];
	char** result;
	DIR* dir;
	struct dirent* entry;
	unsigned int i,j;
	int extlen;

	dir=opendir(path);
	if(!dir)
	{
		//Return an empty list
		*count=0;
		result=(char**)sys_alloc(sizeof(char*));
		*result=0;
		return result;
	}

	extlen=strlen(ext);

	i=0;
	while((entry=readdir(dir)))
	{
		int entlen=strlen(entry->d_name);
		if(entry->d_type==DT_DIR||entlen<extlen||strcmp(entry->d_name+entlen-extlen,ext))
			continue;

		list[i]=(char*)sys_alloc(sizeof(char)*(strlen(entry->d_name)+1));
		strcpy(list[i],entry->d_name);
		i++;
	}

	closedir(dir);

	*count=i;
	result=(char**)sys_alloc(sizeof(char*)*(i+1));
	for(j=0;j<i;j++)
		result[j]=list[j];
	result[i]=0;
	return result;
}

char** sys_listpaths(const char* filter,unsigned int* count)
{
	char* list[FS_MAX_LIST];
	char** result;
	DIR* dir;
	struct dirent* entry;
	unsigned int i,j;

	dir=opendir(filter);
	if(!dir)
	{
		//Return an empty list
		*count=0;
		result=(char**)sys_alloc(sizeof(char*));
		*result=0;
		return result;
	}

	i=0;
	while((entry=readdir(dir)))
	{
		if(entry->d_type!=DT_DIR)
			continue;

		list[i]=(char*)sys_alloc(sizeof(char)*(strlen(entry->d_name)+1));
		strcpy(list[i],entry->d_name);
		i++;
	}

	closedir(dir);

	*count=i;
	result=(char**)sys_alloc(sizeof(char*)*(i+1));
	for(j=0;j<i;j++)
		result[j]=list[j];
	result[i]=0;
	return result;
}

void sys_freelist(char** list)
{
	unsigned int i;
	for(i=0;list[i];i++)
		sys_free(list[i]);
	sys_free(list);
}
#endif
