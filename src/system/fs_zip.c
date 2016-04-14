#include "fs_private.h"
#include "fs_zip.h"

int fs_zipcheck(sys_file file)
{
	fs_uint32_t sig;
	//Check the 1st file signature
	sys_read(&sig,sizeof(fs_uint32_t),file);
	return (sig==FS_ZIP_LOCALMAGIC);
}

int fs_ziplocatedir(sys_file file,fs_zipcentraldirend_t* end)
{
	char buf[FS_ZIP_SEEK];
	int count=1;
	int found=0;
	int i;

	if(!sys_seek(file,-FS_ZIP_SEEK,FS_SEEK_END))
		return 0;

	do
	{
		if(sys_read(buf,FS_ZIP_SEEK,file)!=FS_ZIP_SEEK)
			return 0;

		//Try to find the central directory
		for(i=FS_ZIP_SEEK-1;i>=0;i--)
		{
			fs_uint32_t m=*(fs_uint32_t*)(buf+i);
			if(m==FS_ZIP_DIRECTORYMAGIC)
			{
				//We found it, stop here
				found=1;
				sys_seek(file,(-FS_ZIP_SEEK+i),FS_SEEK_CUR);
				sys_read(end,sizeof(fs_zipcentraldirend_t),file);

				//Set the pointer just before the central dir signature
				sys_seek(file,-(long)sizeof(fs_zipcentraldirend_t),FS_SEEK_CUR);
				return 1;
			}
		}

		//Continue the search
		count++;
		if(!sys_seek(file,-(count*FS_ZIP_SEEK),FS_SEEK_END))
			return 0;
	}while(!found);

	return 1;
}

int fs_zipreadheader(sys_file file,fs_zipfileheader_t* header,char* name,unsigned int maxname)
{
	char buf[FS_ZIP_SEEK];
	int found=0;
	int i;

	if(!sys_seek(file,-FS_ZIP_SEEK,FS_SEEK_CUR))
		return 0;

	do
	{
		if(sys_read(buf,FS_ZIP_SEEK,file)!=FS_ZIP_SEEK)
			return 0;

		//Try to find the file header signature
		for(i=FS_ZIP_SEEK-1;i>=0;i--)
		{
			fs_uint32_t m=*(fs_uint32_t*)(buf+i);
			if(m==FS_ZIP_FILEMAGIC)
			{
				long offset;

				//We found it, stop here
				found=1;
				sys_seek(file,(-FS_ZIP_SEEK+i),FS_SEEK_CUR);
				offset=sys_tell(file);

				sys_read(header,sizeof(fs_zipfileheader_t),file);
				if(name)
				{
					unsigned int size=((maxname-1)<header->namelen)?(maxname-1):header->namelen;
					sys_read(name,size,file);
					name[size]=0;
				}

				//Restore file pointer
				sys_seek(file,offset,FS_SEEK_SET);
				return 1;
			}
		}

		//Continue the search
		if(!sys_seek(file,-2*FS_ZIP_SEEK,FS_SEEK_CUR))
			return 0;
	}while(!found);

	return 1;
}

int fs_zipreadlocal(sys_file file,unsigned long offset,fs_ziplocalheader_t* local)
{
	if(!sys_seek(file,offset,FS_SEEK_SET))
		return 0;
	if(sys_read(local,sizeof(fs_ziplocalheader_t),file)!=sizeof(fs_ziplocalheader_t))
		return 0;
	return 1;
}

fs_zipstream_t* fs_zipopenstream(const char* name,unsigned long offset)
{
	sys_file file;
	fs_ziplocalheader_t local;
	fs_zipstream_t* stream;

	file=sys_open(name,FS_OPEN_READ);
	if(!sys_isvalid(file))
		return 0;

	//Read local header
	if(!sys_seek(file,offset,FS_SEEK_SET))
	{
		sys_close(file);
		return 0;
	}

	if(sys_read(&local,sizeof(fs_ziplocalheader_t),file)!=sizeof(fs_ziplocalheader_t))
	{
		sys_close(file);
		return 0;
	}

	//Only handle deflated and raw zip files
	if(local.method!=8&&local.method!=0)
		return 0;

	//Skip file name and extra data
	sys_seek(file,local.namelen+local.extralen,FS_SEEK_CUR);

	//Prepare stream for reading
	stream=sys_alloc(sizeof(fs_zipstream_t));
	memset(&stream->stream,0,sizeof(z_stream));

	stream->offset=offset+sizeof(fs_ziplocalheader_t)+local.namelen+local.extralen;

	if(local.method==8)
	{
		stream->raw=0;
		inflateInit2(&stream->stream,-15);
	}
	else
	{
		stream->raw=1;
		stream->stream.avail_in=local.size;
	}

	stream->file=file;
#ifdef FS_ZIP_CRC32CHECK
	stream->crc32check=local.crc32;
	stream->crc32=crc32(0,0,0);
#endif
	stream->size=local.size;

	return stream;
}

