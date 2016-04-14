#include "config.h"
#include "e_font.h"
#include "e_utils.h"

static unsigned long e_readfont(FT_Stream stream,unsigned long offset,unsigned char* buffer,unsigned long count)
{
	fs_handle_t file=(fs_handle_t)stream->descriptor.value;
	fs_seek(file,offset,FS_SEEK_SET);
	return (unsigned long)fs_read(buffer,count,file);
}

static void e_closefont(FT_Stream stream)
{
	//Nothing to do
}

e_font_t* e_loadfont(e_device_t* device,unsigned int size,fs_handle_t file)
{
	e_font_t* font;
	FT_Open_Args args;
	FT_Face face;
	FT_StreamRec stream;
	unsigned int i;

	//Setup stream
	memset(&stream,0,sizeof(FT_StreamRec));
	stream.size=fs_size(file);
	stream.pos=0;
	stream.descriptor.value=(long)file;
	stream.read=e_readfont;
	stream.close=e_closefont;

	memset(&args,0,sizeof(FT_Open_Args));
	args.flags=FT_OPEN_STREAM;
	args.stream=&stream;
	if(FT_Open_Face(device->library,&args,0,&face))
		return 0;

	FT_Set_Char_Size(face,size<<6,size<<6,96,96);

	font=(e_font_t*)sys_alloc(sizeof(e_font_t));

	font->maxheight=0;
	font->maxwidth=0;
	font->base=glGenLists(E_FONT_CHARACTERS);
	glGenTextures(E_FONT_CHARACTERS,font->textures);

	//Create textures
	for(i=0;i<E_FONT_CHARACTERS;i++)
	{
		e_fontcharacter_t* character=&font->characters[i];
		unsigned int w,h;
		unsigned char* bitmap;
		unsigned int x,y;
		float xtex,ytex;

		if(FT_Load_Glyph(face,FT_Get_Char_Index(face,i),FT_LOAD_DEFAULT))
			continue;

		FT_Render_Glyph(face->glyph,FT_RENDER_MODE_NORMAL);

		character->width=face->glyph->bitmap.width;
		character->height=face->glyph->bitmap.rows;
		character->left=face->glyph->metrics.horiBearingX>>6;
		character->top=face->glyph->metrics.horiBearingY>>6;
		character->advance=face->glyph->advance.x>>6;

		if(font->maxheight<character->height)
			font->maxheight=character->height;
		if(font->maxwidth<character->width)
			font->maxwidth=character->width;

		w=e_pow2(character->width);
		h=e_pow2(character->height);

		//Read character bitmap
		bitmap=(unsigned char*)sys_alloc(sizeof(unsigned char)*2*w*h);
		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{
				const int index=(x+y*w)*2;
				bitmap[index]=0xFF;
				if(x>=character->width||y>=character->height)
					bitmap[index+1]=0;
				else
					bitmap[index+1]=face->glyph->bitmap.buffer[x+y*character->width];
			}
		}

		glBindTexture(GL_TEXTURE_2D,font->textures[i]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,bitmap);
		sys_free(bitmap);

		//Build a display list for this character
		xtex=(float)character->width/(float)w;
		ytex=(float)character->height/(float)h;

		glNewList(font->base+i,GL_COMPILE);

		glBindTexture(GL_TEXTURE_2D,font->textures[i]);
		glPushMatrix();

		glTranslatef((float)character->left,(float)-character->top,0.0f);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);
		glVertex2i(0,0);
		glTexCoord2f(xtex,0.0f);
		glVertex2i(character->width,0);
		glTexCoord2f(xtex,ytex);
		glVertex2i(character->width,character->height);
		glTexCoord2f(0.0f,ytex);
		glVertex2i(0,character->height);
		glEnd();

		glPopMatrix();

		glTranslatef((float)character->advance,0.0f,0.0f);
		glEndList();
	}
	FT_Done_Face(face);

	return font;
}

void e_destroyfont(e_font_t* font)
{
	glDeleteLists(font->base,E_FONT_CHARACTERS);
	glDeleteTextures(E_FONT_CHARACTERS,font->textures);
	sys_free(font);
}

void e_textsize(const e_font_t* font,const char* text,unsigned int* width,unsigned int* height)
{
	unsigned int i;
	const unsigned int size=strlen(text);
	*width=0;
	*height=0;

	for(i=0;i<size;i++)
	{
		const unsigned int index=(unsigned int)text[i];
		*width+=font->characters[index].advance;
		if(*height<font->characters[index].height)
			*height=font->characters[index].height;
	}
}

void e_textsizerange(const e_font_t* font,const char* text,unsigned int count,unsigned int* width,unsigned int* height)
{
	unsigned int i;
	*width=0;
	*height=0;

	for(i=0;i<count;i++)
	{
		const unsigned int index=(unsigned int)text[i];
		*width+=font->characters[index].advance;
		if(*height<font->characters[index].height)
			*height=font->characters[index].height;
	}
}

void e_print(e_device_t* device,const e_font_t* font,int x,int y,const char* text)
{
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glColor3f(1.0f,1.0f,1.0f);

	glPushMatrix();
	glTranslatef((mt_scalar_t)x,(mt_scalar_t)y,0.0f);
	glListBase(font->base);
	glCallLists((GLsizei)strlen(text),GL_UNSIGNED_BYTE,text);
	glPopMatrix();

	glPopAttrib();
}

void e_printrange(e_device_t* device,const e_font_t* font,int x,int y,const char* text,unsigned int count)
{
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glColor3f(1.0f,1.0f,1.0f);

	glPushMatrix();
	glTranslatef((mt_scalar_t)x,(mt_scalar_t)y,0.0f);
	glListBase(font->base);
	glCallLists((GLsizei)count,GL_UNSIGNED_BYTE,text);
	glPopMatrix();

	glPopAttrib();
}
