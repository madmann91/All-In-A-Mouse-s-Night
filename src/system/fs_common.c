#include "config.h"
#include "fs_common.h"
#include "fs_private.h"

fs_handle_t fs_open(const char* name,int mode)
{
	if(fs_numhandles>=FS_MAX_HANDLES)
		return FS_INVALID_HANDLE;

	if(mode==FS_OPEN_READ)
	{
		//Search directories
		fs_searchpath_t* search;
		for(search=fs_paths;search;search=search->next)
		{
			if(search->pack)
			{
				//This is a zip file
				fs_package_t* pack=search->pack;
				unsigned int hash=fs_hashfilename(name,pack->hashsize);
				fs_packfile_t* file;

				for(file=pack->hash[hash];file;file=file->next)
				{
					if(!fs_comparepaths(file->name,name))
					{
						//We found it
						fs_ziplocalheader_t local;
						fs_zipstream_t* zfile=fs_zipopenstream(pack->name,file->offset);
						fs_file_t* handle=&fs_handles[fs_numhandles];

						if(!zfile)
							continue;

						fs_zipreadlocal(pack->fp,file->offset,&local);
						
						handle->fp=0;
						handle->zip=zfile;
						handle->size=local.size;
						fs_numhandles++;
						return (fs_numhandles-1);
					}
				}
			}
			else
			{
				//This is a normal directory
				fs_directory_t* dir=search->dir;
				fs_file_t* handle=&fs_handles[fs_numhandles];
				sys_file fp;

				fp=sys_open(fs_makepath(dir->name,dir->dirname,name),FS_OPEN_READ);
				if(!sys_isvalid(fp))
					continue;

				//We found it
				handle->fp=fp;
				handle->zip=0;
				sys_seek(fp,0,FS_SEEK_END);
				handle->size=sys_tell(fp);
				sys_seek(fp,0,FS_SEEK_SET);
				fs_numhandles++;
				return (fs_numhandles-1);
			}
		}
		//Not found...
		return FS_INVALID_HANDLE;
	}
	else if(mode==FS_OPEN_WRITE)
	{
		//We do not need to search pack files, since writting is no supported
		fs_searchpath_t* search;
		for(search=fs_paths;search;search=search->next)
		{
			if(search->dir)
			{
				fs_directory_t* dir=search->dir;
				fs_file_t* handle=&fs_handles[fs_numhandles];
				sys_file fp;

				fp=sys_open(fs_makepath(dir->dirname,dir->name,name),FS_OPEN_WRITE);
				if(!sys_isvalid(fp))
					continue;

				//We found it
				handle->fp=fp;
				handle->zip=0;
				handle->size=sys_size(fp);
				fs_numhandles++;
				return (fs_numhandles-1);
			}
		}
	}

	//Invalid modes go here
	return FS_INVALID_HANDLE;
}

fs_size_t fs_size(fs_handle_t handle)
{
	return fs_handles[handle].size;
}

fs_size_t fs_read(void* buffer,fs_size_t size,fs_handle_t handle)
{
	fs_file_t* file=&fs_handles[handle];

	if(file->zip)
		return fs_zipreadstream(buffer,size,file->zip);
	else //We assume this is a file
		return sys_read(buffer,size,file->fp);

	//Never reached
	return 0;
}

fs_size_t fs_write(const void* buffer,fs_size_t size,fs_handle_t handle)
{
	fs_file_t* file=&fs_handles[handle];

	if(file->fp)
		return sys_write(buffer,size,file->fp);

	//Does not work on zip files 
	return 0;
}

int fs_seek(fs_handle_t handle,long offset,int origin)
{
	fs_file_t* file=&fs_handles[handle];

	if(file->zip)
		return fs_zipseekstream(file->zip,offset,origin);
	else //We assume this is a file
		return sys_seek(file->fp,offset,origin);

	//Never reached
	return 0;
}

unsigned long fs_tell(fs_handle_t handle)
{
	fs_file_t* file=&fs_handles[handle];

	if(file->zip)
		return fs_ziptellstream(file->zip);
	else //We assume this is a file
		return sys_tell(file->fp);

	//Never reached
	return 0;
}

int fs_close(fs_handle_t handle)
{
	fs_file_t* file=&fs_handles[handle];
	int ret;

	if(file->zip)
		ret=fs_zipclosestream(file->zip);
	else //We assume this is a file
		ret=sys_close(file->fp);

	fs_numhandles--;
	if(handle<fs_numhandles)
		fs_handles[handle]=fs_handles[fs_numhandles];

	//Never reached
	return ret;
}

const char* fs_pathtofile(const char* path,const char* file)
{
	static char paths[FS_MAKE_PATH][FS_MAX_PATH];
	static unsigned int index;

	index=(index+1)%FS_MAKE_PATH;
	sprintf(paths[index],"%s%c%s",path,FS_SEPARATOR,file);
	return paths[index];
}

