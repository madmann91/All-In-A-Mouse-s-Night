/***********************************************************
*                                                          *
*  File : e_particles.h                                    *
*  Date : 13/11/2009                                       *
*  Comments : Particle system                              *
*                                                          *
************************************************************/

#ifndef __E_PARTICLES_H__
#define __E_PARTICLES_H__

#include "e_device.h"

#include "mt_vector.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Particles */
/*! @{ */

/*! A particle system element */
struct e_element_s
{
	mt_vector3_t position;			/*!< Element position in space */
	mt_vector3_t velocity;			/*!< Element velocity */
	e_color4_t color;				/*!< Element color in RGBA format */
};

typedef struct e_element_s e_element_t;

/*! Particle system structure */
struct e_particles_s
{
	unsigned int numelements;		/*!< The number of elements */
	e_element_t* elements;			/*!< The element array */
	unsigned int numactive;			/*!< The number of visible elements */
	mt_scalar_t speed;				/*!< The fading speed */
};

typedef struct e_particles_s e_particles_t;

/*! Particle system modifications */
enum E_PARTICLES_FLAGS
{
	E_PARTICLES_WHITE=0x01,			/*!< Set each element color to white */
	E_PARTICLES_BLACK=0x02,			/*!< Set each element color to black */
	E_PARTICLES_XRAND=0x04,			/*!< Randomize velocity and position between -1..1 on each x-axis */
	E_PARTICLES_YRAND=0x08,			/*!< Randomize velocity and position between -1..1 on each y-axis */
	E_PARTICLES_ZRAND=0x10,			/*!< Randomize velocity and position between -1..1 on each z-axis */
};

/*!
  \brief Creates a particle system
  \param[in] device The device to use
  \param[in] numelements The number of elements the system will have
  \return A valid particle system object or 0 if an error occured
*/
e_particles_t* e_createparticles(e_device_t* device,unsigned int numelements);
/*!
  \brief Reset a particle system
  \param[in] particles The particle system to reset
  \param[in] flags A combinaison of particle system modification flags
*/
void e_resetparticles(e_particles_t* particles,int flags);
/*!
  \brief Updates the given particle system
  \param[in] particles The particle system to update
  \param[in] forces The forces to apply to each element
  \param[in] friction The friction factor
  \param[in] dt Time step
*/
void e_updateparticles(e_particles_t* particles,const mt_vector3_t forces,mt_scalar_t friction,mt_scalar_t dt);
/*!
  \brief Translates every particle contained in the given system
  \param[in] particles The particle system to translate
  \param[in] x The translation on x-axis
  \param[in] y The translation on y-axis
  \param[in] z The translation on z-axis
*/
void e_translateparticles(e_particles_t* particles,mt_scalar_t x,mt_scalar_t y,mt_scalar_t z);
/*!
  \brief Renders the given particle system
  \param[in] device The device to use
  \param[in] particles The particle system to render
*/
void e_renderparticles(e_device_t* device,const e_particles_t* particles);
/*!
  \brief Destroys the given particle system
  \param[in] particles The particle system to destroy
*/
void e_destroyparticles(e_particles_t* particles);

/*! @} */

/*! @} */

#endif
