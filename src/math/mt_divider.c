#include "sys_common.h"
#include "mt_divider.h"
#include "mt_primes.h"
#include "mt_collision.h"

static void mt_dividenode(mt_dividernode_t* node,unsigned int depth)
{
	const mt_vector3_t middle={(node->max[0]+node->min[0])/2,
		(node->max[1]+node->min[1])/2,
		(node->max[2]+node->min[2])/2};
	mt_dividernode_t* child;
	mt_vector3_t min,max;
	int i,j,k;

	node->colliders=0;
	node->numcolliders=0;
	node->maxcolliders=0;

	//Do not subdivide more than needed
	if(depth<=0)
	{
		node->children=0;
		return;
	}

	node->children=sys_alloc(sizeof(mt_dividernode_t)*8);

	//Subdivide this node between 8 children
	for(i=0;i<2;i++)
	{
		min[0]=(i==0)?node->min[0]:middle[0];
		max[0]=(i==0)?middle[0]:node->max[0];
		for(j=0;j<2;j++)
		{
			min[1]=(j==0)?node->min[1]:middle[1];
			max[1]=(j==0)?middle[1]:node->max[1];
			for(k=0;k<2;k++)
			{
				min[2]=(k==0)?node->min[2]:middle[2];
				max[2]=(k==0)?middle[2]:node->max[2];

				child=&node->children[(i<<2)+(j<<1)+k];
				mt_vec3copy(child->min,min);
				mt_vec3copy(child->max,max);
				mt_dividenode(child,depth-1);
			}
		}
	}
}

mt_divider_t* mt_createdivider(unsigned int depth,const mt_vector3_t min,const mt_vector3_t max)
{
	mt_divider_t* divider=sys_alloc(sizeof(mt_divider_t));

	memset(divider,0,sizeof(mt_divider_t));
	
	divider->root=sys_alloc(sizeof(mt_dividernode_t));
	memset(divider->root,0,sizeof(mt_dividernode_t));
	mt_vec3copy(divider->root->min,min);
	mt_vec3copy(divider->root->max,max);
	divider->depth=depth;

	//Start subdivision
	mt_dividenode(divider->root,depth);

	return divider;
}

static void mt_addcollider(mt_dividernode_t* node,unsigned int index)
{
	unsigned int i;
	
	//Grow this array if needed
	if(node->numcolliders>=node->maxcolliders)
	{
		node->maxcolliders=node->maxcolliders*2+1;
		node->colliders=sys_realloc(node->colliders,node->maxcolliders*sizeof(unsigned int));
	}

	//Insert sort the new collider in the array
	for(i=node->numcolliders;i>0&&node->colliders[i-1]>index;i--)
		node->colliders[i]=node->colliders[i-1];

	node->colliders[i]=index;
	node->numcolliders++;
}

static int mt_findcollider(mt_dividernode_t* node,unsigned int index)
{
	int min=0,max=node->numcolliders-1;
	int middle;

	if(!node->numcolliders)
		return -1;

	//A simple bisection to find the position of the collider
	while(max-min>1)
	{
		middle=(max+min)/2;
		if(node->colliders[middle]>index)
			max=middle;
		else
			min=middle;
	}

	if(node->colliders[max]==index)
		return max;
	if(node->colliders[min]==index)
		return min;

	//Return -1 on error (not found)
	return -1;
}

static void mt_removecollider(mt_dividernode_t* node,unsigned int position)
{
	unsigned int i;
	node->numcolliders--;
	for(i=position;i<node->numcolliders;i++)
		node->colliders[i]=node->colliders[i+1];
}

static void mt_clearnode(mt_dividernode_t* node)
{
	//Clear this node if needed
	if(node->children&&node->numcolliders>0)
	{
		int i;
		for(i=0;i<8;i++)
			mt_clearnode(&node->children[i]);
	}

	node->numcolliders=0;
}