static fs_package_t* fs_registerzip(const char* filename)
{
	fs_zipfileheader_t header;
	fs_zipcentraldirend_t end;
	unsigned long offset;
	fs_package_t* pack;
	unsigned int i,j;
	sys_file fp;

	fp=sys_open(filename,FS_OPEN_READ);
	if(!sys_isvalid(fp))
		return 0;

	if(!fs_zipcheck(fp))
	{
		sys_close(fp);
		return 0;
	}

	if(!fs_ziplocatedir(fp,&end))
	{
		sys_close(fp);
		return 0;
	}

	//Count the real number of entries (some entries only contain a directory name)
	offset=sys_tell(fp);
	for(i=0,j=0;i<end.numentries;i++)
	{
		if(!fs_zipreadheader(fp,&header,0,0))
		{
			sys_close(fp);
			return 0;
		}
		if(header.size>0&&header.csize>0)
			j++;
	}
	sys_seek(fp,offset,FS_SEEK_SET);

	pack=(fs_package_t*)sys_alloc(sizeof(fs_package_t));
	pack->numfiles=j;
	strcpy(pack->name,filename);

	//Determine hash table size
	pack->hashsize=1;
	while(pack->hashsize<pack->numfiles&&pack->hashsize<FS_MAX_HASH)
		pack->hashsize<<=1;

	pack->files=(fs_packfile_t*)sys_alloc(sizeof(fs_packfile_t)*pack->numfiles);
	pack->hash=(fs_packfile_t**)sys_alloc(sizeof(fs_packfile_t*)*pack->hashsize);
	memset(pack->hash,0,sizeof(fs_packfile_t*)*pack->hashsize);

	//Add every file in the zip archive
	i=0;
	while(i<pack->numfiles)
	{
		fs_packfile_t* pfile=&pack->files[i];
		unsigned int hash;

		fs_zipreadheader(fp,&header,pfile->name,FS_MAX_PATH);
		if(header.size>0&&header.csize>0)
		{
			//Change separators... (zip uses '/' which might be different from FS_SEPARATOR)
			fs_setseparators(pfile->name);
			pfile->offset=header.offset;
			
			hash=fs_hashfilename(pfile->name,pack->hashsize);
			pfile->next=pack->hash[hash];
			pack->hash[hash]=pfile;

			i++;
		}		
	}

	pack->fp=fp;
	return pack;
}

static int fs_sortpaths(const void* a,const void* b)
{
	char* path1=*(char**)a,*path2=*(char**)b;
	return fs_comparepaths(path1,path2);
}

int fs_registerpath(const char* path,const char* dir)
{
	fs_searchpath_t* search=fs_paths;
	char** list,**ptr;
	unsigned int count;

	//Do not add this path if we already have registered it
	while(search)
	{
		if(search->dir)
		{
			if(!strcmp(search->dir->name,path)&&!strcmp(search->dir->dirname,dir))
				return 0;
		}
		search=search->next;
	}

	//Add this directory
	search=(fs_searchpath_t*)sys_alloc(sizeof(fs_searchpath_t));
	search->pack=0;
	search->dir=(fs_directory_t*)sys_alloc(sizeof(fs_directory_t));
	strcpy(search->dir->name,path);
	strcpy(search->dir->dirname,dir);
	search->next=fs_paths;
	fs_paths=search;

	//And the associated zip files
	list=sys_listfiles(fs_makepath(path,dir,""),FS_PACK_EXT,&count);

	//Sort them so that new packages overwrite old ones
	qsort(list,count,sizeof(char*),fs_sortpaths);

	//Add packages
	for(ptr=list;*ptr;ptr++)
	{
		fs_package_t* pack=fs_registerzip(fs_makepath(path,dir,*ptr));
		if(!pack)
			continue;

		search=(fs_searchpath_t*)sys_alloc(sizeof(fs_searchpath_t));
		search->dir=0;
		search->pack=pack;
		search->next=fs_paths;
		fs_paths=search;
	}
	sys_freelist(list);

	return 1;
}

int fs_init(void)
{
	fs_paths=0;
	if(!fs_registerpath(FS_BASE_PATH,"base"))
		return 0;
	return 1;
}

void fs_quit(void)
{
	unsigned int i;

	//Loop through all search paths and destroy associated memory
	fs_searchpath_t* search=fs_paths;
	while(search)
	{
		fs_searchpath_t* next=search->next;
		if(search->pack)
		{
			sys_close(search->pack->fp);
			sys_free(search->pack->hash);
			sys_free(search->pack->files);
			sys_free(search->pack);
			sys_free(search);
		}
		else
		{
			sys_free(search->dir);
			sys_free(search);
		}
		search=next;
	}

	//Close opened handles
	for(i=0;i<fs_numhandles;i++)
		fs_close(i);
}
