/***********************************************************
*                                                          *
*  File : e_device.h                                       *
*  Date : 13/11/2009                                       *
*  Comments : 3D device handling                           *
*                                                          *
************************************************************/

#ifndef __E_DEVICE_H__
#define __E_DEVICE_H__

#include "sys_common.h"
#include "e_common.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Device */
/*! @{ */

/*! \brief List of key codes */
enum E_KEYS
{
	E_KEY_FIRST=0,
	E_KEY_BACKSPACE=8,
	E_KEY_TAB=9,
	E_KEY_CLEAR=12,
	E_KEY_RETURN=13,
	E_KEY_PAUSE=19,
	E_KEY_ESCAPE=27,
	E_KEY_SPACE=32,
	E_KEY_EXCLAIM=33,
	E_KEY_QUOTEDBL=34,
	E_KEY_HASH=35,
	E_KEY_DOLLAR=36,
	E_KEY_AMPERSAND=38,
	E_KEY_QUOTE=39,
	E_KEY_LEFTPAREN=40,
	E_KEY_RIGHTPAREN=41,
	E_KEY_ASTERISK=42,
	E_KEY_PLUS=43,
	E_KEY_COMMA=44,
	E_KEY_MINUS=45,
	E_KEY_PERIOD=46,
	E_KEY_SLASH=47,
	E_KEY_0=48,
	E_KEY_1=49,
	E_KEY_2=50,
	E_KEY_3=51,
	E_KEY_4=52,
	E_KEY_5=53,
	E_KEY_6=54,
	E_KEY_7=55,
	E_KEY_8=56,
	E_KEY_9=57,
	E_KEY_COLON=58,
	E_KEY_SEMICOLON=59,
	E_KEY_LESS=60,
	E_KEY_EQUALS=61,
	E_KEY_GREATER=62,
	E_KEY_QUESTION=63,
	E_KEY_AT=64,
	E_KEY_LEFTBRACKET=91,
	E_KEY_BACKSLASH=92,
	E_KEY_RIGHTBRACKET=93,
	E_KEY_CARET=94,
	E_KEY_UNDERSCORE=95,
	E_KEY_BACKQUOTE=96,
	E_KEY_A=97,
	E_KEY_B=98,
	E_KEY_C=99,
	E_KEY_D=100,
	E_KEY_E=101,
	E_KEY_F=102,
	E_KEY_G=103,
	E_KEY_H=104,
	E_KEY_I=105,
	E_KEY_J=106,
	E_KEY_K=107,
	E_KEY_L=108,
	E_KEY_M=109,
	E_KEY_N=110,
	E_KEY_O=111,
	E_KEY_P=112,
	E_KEY_Q=113,
	E_KEY_R=114,
	E_KEY_S=115,
	E_KEY_T=116,
	E_KEY_U=117,
	E_KEY_V=118,
	E_KEY_W=119,
	E_KEY_X=120,
	E_KEY_Y=121,
	E_KEY_Z=122,
	E_KEY_DELETE=127,
	E_KEY_NUM0=256,
	E_KEY_NUM1=257,
	E_KEY_NUM2=258,
	E_KEY_NUM3=259,
	E_KEY_NUM4=260,
	E_KEY_NUM5=261,
	E_KEY_NUM6=262,
	E_KEY_NUM7=263,
	E_KEY_NUM8=264,
	E_KEY_NUM9=265,
	E_KEY_NUMPERIOD=266,
	E_KEY_NUMDIVIDE=267,
	E_KEY_NUMMULTIPLY=268,
	E_KEY_NUMMINUS=269,
	E_KEY_NUMPLUS=270,
	E_KEY_NUMENTER=271,
	E_KEY_NUMEQUALS=272,
	E_KEY_UP=273,
	E_KEY_DOWN=274,
	E_KEY_RIGHT=275,
	E_KEY_LEFT=276,
	E_KEY_INSERT=277,
	E_KEY_HOME=278,
	E_KEY_EN=279,
	E_KEY_PAGEUP= 280,
	E_KEY_PAGEDOWN= 281,
	E_KEY_F1=282,
	E_KEY_F2=283,
	E_KEY_F3=284,
	E_KEY_F4=285,
	E_KEY_F5=286,
	E_KEY_F6=287,
	E_KEY_F7=288,
	E_KEY_F8=289,
	E_KEY_F9=290,
	E_KEY_F10=291,
	E_KEY_F11=292,
	E_KEY_F12=293,
	E_KEY_F13=294,
	E_KEY_F14=295,
	E_KEY_F15=296,
	E_KEY_NUMLOCK=300,
	E_KEY_CAPSLOCK=301,
	E_KEY_SCROLLOCK=302,
	E_KEY_RSHIFT=303,
	E_KEY_LSHIFT=304,
	E_KEY_RCTRL=305,
	E_KEY_LCTRL=306,
	E_KEY_RALT=307,
	E_KEY_LALT=308
};

/*! \brief Types of events */
enum E_EVENTS
{
	E_EVENT_QUIT,				/*!< The application has been requested to quit */
	E_EVENT_MOUSEMOVE,			/*!< The mouse moved inside the application */
	E_EVENT_MOUSEBUTTONUP,		/*!< A mouse button was released */
	E_EVENT_MOUSEBUTTONDOWN,	/*!< A mouse button was pressed */
	E_EVENT_KEYDOWN,			/*!< A key was pressed */
	E_EVENT_KEYUP				/*!< A key was released */
};

