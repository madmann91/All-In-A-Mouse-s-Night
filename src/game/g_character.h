/***********************************************************
*                                                          *
*  File : g_character.h                                    *
*  Date : 24/12/2009                                       *
*  Comments : Character-related structures                 *
*                                                          *
************************************************************/

#ifndef __G_CHARACTER_H__
#define __G_CHARACTER_H__

#include "g_common.h"
#include "g_controller.h"
#include "mt_transform.h"
#include "e_skeleton.h"
#include "e_model.h"
#include "e_texture.h"

/*! \addtogroup g */
/*! @{ */

/*! \name Character */
/*! @{ */

#define G_MAX_CHARACTERNAME 32
#define G_MAX_WEAPONNAME 16
#define G_MAX_WEAPONS 4

/*! Weapon effect types */
enum G_WEAPON_EFFECT
{
	G_EFFECT_BULLET		/*!< Simply draw a bullet ray */
};

/*! A weapon object */
struct g_weapon_s
{
	char name[G_MAX_WEAPONNAME];	/*!< The weapon name */
	unsigned int effect;			/*!< The bullet effect type */

	e_model_t* model;				/*!< The weapon model */
	e_modelbuffer_t* buffer;		/*!< The weapon model buffer */
	e_texture_t* texture;			/*!< The weapon texture */
};

typedef struct g_weapon_s g_weapon_t;

/*! Character flags */
enum G_CHARACTER_FLAGS
{
	G_CHARACTER_CANJUMP=0x01			/*!< The character can jump */
};

/*! Character states */
enum G_CHARACTER_STATES
{
	G_CHARACTER_WAITING,			/*!< The character is waiting */
	G_CHARACTER_JUMPING,			/*!< The character is jumping */
	G_CHARACTER_RUNNING				/*!< The character is running */
};

/*! Character skeleton animations */
enum G_CHARACTER_ANIMATIONS
{
	G_ANIMATION_JUMP,				/*!< Jump animation */
	G_ANIMATION_WAIT,				/*!< Wait animation */
	G_ANIMATION_RUN					/*!< Run animation */
};

/*! A character class */
struct g_characterclass_s
{
	char name[G_MAX_CHARACTERNAME];	/*!< The character class name */
	mt_scalar_t invmass;			/*!< The character inverse mass */
	mt_scalar_t life;				/*!< The character life */
	int classid;					/*!< The character ai class id */

	e_model_t* model;				/*!< The character model */
	e_skeleton_t* skeleton;			/*!< The character skeleton */
	e_texture_t* texture;			/*!< The character texture */
};

typedef struct g_characterclass_s g_characterclass_t;

/*! The information needed to render a character */
struct g_rendercharacter_s
{
	mt_vector3_t light;				/*!< The position of the light in character-space */
	mt_vector3_t weaponlight;		/*!< The position of the light in weapon-space */
	mt_matrix4_t matrix;			/*!< The character matrix */
	mt_matrix4_t invweaponmatrix;	/*!< The character-space to weapon-space matrix */
	mt_matrix4_t weaponmatrix;		/*!< The weapon matrix */
	int islightened:1;				/*!< The flag that indicates if the character is lightened */
};

typedef struct g_rendercharacter_s g_rendercharacter_t;

/*! Character structure */
struct g_character_s
{
	//Game properties
	char name[G_MAX_CHARACTERNAME];		/*!< Character name */
	mt_scalar_t invmass;				/*!< Character inverse mass */
	unsigned int state;					/*!< Character state */
	mt_scalar_t armor;					/*!< The armor of the character (between 0 and 1) */
	mt_scalar_t life;					/*!< The life of the character (between 0 and 1) */
	int weapons[G_MAX_WEAPONS];			/*!< The weapons owned by the character (indices in the weapons array) */
	int currentweapon;					/*!< The current weapon, or -1 */
	unsigned int numweapons;			/*!< The number of weapons this character owns */
	g_controller_t* controller;			/*!< The character controller */
	int classid;						/*!< The character class id (for artificial intelligence purposes) */
	int flags;							/*!< Character flags */

	//Physic properties
	mt_transform_t transform;			/*!< Position and orientation in 3d space */
	mt_transform_t inverse;				/*!< Inverse position and orientation */
	mt_scalar_t heading;				/*!< Character heading */
	mt_scalar_t pitch;					/*!< Character pitch */
	mt_vector3_t velocity;				/*!< Charater velocity */
	mt_vector3_t halfextents;			/*!< The character's bounding box half extents */

	//Rendering
	g_rendercharacter_t render;			/*!< Rendering info */
	e_animation_t anim;					/*!< The current animation */
	e_texture_t* texture;				/*!< The player texture */
	e_model_t* model;					/*!< The player model */
	e_modelbuffer_t* modelbuffer;		/*!< The player model buffer */
	e_skeleton_t* skeleton;				/*!< The player skeleton */
	e_skeletonbuffer_t* skeletonbuffer;	/*!< The player skeleton buffer */
};

typedef struct g_character_s g_character_t;

/*!
  \brief Sets the character's current animation according to its state
  \param[in] character The character to animate
  \param[in] dt The elapsed time since the last call
*/
void g_animatecharacter(g_character_t* character,mt_scalar_t dt);
/*!
  \brief Destroys the given character class
  \param[in] type The character class to destroy
*/
void g_destroycharacterclass(g_characterclass_t* type);
/*!
  \brief Destroys the given character
  \param[in] character The character to destroy
*/
void g_destroycharacter(g_character_t* character);
/*!
  \brief Destroys the given weapon
  \param[in] weapon The weapon to destroy
*/
void g_destroyweapon(g_weapon_t* weapon);

/*! @} */

/*! @} */

#endif