void mt_resizedivider(mt_divider_t* divider,unsigned int numcolliders)
{
	int prime,factor;
	unsigned int i;

	//Reallocate colliders
	divider->colliders=sys_realloc(divider->colliders,sizeof(mt_collider_t)*numcolliders);
	divider->numcolliders=numcolliders;

	factor=numcolliders/4;
	prime=mt_nextprime(numcolliders/4+1);

	//Recursively clear the divider
	mt_clearnode(divider->root);

	//We have removed all the colliders from the divider, we need to set the node pointers to 0
	for(i=0;i<divider->numcolliders;i++)
		divider->colliders[i].node=0;
}

enum MT_NODE_PLACES
{
	MT_PLACE_NOTFOUND,
	MT_PLACE_THISNODE,
	MT_PLACE_CHILDREN
};

static int mt_placecollider(mt_dividernode_t* node,mt_collider_t* collider,unsigned int index,const mt_vector3_t min,const mt_vector3_t max)
{
	//The collider must be completely contained in the node
	if(min[0]>node->min[0]&&max[0]<node->max[0]&&
		min[1]>node->min[1]&&max[1]<node->max[1]&&
		min[2]>node->min[2]&&max[2]<node->max[2])
	{
		//First we try to add it to the children
		if(node->children)
		{
			int i;
			
			for(i=0;i<8;i++)
			{
				//We do not add the collider to this node if it has found a better place elsewhere
				if(mt_placecollider(&node->children[i],collider,index,min,max)!=MT_PLACE_NOTFOUND)
					return MT_PLACE_CHILDREN;
			}
		}

		//All the children refuse to place this collider, then the collider must belong to this node
		if(mt_findcollider(node,index)<0)
			mt_addcollider(node,index);

		//Setup the node pointer in the collider structure
		collider->node=node;

		return MT_PLACE_THISNODE;
	}

	return MT_PLACE_NOTFOUND;
}

void mt_updatecollider(mt_divider_t* divider,unsigned int index,const mt_vector3_t min,const mt_vector3_t max)
{
	mt_collider_t* collider=&divider->colliders[index];
	mt_dividernode_t* node=collider->node;

	//The collider may not be placed yet in the tree
	if(node)
	{
		int place=mt_placecollider(node,collider,index,min,max);
		//Are we still in this node ?
		if(place!=MT_PLACE_NOTFOUND)
		{
			//Do we need to remove the old collider ?
			if(place!=MT_PLACE_THISNODE)
			{
				//The collider has moved to the children of this node, we must remove it
				int position=mt_findcollider(node,index);
				if(position>=0)
					mt_removecollider(node,(unsigned int)position);
			}
		}
		else
		{
			//We are not in this node (nor in its children)...
			//We must remove our collider from it and restart from the root node
			int position=mt_findcollider(node,index);
			if(position>=0)
				mt_removecollider(node,(unsigned int)position);

			mt_placecollider(divider->root,collider,index,min,max);
		}
	}
	else
	{
		//This collider didn't belong to any node yet... no need to remove any old collider
		mt_placecollider(divider->root,collider,index,min,max);
	}
	mt_vec3copy(collider->min,min);
	mt_vec3copy(collider->max,max);
}

static void mt_addindex(mt_divider_t* divider,unsigned int index)
{
	if(divider->numquery>=divider->maxquery)
	{
		divider->maxquery=2*divider->maxquery+1;
		divider->query=sys_realloc(divider->query,sizeof(unsigned int)*divider->maxquery);
	}

	divider->query[divider->numquery]=index;
	divider->numquery++;
}

static void mt_raywalk(const mt_dividernode_t* node,mt_divider_t* divider,const mt_vector3_t start,const mt_vector3_t invdir)
{
	mt_scalar_t t0,t1;

	//Ray test this node
	if(mt_raytobox(start,invdir,node->min,node->max,&t0,&t1)&&t1>0)
	{
		unsigned int i;
		mt_collider_t* collider;

		//Ray-test all the colliders in this node
		for(i=0;i<node->numcolliders;i++)
		{
			const unsigned int index=node->colliders[i];
			collider=&divider->colliders[index];
			if(!mt_raytobox(start,invdir,collider->min,collider->max,&t0,&t1)||t0>1||t1<0)
				continue;
			mt_addindex(divider,index);
		}

		if(node->children)
		{
			//Ray-test every child
			for(i=0;i<8;i++)
				mt_raywalk(&node->children[i],divider,start,invdir);
		}
	}
}