unsigned long fs_zipreadstream(void* buffer,fs_size_t size,fs_zipstream_t* stream)
{
	unsigned long count=0;

	if(stream->raw)
	{
		count=(size<stream->stream.avail_in)?size:stream->stream.avail_in;
		sys_read(buffer,count,stream->file);
		stream->stream.avail_in-=count;
		stream->stream.total_out+=count;
	}
	else
	{
		stream->stream.next_out=buffer;
		stream->stream.avail_out=size;

		//Make the data available from the caller
		while(stream->stream.avail_out>0)
		{
			unsigned long before;
			int ret;

			if(!stream->stream.avail_in)
			{
				sys_read(stream->buffer,FS_ZIP_BUFFER,stream->file);
				stream->stream.avail_in=FS_ZIP_BUFFER;
				stream->stream.next_in=stream->buffer;
			}

			before=stream->stream.avail_out;
			ret=inflate(&stream->stream,Z_SYNC_FLUSH);
			count+=(before-stream->stream.avail_out);

			if(ret==Z_STREAM_END||ret!=Z_OK)
				break;
		}
	}

#ifdef FS_ZIP_CRC32CHECK
	stream->crc32=crc32(stream->crc32,buffer,count);
#endif

	return count;
}

unsigned long fs_ziptellstream(fs_zipstream_t* stream)
{
	return stream->stream.total_out;
}

int fs_zipseekstream(fs_zipstream_t* stream,long offset,int origin)
{
	char buffer[FS_ZIP_BUFFER];
	
	if(stream->raw)
	{
		if(origin==FS_SEEK_SET)
			sys_seek(stream->file,stream->offset+offset,FS_SEEK_SET);
		else if(origin==FS_SEEK_CUR)
			sys_seek(stream->file,offset,FS_SEEK_CUR);
		else //origin==FS_SEEK_END
			sys_seek(stream->file,stream->size+stream->offset+offset,FS_SEEK_SET);
	}
	else
	{
		//Avoid restarting from the beginning when possible
		if(origin==FS_SEEK_SET&&(offset-(long)stream->stream.total_out)>=0)
		{
			origin=FS_SEEK_CUR;
			offset-=stream->stream.total_out;
		}

		if(origin==FS_SEEK_CUR&&offset>0)
		{
			//Seek from the current position
			while(offset>0)
			{
				unsigned long size=(offset>FS_ZIP_BUFFER)?FS_ZIP_BUFFER:offset;
				fs_zipreadstream(buffer,size,stream);
				offset-=size;
			}
		}
		else
		{
			unsigned long seek;

			if(origin==FS_SEEK_END)
				seek=offset+stream->size;
			else if(origin==FS_SEEK_SET)
				seek=offset;
			else //origin==FS_SEEK_CUR
				seek=stream->stream.total_out+offset;

			//Seek from the start (quite slow, better use FS_SEEK_CUR)
			if(inflateReset(&stream->stream)!=Z_OK)
				return 0;
	
#ifdef FS_ZIP_CRC32CHECK
			stream->crc32=crc32(0,0,0);
#endif

			//Restart stream
			stream->stream.avail_in=0;
			sys_seek(stream->file,stream->offset,FS_SEEK_SET);

			while(seek>0)
			{
				unsigned long size=(seek>FS_ZIP_BUFFER)?FS_ZIP_BUFFER:seek;
				fs_zipreadstream(buffer,size,stream);
				seek-=size;
			}
		}
	}

	return 1;
}

int fs_zipclosestream(fs_zipstream_t* stream)
{
#ifdef FS_ZIP_CRC32CHECK
	int ok=(stream->crc32==stream->crc32check);
#endif
	if(!stream->raw)
		inflateEnd(&stream->stream);
	sys_close(stream->file);
	sys_free(stream);
#ifdef FS_ZIP_CRC32CHECK
	return ok;
#else
	return 1;
#endif
}
