#include "config.h"
#include "../e_sound.h"

struct e_openalsource_s
{
	ALuint id;
};

typedef struct e_openalsource_s e_openalsource_t;

struct e_openalsound_s
{
	ALCdevice* device;
	ALCcontext* context;
};

typedef struct e_openalsound_s e_openalsound_t;

e_soundsystem_t* e_createsoundsystem(void)
{
	e_soundsystem_t* system;
	e_openalsound_t* sound;
	ALCdevice* device;
	ALCcontext* context;

	device=alcOpenDevice(NULL);
	if(!device)
		return 0;

	context=alcCreateContext(device,NULL);

	sound=malloc(sizeof(e_openalsound_t));
	sound->device=device;
	sound->context=context;
	system=malloc(sizeof(e_soundsystem_t));
	system->data=(void*)sound;

	return system;
}

void e_destroysoundsystem(e_soundsystem_t* system)
{
	e_openalsound_t* sound=(e_openalsound_t*)system->data;
	alcCloseDevice(sound->device);
	alcDestroyContext(sound->context);
	free(sound);
	free(system);
}

void e_setlistenerinfo(e_soundsystem_t* system,e_listenerinfo_t* listener)
{
	alListenerfv(AL_POSITION,listener->position);
	alListenerfv(AL_VELOCITY,listener->speed);
	alListenerfv(AL_ORIENTATION,listener->at);
}

e_source_t* e_createsource(e_soundsystem_t* system)
{
	e_source_t* source;
	e_openalsource_t* alsource;

	alsource=(e_openalsource_t*)malloc(sizeof(e_openalsource_t));
	alGenSources(1,&alsource->id);

	source=(e_source_t*)malloc(sizeof(e_source_t));
	source->data=(void*)alsource;

	return source;
}

void e_destroysource(e_source_t* source)
{
	e_openalsource_t* alsource=(e_openalsource_t*)source->data;
	alDeleteSources(1,&alsource->id);

	free(alsource);
	free(source);
}

void e_setsourceinfo(e_source_t* source,e_sourceinfo_t* info)
{
	e_openalsource_t* alsource=(e_openalsource_t*)source->data;
	alSourcefv(alsource->id,AL_POSITION,info->position);
	alSourcefv(alsource->id,AL_DIRECTION,info->direction);
	alSourcefv(alsource->id,AL_VELOCITY,info->speed);
}
