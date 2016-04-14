#include "e_texture.h"
#include "e_utils.h"

static int e_checkpng(fs_handle_t file)
{
	png_byte sig[8];
	fs_read(sig,sizeof(png_byte)*8,file);
	return (!png_sig_cmp(sig,0,8));
}

static void e_readpng(png_structp png,png_bytep data,png_size_t length)
{
	if(fs_read(data,length,(fs_handle_t)png_get_io_ptr(png))!=length)
		png_error(png,"read error");
}

static e_texture_t* e_loadpng(e_device_t* device,int mipmaps,fs_handle_t file)
{
	unsigned int width,height;
	unsigned int rowbytes;
	unsigned char* data;
	unsigned char** rows;
	unsigned int i;
	int bpp,type;
	int format;

	png_structp png;
	png_infop info;
	e_texture_t* texture;

	if(!e_checkpng(file))
		return 0;

	png=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,(png_error_ptr)0,(png_error_ptr)0);
	if(!png)
		return 0;

	info=png_create_info_struct(png);
	if(!info)
	{
		png_destroy_read_struct(&png,0,0);
		return 0;
	}

	//Needed by libpng
	if(setjmp(png_jmpbuf(png)))
	{
        png_destroy_read_struct(&png,&info,0);
		if(rows)
			sys_free(rows);
		if(data)
			sys_free(data);
        return 0;
    }

	//Read up to image data
	png_set_read_fn(png,(png_voidp)file,e_readpng);
	png_set_sig_bytes(png,8);
	png_read_info(png,info);
	png_get_IHDR(png,info,&width,&height,&bpp,&type,0,0,0);

	if(width!=e_pow2(width)||
		height!=e_pow2(height))
		longjmp(png_jmpbuf(png),1);

	//Convert this image to fit the required format
	if(type==PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);
	if(type==PNG_COLOR_TYPE_GRAY||
        type==PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
	if(type==PNG_COLOR_TYPE_GRAY||bpp<8)
        png_set_expand(png);
    if(png_get_valid(png,info,PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

	png_read_update_info(png,info);
	rowbytes=png_get_rowbytes(png,info);

	//Allocate memory to store the image
	data=(unsigned char*)sys_alloc(rowbytes*height);
	if(!data)
		longjmp(png_jmpbuf(png),1);

	rows=(unsigned char**)sys_alloc(sizeof(unsigned char*)*height);
	if(!rows)
		longjmp(png_jmpbuf(png),1);

	for(i=0;i<height;i++)
		rows[i]=data+(height-(i+1))*rowbytes;

	//Read png
	png_read_image(png,rows);
	png_read_end(png,0);
	sys_free(rows);

	texture=(e_texture_t*)sys_alloc(sizeof(e_texture_t));
	texture->width=width;
	texture->height=height;

	glGenTextures(1,&texture->id);
	glBindTexture(GL_TEXTURE_2D,texture->id);

	texture->bpp=(type&PNG_COLOR_MASK_ALPHA)?32:24;
	format=(type&PNG_COLOR_MASK_ALPHA)?GL_RGBA:GL_RGB;
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	if(mipmaps)
		gluBuild2DMipmaps(GL_TEXTURE_2D,(type&PNG_COLOR_MASK_ALPHA)?4:3,texture->width,texture->height,format,GL_UNSIGNED_BYTE,data);
	else
		glTexImage2D(GL_TEXTURE_2D,0,format,texture->width,texture->height,0,format,GL_UNSIGNED_BYTE,data);

	sys_free(data);
	return texture;
}

struct e_jpgerror_s
{
	struct jpeg_error_mgr mgr;
	jmp_buf buffer;
};

typedef struct e_jpgerror_s e_jpgerror_t;

struct e_jpgsource_s
{
	struct jpeg_source_mgr mgr;
	fs_handle_t file;
	fs_size_t size;
	unsigned char* buffer;
};

typedef struct e_jpgsource_s e_jpgsource_t;

static void e_jpgerror(j_common_ptr cinfo)
{
	e_jpgerror_t* error=(e_jpgerror_t*)cinfo->err;
	longjmp(error->buffer,1);
}

static boolean e_jpgfillbuffer(j_decompress_ptr cinfo)
{
	e_jpgsource_t* src=(e_jpgsource_t*)cinfo->src;
	fs_size_t count;

	//Adapted from jdatasrc.c in the jpeg package
	count=fs_read(src->buffer,src->size,src->file);

	if(count<=0)
	{
		src->buffer[0]=0xFF;
		src->buffer[1]=JPEG_EOI;
		count=2;
	}

	src->mgr.next_input_byte=src->buffer;
	src->mgr.bytes_in_buffer=count;

	return TRUE;
}

static void e_jpgskipdata(j_decompress_ptr cinfo,long numbytes)
{
	e_jpgsource_t* src=(e_jpgsource_t*)cinfo->src;

	if(numbytes>0)
		fs_seek(src->file,numbytes,FS_SEEK_CUR);
}

static void e_jpgnop(j_decompress_ptr cinfo)
{
	//Nothing to do here...
}

static e_texture_t* e_loadjpg(e_device_t* device,int mipmaps,fs_handle_t file)
{
	struct jpeg_decompress_struct cinfo;
	unsigned char* data=0;
	unsigned int width,height;
	unsigned int stride;
	e_jpgerror_t error;
	e_jpgsource_t* src=0;
	e_texture_t* texture;

	cinfo.err=jpeg_std_error(&error.mgr);
	error.mgr.error_exit=e_jpgerror;

	if(setjmp(error.buffer))
	{
		//Destroy memory
		src=(e_jpgsource_t*)cinfo.src;
		jpeg_destroy_decompress(&cinfo);
		if(src)
		{
			sys_free(src->buffer);
			sys_free(src);
		}
		if(data)
			sys_free(data);
		return 0;
	}

	cinfo.src=0;
	jpeg_create_decompress(&cinfo);

	cinfo.src=(struct jpeg_source_mgr*)sys_alloc(sizeof(e_jpgsource_t));

	//Init data source
	src=(e_jpgsource_t*)cinfo.src;
	src->mgr.init_source=e_jpgnop;
	src->mgr.fill_input_buffer=e_jpgfillbuffer;
	src->mgr.skip_input_data=e_jpgskipdata;
	src->mgr.resync_to_restart=jpeg_resync_to_restart;
	src->mgr.term_source=e_jpgnop;
	src->mgr.bytes_in_buffer=0;
	src->mgr.next_input_byte=0;
	src->file=file;

	src->size=256;
	src->buffer=sys_alloc(sizeof(unsigned char)*src->size);

	jpeg_read_header(&cinfo,TRUE);
	jpeg_start_decompress(&cinfo);

	//Do not accept textures with non-power-of-two dimensions
	if(cinfo.output_width!=e_pow2(cinfo.output_width)||
		cinfo.output_height!=e_pow2(cinfo.output_height))
		longjmp(error.buffer,1);

	//Force RGB output
	cinfo.out_color_space=JCS_RGB;
	width=cinfo.output_width;
	height=cinfo.output_height;

	stride=sizeof(unsigned char)*3*cinfo.output_width;
	data=(unsigned char*)sys_alloc(stride*cinfo.output_height);

	//Read each scanline
	while(cinfo.output_scanline<cinfo.output_height)
	{
		unsigned char* ptr=data+stride*(cinfo.output_height-(cinfo.output_scanline+1));
		unsigned char** row=&ptr;
		jpeg_read_scanlines(&cinfo,row,1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	sys_free(src->buffer);
	sys_free(src);

	texture=(e_texture_t*)sys_alloc(sizeof(e_texture_t));
	texture->width=width;
	texture->height=height;
	texture->bpp=24;

	glGenTextures(1,&texture->id);
	glBindTexture(GL_TEXTURE_2D,texture->id);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	if(mipmaps)
		gluBuild2DMipmaps(GL_TEXTURE_2D,3,texture->width,texture->height,GL_RGB,GL_UNSIGNED_BYTE,data);
	else
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texture->width,texture->height,0,GL_RGB,GL_UNSIGNED_BYTE,data);

	sys_free(data);
	return texture;
}

e_texture_t* e_loadtexture(e_device_t* device,int mipmaps,fs_handle_t file)
{
	//Try to load the texture as a png first
	e_texture_t* texture;
	texture=e_loadpng(device,mipmaps,file);
	if(!texture)
	{
		//Restart stream and try to read as JPG
		fs_seek(file,0,FS_SEEK_SET);
		return e_loadjpg(device,mipmaps,file);
	}
	return texture;
}

e_texture_t* e_creategradient(e_device_t* device,const unsigned char* values,unsigned int numvalues)
{	
	e_texture_t* texture;

	//We load a gradient that will be used for shading
	if(e_pow2(numvalues)!=numvalues)
		return 0;

	texture=(e_texture_t*)sys_alloc(sizeof(e_texture_t));
	texture->width=numvalues;
	texture->height=1;
	texture->bpp=8;

	glGenTextures(1,&texture->id);
	glBindTexture(GL_TEXTURE_1D,texture->id);

	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,numvalues,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,values);
	glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP);

	return texture;
}

void e_destroytexture(e_texture_t* texture)
{
	glDeleteTextures(1,&texture->id);
	sys_free(texture);
}

void e_rendertexture(e_device_t* device,e_texture_t* texture,int xoffset,int yoffset,unsigned int width,unsigned int height,int x,int y)
{
	const float offset[2]={(float)xoffset/(float)texture->width,1.0f-(float)(yoffset+height)/(float)texture->height};
	const float size[2]={(float)width/(float)texture->width,(float)height/(float)texture->height};

	glBindTexture(GL_TEXTURE_2D,texture->id);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);

	glTexCoord2f(offset[0]+size[0],offset[1]+size[1]);
	glVertex2i(x+width,y);
	
	glTexCoord2f(offset[0]+size[0],offset[1]);
	glVertex2i(x+width,y+height);
	
	glTexCoord2f(offset[0],offset[1]);
	glVertex2i(x,y+height);
	
	glTexCoord2f(offset[0],offset[1]+size[1]);
	glVertex2i(x,y);

	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}
