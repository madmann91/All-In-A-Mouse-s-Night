#include "config.h"
#include "g_character.h"

void g_animatecharacter(g_character_t* character,mt_scalar_t dt)
{
	switch(character->state)
	{
		case G_CHARACTER_WAITING:
			e_nextanimation(&character->anim,character->skeleton,G_ANIMATION_WAIT);
			e_playanimation(&character->anim,character->skeleton,dt,dt);
			break;
		case G_CHARACTER_JUMPING:
			e_nextanimation(&character->anim,character->skeleton,G_ANIMATION_JUMP);
			e_stopanimation(&character->anim,character->skeleton,dt,dt);
			break;
		case G_CHARACTER_RUNNING:
			e_nextanimation(&character->anim,character->skeleton,G_ANIMATION_RUN);
			e_playanimation(&character->anim,character->skeleton,dt,dt);
			break;
	}
}

void g_destroycharacterclass(g_characterclass_t* type)
{
	e_destroymodel(type->model);
	e_destroytexture(type->texture);
	e_destroyskeleton(type->skeleton);
}

void g_destroycharacter(g_character_t* character)
{
	g_destroycontroller(character->controller);
	e_destroymodelbuffer(character->modelbuffer);
	e_destroyskeletonbuffer(character->skeletonbuffer);
}

void g_destroyweapon(g_weapon_t* weapon)
{
	e_destroytexture(weapon->texture);
	e_destroymodel(weapon->model);
	e_destroymodelbuffer(weapon->buffer);
}