/*! \brief Mouse button identifiers */
enum E_BUTTONS
{
	E_BUTTON_LEFT,				/*!< Left mouse button */
	E_BUTTON_MIDDLE,			/*!< Middle mouse button */
	E_BUTTON_RIGHT,				/*!< Right mouse button */
	E_BUTTON_NONE				/*!< No mouse button */
};

/*! \brief Types of frame */
enum E_FRAME_TYPE
{
	E_FRAME_BUMP=0x01,				/*!< Draw the frame as if it was on the top of the screen */
	E_FRAME_SUNKEN=0x02,			/*!< Draw the frame with a dark colour */
	E_FRAME_NOTOP=0x04,				/*!< Do not draw the top border */
	E_FRAME_NOBOTTOM=0x08,			/*!< Do not draw the bottom border */
	E_FRAME_NOLEFT=0x10,			/*!< Do not draw the left border */
	E_FRAME_NORIGHT=0x20,			/*!< Do not draw the right border */
	E_FRAME_NOBORDER=E_FRAME_NOLEFT|E_FRAME_NORIGHT|E_FRAME_NOBOTTOM|E_FRAME_NOTOP	/*!< Do not draw any border */
};

/*! \brief Mouse information */
struct e_mouseinfo_s
{
	int x;							/*!< Horizontal position */
	int y;							/*!< Vertical position */
	int xrel;						/*!< Horizontal relative position */
	int yrel;						/*!< Vertical relative position */
	unsigned int button;			/*!< Which button was pressed */
};

typedef struct e_mouseinfo_s e_mouseinfo_t;

/*! \brief Keyboard information */
struct e_keyboardinfo_s
{
	unsigned int key;				/*!< Key code */
};

typedef struct e_keyboardinfo_s e_keyboardinfo_t;

/*! \brief Event information */
struct e_event_s
{
	unsigned int type;				/*!< Event type */
	union
	{
		e_mouseinfo_t mouse;		/*!< Mouse information, if the even is a mouse-related message */
		e_keyboardinfo_t keyboard;  /*!< Keyboard information, if the even is a keyboard-related message */
	};
};

typedef struct e_event_s e_event_t;

/*! \brief Device structure */
struct e_device_s
{
	FT_Library library;				/*!< The freetype library object */

	unsigned int width;				/*!< Screen width */
	unsigned int height;			/*!< Screen height */
	unsigned int bpp;				/*!< Bits per pixel */

	int active:1;					/*!< Activation flag */
	int fullscreen:1;				/*!< Fullscreen flag */
};

typedef struct e_device_s e_device_t;

/*!
  \brief Creates a device
  \param[in] caption Window caption
  \param[in] width Screen width
  \param[in] height Screen height
  \param[in] bpp The number of bits per pixel
  \param[in] fullscreen Flag to indicate whether to use fullscreen mode
  \return 0 or a fully operationnal device
*/
e_device_t* e_createdevice(const char* caption,unsigned int width,unsigned int height,unsigned int bpp,int fullscreen);
/*!
  \brief Destroys a device
  \param[in] device The device to destroy
*/
void e_destroydevice(e_device_t* device);
/*!
  \brief Gets the mouse position
  \param[in] device The device to use
  \param[in,out] x Horizontal position
  \param[in,out] y Vertical position
*/
void e_getmouse(e_device_t* device,int* x,int* y);
/*!
  \brief Sets the mouse position
  \param[in] device The device to use
  \param[in] x Horizontal position
  \param[in] y Vertical position
*/
void e_setmouse(e_device_t* device,int x,int y);
/*!
  \brief Shows or hides the mouse pointer
  \param[in] device The device to use
  \param[in] show Visibility flag
*/
void e_showmouse(e_device_t* device,int show);
/*!
  \brief Gets keyboard information 
  \param[in] device The device to use
  \return The keyboard buffer, an array of 256 boolean values indexed by E_KEYS
*/
const unsigned char* e_getkeyboard(e_device_t* device);
/*!
  \brief Gets an event from the device event queue
  \param[in] device The device to poll event from
  \param[out] event The event, if there is an event available
  \return 1 if an event was found
*/
int e_getevent(e_device_t* device,e_event_t* event);
/*!
  \brief Clears the device screen
  \param[in] device The device to clear
*/
void e_cleardevice(e_device_t* device);
/*!
  \brief Swap the device buffers and renders the scene
  \param[in] device The device to render
*/
void e_renderdevice(e_device_t* device);
/*!
  \brief Renders a widget frame
  \param[in] device The device to render frame on
  \param[in] x Horizontal position
  \param[in] y Vertical position
  \param[in] width Frame width
  \param[in] height Frame height
  \param[in] flags Frame rendering flags
  \sa E_FRAME_TYPE
*/
void e_renderframe(e_device_t* device,int x,int y,unsigned int width,unsigned int height,int flags);
/*!
  \brief Renders a fading square
  \param[in] device The device to use
  \param[in] factor The fading factor (between 0..1)
  The projection matrix should be orthogonal before any call to this function
*/
void e_fade(e_device_t* device,mt_scalar_t factor);

/* GL extensions functions */

//Vertex buffer objects (required)
extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
extern PFNGLMAPBUFFERARBPROC glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
//Multitexturing (required)
//extern PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
//extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
//extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
//extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
//Vertex and fragment programs (required)
extern PFNGLBINDPROGRAMARBPROC glBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;
extern PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
extern PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;
extern PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;
//Two-sided stencil buffer (optional)
extern PFNGLACTIVESTENCILFACEEXTPROC glActiveStencilFaceEXT;

/*! @} */

/*! @} */

#endif
