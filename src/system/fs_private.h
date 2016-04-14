/***********************************************************
*                                                          *
*  File : fs_private.h                                     *
*  Date : 05/06/2011                                       *
*  Comments : Private definitions                          *
*                                                          *
************************************************************/

#ifndef __FS_PRIVATE_H__
#define __FS_PRIVATE_H__

#include "config.h"
#include "sys_common.h"
#include "fs_common.h"
#include "fs_zip.h"

/*! \addtogroup sys */
/*! @{ */

//Private definitions, changing them will change the file system behaviour
#define FS_MAX_LIST 32			//The maximum number of files/paths in lists 
#define FS_MAX_HASH 256			//The maximum package hash table size
#define FS_MAKE_PATH 4			//The maximum number of independant calls to fs_makepath
#define FS_SEPARATOR '/'		//The file system separator
#define FS_PACK_EXT ".zip"		//Package files extensions
#define FS_BASE_PATH "."		//The current directory

//A real file system directory (ex : "fonts\hud")
struct fs_directory_s
{
	char name[FS_MAX_PATH];			// ex : "fonts"
	char dirname[FS_MAX_PATH];		// ex : "hud"
};

typedef struct fs_directory_s fs_directory_t;

//A zip package file
struct fs_packfile_s
{
	char name[FS_MAX_PATH];			//File name
	unsigned long offset;			//Offset in the archive
	struct fs_packfile_s* next;		//Next file
};

typedef struct fs_packfile_s fs_packfile_t;

//A zip package
struct fs_package_s
{
	char name[FS_MAX_PATH];		//Package file name
	unsigned int numfiles;		//Number of files
	unsigned int hashsize;		//Hash table size
	sys_file fp;				//System file handle
	fs_packfile_t** hash;		//Hash table (fast "find" operation)
	fs_packfile_t* files;		//The file list
};

typedef struct fs_package_s fs_package_t;

//A search path (only one of pack or dir will be non 0)
struct fs_searchpath_s
{
	struct fs_searchpath_s* next;	//The next path
	fs_package_t* pack;				//A package file object or
	fs_directory_t* dir;			//A directory
};

typedef struct fs_searchpath_s fs_searchpath_t;

//A file handle
struct fs_file_s
{
	fs_size_t size;			//The file size
	fs_zipstream_t* zip;	//A handle to a zip stream or
	sys_file fp;			//A handle to a system file
};

typedef struct fs_file_s fs_file_t;

//Global variables 
extern fs_searchpath_t* fs_paths;
extern fs_file_t fs_handles[FS_MAX_HANDLES];
extern unsigned int fs_numhandles;

//Utility functions
int fs_matchfilter(const char* text,const char* filter);
unsigned int fs_hashfilename(const char* name,unsigned int size);
const char* fs_makepath(const char* base,const char* path,const char* file);
int fs_comparepaths(const char* path1,const char* path2);
char* fs_setseparators(char* path);

/*! @} */

#endif