const unsigned int* mt_rayquery(mt_divider_t* divider,unsigned int* count,const mt_vector3_t start,const mt_vector3_t direction)
{
	const mt_vector3_t invdir={(mt_scalar_t)1.0/direction[0],
		(mt_scalar_t)1.0/direction[1],
		(mt_scalar_t)1.0/direction[2]};

	divider->numquery=0;
	mt_raywalk(divider->root,divider,start,invdir);
	*count=divider->numquery;
	return (divider->numquery>0)?(const unsigned int*)divider->query:0;
}

static void mt_boxwalk(const mt_dividernode_t* node,mt_divider_t* divider,const mt_vector3_t min,const mt_vector3_t max)
{
	//Test this node against the box
	if(min[0]<=node->max[0]&&max[0]>=node->min[0]&&
		min[1]<=node->max[1]&&max[1]>=node->min[1]&&
		min[2]<=node->max[2]&&max[2]>=node->min[2])
	{
		unsigned int i;
		mt_collider_t* collider;

		for(i=0;i<node->numcolliders;i++)
		{
			const unsigned int index=node->colliders[i];
			collider=&divider->colliders[index];

			if(min[0]>collider->max[0]||max[0]<collider->min[0]||
				min[1]>collider->max[1]||max[1]<collider->min[1]||
				min[2]>collider->max[2]||max[2]<collider->min[2])
				continue;

			mt_addindex(divider,index);
		}

		if(node->children)
		{
			for(i=0;i<8;i++)
				mt_boxwalk(&node->children[i],divider,min,max);
		}
	}
}

const unsigned int* mt_boxquery(mt_divider_t* divider,unsigned int* count,const mt_vector3_t min,const mt_vector3_t max)
{
	divider->numquery=0;
	mt_boxwalk(divider->root,divider,min,max);
	*count=divider->numquery;
	return (divider->numquery>0)?(const unsigned int*)divider->query:0;
}

static void mt_frustumwalk(const mt_dividernode_t* node,mt_divider_t* divider,const mt_plane_t planes[6])
{
	//Test this node against the box
	if(mt_frustumtobox(planes,node->min,node->max))
	{
		unsigned int i;
		mt_collider_t* collider;

		//Test all the colliders
		for(i=0;i<node->numcolliders;i++)
		{
			const unsigned int index=node->colliders[i];
			collider=&divider->colliders[index];
			if(!mt_frustumtobox(planes,collider->min,collider->max))
				continue;
			mt_addindex(divider,index);
		}

		if(node->children)
		{
			for(i=0;i<8;i++)
				mt_frustumwalk(&node->children[i],divider,planes);
		}
	}
}

const unsigned int* mt_frustumquery(mt_divider_t* divider,unsigned int* count,const mt_plane_t planes[6])
{
	divider->numquery=0;
	mt_frustumwalk(divider->root,divider,planes);
	*count=divider->numquery;
	return (divider->numquery>0)?(const unsigned int*)divider->query:0;
}

static void mt_destroynode(mt_dividernode_t* node)
{
	if(node->colliders)
		sys_free(node->colliders);

	if(node->children)
	{
		int i;

		//Destroy children
		for(i=0;i<8;i++)
			mt_destroynode(&node->children[i]);

		sys_free(node->children);
	}
}

void mt_destroydivider(mt_divider_t* divider)
{
	if(divider->query)
		sys_free(divider->query);
	mt_destroynode(divider->root);
	sys_free(divider->root);
	if(divider->colliders)
		sys_free(divider->colliders);
	sys_free(divider);
}
