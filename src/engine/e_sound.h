#ifndef __E_SOUND_H__
#define __E_SOUND_H__

struct e_sourceinfo_s
{
	mt_scalar_t position[3];
	mt_scalar_t speed[3];
	mt_scalar_t direction[3];
};

typedef struct e_sourceinfo_s e_sourceinfo_t;

struct e_source_s
{
	void* data;
};

typedef struct e_source_s e_source_t;

struct e_listenerinfo_s
{
	mt_scalar_t position[3];
	mt_scalar_t speed[3];
	mt_scalar_t at[3];
	mt_scalar_t up[3];
};

typedef struct e_listenerinfo_s e_listenerinfo_t;

struct e_soundsystem_s
{
	void* data;
};

typedef struct e_soundsystem_s e_soundsystem_t;

e_soundsystem_t* e_createsoundsystem(void);
void e_destroysoundsystem(e_soundsystem_t*);
void e_setlistenerinfo(e_soundsystem_t*,e_listenerinfo_t*);

e_source_t* e_createsource(e_soundsystem_t*);
void e_destroysource(e_source_t*);
void e_setsourceinfo(e_source_t*,e_sourceinfo_t*);

#endif
