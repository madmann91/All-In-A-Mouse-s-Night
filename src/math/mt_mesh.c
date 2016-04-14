#include "sys_common.h"
#include "mt_mesh.h"
#include "mt_collision.h"

struct mt_treeinfo_s
{
	mt_scalar_t minratio;
	unsigned int minindices;
	unsigned int maxdepth;
	const mt_vector3_t* vertices;
};

typedef struct mt_treeinfo_s mt_treeinfo_t;

struct mt_boxquery_s
{
	mt_vector3_t min,max;
	mt_vector3_t halfextents;
	mt_vector3_t position;
	const mt_vector3_t* vertices;
	int found:1;
};

typedef struct mt_boxquery_s mt_boxquery_t;

struct mt_rayquery_s
{
	const mt_vector3_t start;
	const mt_vector3_t dir;
	const mt_vector3_t invdir;
	const mt_vector3_t* vertices;
	int found:1;
	mt_scalar_t t0;
};

typedef struct mt_rayquery_s mt_rayquery_t;

static void mt_refitnode(mt_meshnode_t* node,const mt_vector3_t* vertices)
{
	if(node->children)
	{
		mt_meshnode_t* left=node->children;
		mt_meshnode_t* right=node->children+1;

		//Refit this node with the children's bounding boxes
		mt_refitnode(left,vertices);
		mt_refitnode(right,vertices);

		node->min[0]=(left->min[0]<right->min[0])?left->min[0]:right->min[0];
		node->min[1]=(left->min[1]<right->min[1])?left->min[1]:right->min[1];
		node->min[2]=(left->min[2]<right->min[2])?left->min[2]:right->min[2];		

		node->max[0]=(left->max[0]>right->max[0])?left->max[0]:right->max[0];
		node->max[1]=(left->max[1]>right->max[1])?left->max[1]:right->max[1];
		node->max[2]=(left->max[2]>right->max[2])?left->max[2]:right->max[2];	
	}
	else
	{
		unsigned int i;

		mt_vec3copy(node->min,vertices[node->indices[0]]);
		mt_vec3copy(node->max,vertices[node->indices[0]]);

		//Find the minimum and maximum values
		for(i=0;i<node->numindices;i++)
		{	
			mt_vec3min(node->min,vertices[node->indices[i]]);
			mt_vec3max(node->max,vertices[node->indices[i]]);
		}
	}
}

static unsigned int mt_buildnode(mt_meshnode_t* node,const mt_treeinfo_t* info)
{
	const unsigned int numtriangles=node->numindices/3;
	unsigned int axis=0;
	mt_vector3_t extents;
	mt_scalar_t middle;
	unsigned int i;
	unsigned int depth[2];
	mt_meshnode_t* left,*right;

	node->children=0;

	mt_refitnode(node,info->vertices);

	//Ensure that the tree is no too big
	if(node->depth>=info->maxdepth)
		return node->depth;

	//Find best axis
	mt_vec3diff(node->max,node->min,extents);
	if(extents[1]>extents[0])
		axis=1;
	if(extents[axis]<extents[2])
		axis=2;

	//Find the middle point on this axis
	middle=0;
	for(i=0;i<numtriangles;i++)
	{
		const unsigned int p=i*3;
		middle+=(info->vertices[node->indices[p]][axis]+
			info->vertices[node->indices[p+1]][axis]+
			info->vertices[node->indices[p+2]][axis]);
	}
	middle/=(mt_scalar_t)numtriangles;

	//Allocate memory for the node's children
	node->children=(mt_meshnode_t*)sys_alloc(sizeof(mt_meshnode_t)*2);
	left=node->children;
	right=node->children+1;

	//Initialise children
	memset(left,0,sizeof(mt_meshnode_t));
	memset(right,0,sizeof(mt_meshnode_t));

	left->depth=right->depth=node->depth+1;
	left->indices=(unsigned int*)sys_alloc(sizeof(unsigned int)*node->numindices);
	right->indices=(unsigned int*)sys_alloc(sizeof(unsigned int)*node->numindices);

	//Classify all the triangles
	for(i=0;i<numtriangles;i++)
	{
		const unsigned int p=i*3;
		const unsigned int a=node->indices[p];
		const unsigned int b=node->indices[p+1];
		const unsigned int c=node->indices[p+2];

		if((info->vertices[a][axis]+info->vertices[b][axis]+info->vertices[c][axis])<middle)
		{
			left->indices[left->numindices]=a;
			left->indices[left->numindices+1]=b;
			left->indices[left->numindices+2]=c;
			left->numindices+=3;
		}
		else
		{
			right->indices[right->numindices]=a;
			right->indices[right->numindices+1]=b;
			right->indices[right->numindices+2]=c;
			right->numindices+=3;
		}
	}

	//Test if the split was useful
	if((mt_scalar_t)right->numindices/(mt_scalar_t)node->numindices<info->minratio||
		(mt_scalar_t)left->numindices/(mt_scalar_t)node->numindices<info->minratio||
		right->numindices<info->minindices||
		left->numindices<info->minindices)
	{
		sys_free(left->indices);
		sys_free(right->indices);
		sys_free(node->children);
		node->children=0;
		return node->depth;
	}

	//The current node is a parent node, it doesn't need any list of indices anymore
	if(node->depth!=0)
		sys_free(node->indices);

	//Resize lists
	left->indices=(unsigned int*)sys_realloc(left->indices,sizeof(unsigned int)*left->numindices);
	right->indices=(unsigned int*)sys_realloc(right->indices,sizeof(unsigned int)*right->numindices);

	//Continue recursion
	depth[0]=mt_buildnode(left,info);
	depth[1]=mt_buildnode(right,info);
	if(depth[0]>depth[1])
		return depth[0];
	return depth[1];
}

