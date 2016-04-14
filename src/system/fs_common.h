/***********************************************************
*                                                          *
*  File : fs_common.h                                      *
*  Date : 04/06/2011                                       *
*  Comments : File system definitions                      *
*                                                          *
************************************************************/

#ifndef __FS_COMMON_H__
#define __FS_COMMON_H__

/*! \addtogroup sys */
/*! @{ */

/*! \name File system */
/*! @{ */

#define FS_INVALID_HANDLE -1
#define FS_MAX_HANDLES 16
#define FS_MAX_PATH 256
typedef unsigned int fs_handle_t;
typedef unsigned long fs_size_t;

/*! The available opening modes */
enum FS_OPEN_MODES
{
	FS_OPEN_READ,		/*!< Open for reading */
	FS_OPEN_WRITE		/*!< Open for writing */
};

/*! Origins in a file */
enum FS_SEEK_POSITION
{
	FS_SEEK_SET,		/*!< The beginning of the file */
	FS_SEEK_CUR,		/*!< The current position in the file */
	FS_SEEK_END			/*!< The end of the file */
};

/*!
  \brief Initialises the file system
  \return 0 on failure, otherwise 1
*/
int fs_init(void);
/*! \brief Destroys the file system */
void fs_quit(void);

/*!
  \brief Returns a file name string
  \param[in] path The path containing the file
  \param[in] file The file name
  \return The path to the file (ie "path\file")
*/
const char* fs_pathtofile(const char* path,const char* file);
/*!
  \brief Adds a new search directory
  \param[in] path The path to the directory
  \param[in] dir The directory name
  \return 0 on failure, otherwise 1
  This simply adds the given directory, and the zip files contained in it as new search paths.
*/
int fs_registerpath(const char* path,const char* dir);

/*!
  \brief Opens a file
  \param[in] name The file name
  \param[in] mode The opening mode
  \return A valid file handle, or FS_INVALID_HANDLE on error.
  \sa FS_OPEN_MODES
  The registered search directories will be scanned to find the file, as well as registered zip files.
  Zipped files cannot be opened in FS_OPEN_WRITE mode.
*/
fs_handle_t fs_open(const char* name,int mode);
/*!
  \brief Returns the size of a file
  \param[in] file The file handle
  \return The size of the file
*/
fs_size_t fs_size(fs_handle_t file);
/*!
  \brief Reads the contents of a file
  \param[in] buffer The buffer that will receive the file data
  \param[in] size The number of bytes to read
  \param[in] file The file handle
  \return The number of bytes read
  This only works if the file has been opened in FS_OPEN_READ mode.
*/
fs_size_t fs_read(void* buffer,fs_size_t size,fs_handle_t file);
/*!
  \brief Write in a file
  \param[in] buffer The buffer that contains the data
  \param[in] size The number of bytes to write
  \param[in] file The file handle
  \return The number of bytes written
  This only works if the file has been opened in FS_OPEN_WRITE mode.
*/
fs_size_t fs_write(const void* buffer,fs_size_t size,fs_handle_t file);
/*!
  \brief Sets the file pointer position
  \param[in] file The file handle
  \param[in] offset The offset from the origin
  \param[in] origin The origin to consider  
  \return 0 on failure, otherwise 1
  \sa FS_SEEK_POSITION
*/
int fs_seek(fs_handle_t file,long offset,int origin);
/*!
  \brief Returns the file pointer position
  \param[in] file The file handle
  \return The current position of the file pointer, from the start of the file
*/
unsigned long fs_tell(fs_handle_t file);
/*!
  \brief Closes a file handle
  \param[in] file The file handle to close
  \return 0 on failure, otherwise 1
*/
int fs_close(fs_handle_t file);

/*! @} */

/*! @} */

#endif
