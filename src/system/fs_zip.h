/***********************************************************
*                                                          *
*  File : fs_zip.h                                         *
*  Date : 05/06/2011                                       *
*  Comments : Zip file loading                             *
*                                                          *
************************************************************/

#ifndef __FS_ZIP_H__
#define __FS_ZIP_H__

#include "sys_common.h"
#include "fs_common.h"

/*! \addtogroup sys */
/*! @{ */

/*! \name Zip */
/*! @{ */

//#define FS_ZIP_CRC32CHECK
#define FS_ZIP_SEPARATOR '/'

typedef unsigned int fs_uint32_t;
typedef unsigned short fs_uint16_t;
typedef unsigned char fs_uint8_t;

#define FS_ZIP_FILEMAGIC 0x02014B50

#pragma pack(push,1)
struct fs_zipfileheader_s
{
	fs_uint32_t magic;
	fs_uint16_t version;
	fs_uint16_t needed;
	fs_uint16_t flags;
	fs_uint16_t method;
	fs_uint16_t time;
	fs_uint16_t date;
	fs_uint32_t crc32;
	fs_uint32_t csize;
	fs_uint32_t size;
	fs_uint16_t namelen;
	fs_uint16_t extralen;
	fs_uint16_t commentlen;
	fs_uint16_t disknum;
	fs_uint16_t iattribs;
	fs_uint32_t eattribs;
	fs_uint32_t offset;
};
#pragma pack(pop)
typedef struct fs_zipfileheader_s fs_zipfileheader_t;

#define FS_ZIP_LOCALMAGIC 0x04034b50

#pragma pack(push,1)
struct fs_ziplocalheader_s
{
	fs_uint32_t magic;
	fs_uint16_t needed;
	fs_uint16_t flags;
	fs_uint16_t method;
	fs_uint16_t time;
	fs_uint16_t date;
	fs_uint32_t crc32;
	fs_uint32_t csize;
	fs_uint32_t size;
	fs_uint16_t namelen;
	fs_uint16_t extralen;
};
#pragma pack(pop)
typedef struct fs_ziplocalheader_s fs_ziplocalheader_t;

#define FS_ZIP_DIRECTORYMAGIC 0x06054B50

#pragma pack(push,1)
struct fs_zipcentraldirend_s
{
	fs_uint32_t magic;
	fs_uint16_t disknum;
	fs_uint16_t diskstart;
	fs_uint16_t numentries;
	fs_uint32_t size;
	fs_uint32_t offset;
	fs_uint16_t commentlen;
};
#pragma pack(pop)
typedef struct fs_zipcentraldirend_s fs_zipcentraldirend_t;

#define FS_ZIP_SIGNATUREMAGIC 0x05054B50

#pragma pack(push,1)
struct fs_zipsignature_s
{
	fs_uint32_t header;
	fs_uint16_t size;
};
#pragma pack(pop)
typedef struct fs_zipsignature_s fs_zipsignature_t;

#define FS_ZIP_SEEK 256
#define FS_ZIP_BUFFER 256

struct fs_zipstream_s
{
	int raw:1;
	sys_file file;
	fs_uint32_t offset;
	fs_uint32_t size;
#ifdef FS_ZIP_CRC32CHECK
	fs_uint32_t crc32check;
	fs_uint32_t crc32;
#endif
	z_stream stream;
	char buffer[FS_ZIP_BUFFER];
};

typedef struct fs_zipstream_s fs_zipstream_t;

/*!
  \brief Checks a zip file
  \param[in] file An opened system file handle
  \return 1 if the file is a valid zip file, otherwise 0
*/
int fs_zipcheck(sys_file file);
/*!
  \brief Reads a zip file header
  \param[in] file A valid zip file
  \param[in,out] end The storage location for the end of central directory header
  \return 1 on success, otherwise 0
*/
int fs_ziplocatedir(sys_file file,fs_zipcentraldirend_t* end);
/*!
  \brief Reads a zip file header
  \param[in] file A valid zip file
  \param[in,out] header The storage location for the file header
  \param[in,out] name A buffer that will receive the file name (or 0 if no file name is requested)
  \param[in,out] maxname The size of the name buffer
  \return 1 on success, otherwise 0
*/
int fs_zipreadheader(sys_file file,fs_zipfileheader_t* header,char* name,unsigned int maxname);
/*!
  \brief Reads a zip local header
  \param[in] file A valid zip file
  \param[in,out] offset The offset to the local header
  \param[in,out] local The storage location for the local header
  \return 1 on success, otherwise 0
*/
int fs_zipreadlocal(sys_file file,unsigned long offset,fs_ziplocalheader_t* local);
/*!
  \brief Opens a zip stream
  \param[in] name The archive file name
  \param[in] offset The offset within the zip archive
  \return A valid zip stream, or 0 if an error occured
*/
fs_zipstream_t* fs_zipopenstream(const char* name,unsigned long offset);
/*!
  \brief Reads a zip stream
  \param[in] buffer The buffer that will receive data
  \param[in] size The buffer size
  \param[in] stream The zip stream
  \return The number of bytes read (0 means the end of the stream or a fatal error)
*/
unsigned long fs_zipreadstream(void* buffer,fs_size_t size,fs_zipstream_t* stream);
/*!
  \brief Returns the current position
  \param[in] stream The zip stream
  \return The current position in the stream (from the start)
*/
unsigned long fs_ziptellstream(fs_zipstream_t* stream);
/*!
  \brief Changes the stream position
  \param[in] stream The zip stream
  \param[in] offset The offset relative to the origin
  \param[in] origin The origin to consider
  \return 0 on failure, otherwise 1
  \sa FS_SEEK_POSITION
*/
int fs_zipseekstream(fs_zipstream_t* stream,long offset,int origin);
/*!
  \brief Closes a zip stream
  \return 0 on failure, otherwise 1
*/
int fs_zipclosestream(fs_zipstream_t* stream);

/*! @} */

/*! @} */

#endif
