/***********************************************************
*                                                          *
*  File : mt_tables.h                                      *
*  Date : 20/02/2011                                       *
*  Comments : Generic hash table                           *
*                                                          *
************************************************************/

#ifndef __MT_TABLES_H__
#define __MT_TABLES_H__

#include "mt_common.h"
#include "mt_vector.h"

/*! \addtogroup mt */
/*! @{ */

/*! \name Tables */
/*! @{ */

#define MT_GROW_FACTOR ((mt_scalar_t)1.2)

/*! An index structure */
struct mt_index_s
{
	int next;				/*!< The next index, or -1 */
	unsigned int value;		/*!< The index value */
};

typedef struct mt_index_s mt_index_t;

/*! The table structure */
struct mt_table_s
{
	int* table;					/*!< The table of indices */
	unsigned int size;			/*!< The size of the table array */

	mt_index_t* indices;		/*!< The index array */
	unsigned int numindices;	/*!< The number of pairs */
	unsigned int maxindices;	/*!< The maximum number of pairs */

	void* data;					/*!< The user data */
	unsigned int elementsize;	/*!< The size of each element (in bytes) */
};

typedef struct mt_table_s mt_table_t;

/*!
  \brief Creates a hash table
  \param[in] elementsize The size of each element in the table
  \param[in] size The size of the index entries 
  The parameter 'size' should be a prime number.
*/
mt_table_t* mt_createtable(unsigned int elementsize,unsigned int size);
/*!
  Resizes a table and destroys its contents
  \param[in] table The table to resize
  \param[in] size The new table size
*/
void mt_resizetable(mt_table_t* table,unsigned int size);
/*!
  \brief Adds an element to a table
  \param[in] table The table which is to receive the new element
  \param[in] element The corresponding element
  \param[in] hash The hash value to use
*/
void mt_addelement(mt_table_t* table,const void* element,int hash);
/*!
  \brief Clears a table
  \param table The table to cleared
*/
void mt_cleartable(mt_table_t* table);
/*!
  \brief Destroys a table
  \param[in] table The table to be destroyed
*/
void mt_destroytable(mt_table_t* table);

/*! @} */

/*! @} */

#endif