static void mt_boxtonode(const mt_meshnode_t* node,mt_boxquery_t* query)
{
	//AABB against AABB test
	if(query->found||query->min[0]>node->max[0]||query->max[0]<node->min[0]||
		query->min[1]>node->max[1]||query->max[1]<node->min[1]||
		query->min[2]>node->max[2]||query->max[2]<node->min[2])
		return;

	if(node->children)
	{
		mt_boxtonode(node->children,query);
		mt_boxtonode(node->children+1,query);
	}
	else
	{
		const unsigned int numtriangles=node->numindices/3;
		mt_vector3_t a,b,c;
		unsigned int i;

		//Test this leaf
		for(i=0;i<numtriangles;i++)
		{
			const unsigned int p=i*3;

			//Transform these vertices in box space
			mt_vec3diff(query->vertices[node->indices[p]],query->position,a);
			mt_vec3diff(query->vertices[node->indices[p+1]],query->position,b);
			mt_vec3diff(query->vertices[node->indices[p+2]],query->position,c);

			if(mt_triangletobox(a,b,c,query->halfextents))
			{
				query->found=1;
				break;
			}
		}
	}
}

static void mt_raytonode(const mt_meshnode_t* node,mt_rayquery_t* query)
{
	mt_scalar_t t0,t1;
	//Test this node
	if(!mt_raytobox(query->start,query->invdir,node->min,node->max,&t0,&t1))
		return;

	//Ensure we're in bounds
	if(t0>query->t0||t1<0)
		return;

	if(node->children)
	{
		//Continue recursion
		mt_raytonode(node->children,query);
		mt_raytonode(node->children+1,query);
	}
	else
	{
		const unsigned int numtriangles=node->numindices/3;
		unsigned int i;
		mt_scalar_t t0;

		//Test this leaf
		for(i=0;i<numtriangles;i++)
		{
			const unsigned int p=i*3;

			if(mt_raytotriangle(query->start,query->dir,
				query->vertices[node->indices[p]],
				query->vertices[node->indices[p+1]],
				query->vertices[node->indices[p+2]],&t0))
			{
				//Test if the intersection is in bounds
				if(t0>0&&t0<query->t0)
				{
					query->found=1;
					query->t0=t0;
				}
			}
		}
	}
}

static void mt_destroynode(mt_meshnode_t* node)
{
	if(node->children)
	{
		mt_destroynode(node->children);
		mt_destroynode(node->children+1);
		sys_free(node->children);
	}
	else
	{
		//The tree may have no depth at all
		if(node->depth>0)
			sys_free(node->indices);
	}
}

mt_mesh_t* mt_createmesh(unsigned int* indices,unsigned int numindices,const mt_scalar_t* vertices)
{
	mt_mesh_t* mesh=(mt_mesh_t*)sys_alloc(sizeof(mt_mesh_t));
	mt_treeinfo_t info;

	mesh->root=(mt_meshnode_t*)sys_alloc(sizeof(mt_meshnode_t));
	memset(mesh->root,0,sizeof(mt_meshnode_t));
	mesh->root->indices=indices;
	mesh->root->numindices=numindices;
	mesh->vertices=vertices;

	info.vertices=(const mt_vector3_t*)vertices;
	info.minratio=(mt_scalar_t)0.2;	//Left/Right triangle ratio
	info.minindices=9; //Minimum 3 triangles per leaf
	info.maxdepth=(unsigned int)(mt_log((mt_scalar_t)numindices)*((mt_scalar_t)1.0/MT_LN2)); //Maximum recursion depth

	mesh->depth=mt_buildnode(mesh->root,&info);

	return mesh;
}

void mt_refitmesh(mt_mesh_t* mesh,const mt_scalar_t* vertices)
{
	mesh->vertices=vertices;
	mt_refitnode(mesh->root,(const mt_vector3_t*)vertices);
}

int mt_boxtomesh(const mt_mesh_t* mesh,const mt_vector3_t position,const mt_vector3_t halfextents)
{
	mt_boxquery_t boxquery;

	//Setup the query structure
	mt_vec3diff(position,halfextents,boxquery.min);
	mt_vec3sum(position,halfextents,boxquery.max);
	mt_vec3copy(boxquery.halfextents,halfextents);
	mt_vec3copy(boxquery.position,position);
	boxquery.vertices=(const mt_vector3_t*)mesh->vertices;

	boxquery.found=0;
	mt_boxtonode(mesh->root,&boxquery);

	return boxquery.found;
}

int mt_raytomesh(const mt_mesh_t* mesh,const mt_vector3_t start,const mt_vector3_t dir,mt_scalar_t* t0)
{
	mt_rayquery_t query={{start[0],start[1],start[2]},
		{dir[0],dir[1],dir[2]},
		{(mt_scalar_t)1.0/dir[0],(mt_scalar_t)1.0/dir[1],(mt_scalar_t)1.0/dir[2]},
		(const mt_vector3_t*)mesh->vertices,0,*t0};

	mt_raytonode(mesh->root,&query);

	if(query.found)
		*t0=query.t0;

	return query.found;
}

void mt_destroymesh(mt_mesh_t* mesh)
{
	mt_destroynode(mesh->root);
	sys_free(mesh->root);
	sys_free(mesh);
}
