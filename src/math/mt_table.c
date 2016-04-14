#include "sys_common.h"
#include "mt_table.h"

static void* mt_getdata(mt_table_t* table,int index)
{
	return (void*)((unsigned char*)table->data+table->elementsize*index);
}

static void mt_setdata(mt_table_t* table,int index,const void* data)
{
	void* ptr=(void*)((unsigned char*)table->data+table->elementsize*index);
	memcpy(ptr,data,table->elementsize);
}

static void mt_growtable(mt_table_t* table)
{
	table->maxindices=(unsigned int)((mt_scalar_t)table->maxindices*MT_GROW_FACTOR)+1;
	table->indices=(mt_index_t*)sys_realloc(table->indices,sizeof(mt_index_t)*table->maxindices);
	table->data=(void*)sys_realloc(table->data,table->elementsize*table->maxindices);
}

mt_table_t* mt_createtable(unsigned int elementsize,unsigned int size)
{
	mt_table_t* table=(mt_table_t*)sys_alloc(sizeof(mt_table_t));

	memset(table,0,sizeof(mt_table_t));
	table->size=size;
	table->table=(int*)sys_alloc(sizeof(int)*size);
	table->elementsize=elementsize;
	mt_cleartable(table);

	return table;
}

void mt_resizetable(mt_table_t* table,unsigned int size)
{
	mt_cleartable(table);
	table->size=size;
	table->table=(int*)sys_realloc(table->table,sizeof(int)*size);
}

void mt_addelement(mt_table_t* table,const void* element,int hash)
{
	const int position=table->table[hash];
	mt_index_t* index;

	if(position>=0)
	{
		int next,current;
		mt_index_t* list;
		void* data;
		
		next=position;
		do
		{
			current=next;
			list=&table->indices[current];
			data=mt_getdata(table,list->value);

			//Test if the element has already been added
			if(!memcmp(data,element,table->elementsize))
				return;

			next=list->next;
		}
		while(next>=0);

		//Grow the table if needed
		if(table->numindices>=table->maxindices)
			mt_growtable(table);

		//Add this element
		index=&table->indices[table->numindices];
		index->value=table->numindices;
		index->next=-1;

		mt_setdata(table,table->numindices,element);

		table->indices[current].next=table->numindices;
		table->numindices++;
	}
	else
	{
		if(table->numindices>=table->maxindices)
			mt_growtable(table);

		index=&table->indices[table->numindices];
		index->value=table->numindices;
		index->next=-1;

		mt_setdata(table,table->numindices,element);

		table->table[hash]=table->numindices;
		table->numindices++;
	}
}

void mt_cleartable(mt_table_t* table)
{
	unsigned int i;
	for(i=0;i<table->size;i++)
		table->table[i]=-1;
	table->numindices=0;
}

void mt_destroytable(mt_table_t* table)
{
	if(table->indices)
		sys_free(table->indices);
	if(table->data)
		sys_free(table->data);
	sys_free(table->table);
	sys_free(table);
}
