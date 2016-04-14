/***********************************************************
*                                                          *
*  File : sys_common.h                                     *
*  Date : 05/06/2011                                       *
*  Comments : Common system definitions                    *
*                                                          *
************************************************************/

#ifndef __SYS_COMMON_H__
#define __SYS_COMMON_H__

#include "config.h"

/*! \addtogroup sys System module */
/*! @{ */

#ifdef WIN32
typedef HANDLE sys_file;
#else
typedef int sys_file;
#endif

/*!
  \brief Initialises the system module
  \return 1 on success, otherwise 0
*/
int sys_init(void);
/*! \brief Destroys the system module */
void sys_quit(void);

/*!
  \brief Allocates a memory chunk on the heap
  \param[in] size The desired size of the chunk
  \return A valid memory chunk, or 0 if an error occured
*/
void* sys_alloc(unsigned int size);
/*!
  \brief Reallocates a memory chunk
  \param[in] ptr The address of the chunk to reallocate
  \param[in] size The new size of the chunk
  \return A valid memory chunk, or 0 if an error occured
*/
void* sys_realloc(void* ptr,unsigned int size);
/*!
  \brief Releases a memory chunk
  \param[in] ptr The address of the chunk to release
*/
void sys_free(void* ptr);

//Private hard-disk i/o (the functions provided by fs_common can read zip files)
sys_file sys_open(const char* filename,int mode);
unsigned long sys_read(void* buffer,unsigned int size,sys_file file);
unsigned long sys_write(const void* buffer,unsigned int size,sys_file file);
unsigned long sys_tell(sys_file file);
int sys_seek(sys_file file,long offset,int origin);
unsigned int sys_size(sys_file file);
int sys_close(sys_file file);
int sys_isvalid(sys_file file);

//File/path search functions
char** sys_listfiles(const char* path,const char* ext,unsigned int* count);
char** sys_listpaths(const char* filter,unsigned int* count);
void sys_freelist(char** list);

/*! @} */

#endif
