#include "config.h"
#include "fs_private.h"

fs_searchpath_t* fs_paths=0;
fs_file_t fs_handles[FS_MAX_HANDLES];
unsigned int fs_numhandles=0;

int fs_matchfilter(const char* text,const char* filter)
{
	const char* ptr=text;
	unsigned int len=strlen(filter);
	unsigned int i=0;

	//We determine if "text" matches "filter"
	while(1)
	{
		if(filter[i]=='*')
		{
			if(i+1==len)
				return 1;

			ptr=strchr(ptr,filter[++i]);
			if(!ptr)
				return 0;

			ptr++;
			i++;
			continue;
		}

		if(i>=len)
			return 1;
		if(*ptr=='\0')
			return 0;

		if(filter[i]!='?'&&tolower(filter[i])!=tolower(*ptr))
			return 0;

		ptr++;
		i++;
	}

	//This line is never reached
	return 1;
}

unsigned int fs_hashfilename(const char* name,unsigned int size)
{
	unsigned int i;
	unsigned int hash;
	char letter;

	//This is inspired by Q3 source
	hash=0;
	i=0;

	while(name[i])
	{
		letter=tolower(name[i]);

		if(letter=='.') break;
		if(letter=='\\') letter='/';
		if(letter==FS_SEPARATOR) letter='/';

		hash+=(long)(letter)*(i+119);
		i++;
	}

	hash=(hash^(hash>>10)^(hash >> 20));
	hash&=(size-1);
	return hash;
}

const char* fs_makepath(const char* base,const char* path,const char* file)
{
	static char paths[FS_MAKE_PATH][FS_MAX_PATH];
	static unsigned int index;

	index=(index+1)%FS_MAKE_PATH;
	sprintf(paths[index],"%s%c%s%c%s",base,FS_SEPARATOR,path,FS_SEPARATOR,file);
	return paths[index];
}

int fs_comparepaths(const char* path1,const char* path2)
{
	unsigned int i=0;

	//Case and separators insensitive comparison
	while(path1[i]&&path2[i])
	{
		if(path1[i]=='\\'||path1[i]=='/')
		{
			if(path2[i]!='\\'&&path2[i]!='/')
				return path1[i]-path2[i];
		}
		else
		{
			char a=tolower(path1[i]);
			char b=tolower(path2[i]);
			
			if(a!=b)
				return a-b;
		}

		i++;
	}

	return (path1[i]-path2[i]);
}

char* fs_setseparators(char* path)
{
	char* ptr=path;

	//Set the right separators
	while(*ptr)
	{
		if(*ptr=='\\'||*ptr=='/')
			*ptr=FS_SEPARATOR;
		ptr++;
	}
	return path;
}
