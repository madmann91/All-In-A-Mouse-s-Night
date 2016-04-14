#include "config.h"
#include "g_variables.h"
#include "g_math.h"
#include "mt_collision.h"
#include "mt_plane.h"

struct g_movinginfo_s
{
	g_character_t* character;	//Character object
	const mt_mesh_t* mesh;		//Map mesh object

	mt_vector3_t min;			//Bounding box minimum
	mt_vector3_t max;			//Bounding box maximum
	mt_scalar_t tmin;			//Minimum intersection time
	mt_vector3_t velocity;		//Scaled character velocity (tmin is between [0;1])
	mt_plane_t slide;			//Sliding plane

	int found:1;				//Collision flag
};

typedef struct g_movinginfo_s g_movinginfo_t;

static void g_charactervelocity(const mt_meshnode_t* node,g_movinginfo_t* info)
{
	//AABB rejection test
	if(info->min[0]>node->max[0]||info->max[0]<node->min[0]||
		info->min[1]>node->max[1]||info->max[1]<node->min[1]||
		info->min[2]>node->max[2]||info->max[2]<node->min[2])
		return;

	if(node->children)
	{
		g_charactervelocity(node->children,info);
		g_charactervelocity(node->children+1,info);
	}
	else
	{
		const unsigned int numtriangles=node->numindices/3;
		const mt_vector3_t* vertices=(const mt_vector3_t*)info->mesh->vertices;
		g_character_t* character=info->character;
		mt_vector3_t a,b,c,n;
		mt_plane_t plane;
		unsigned int i;
		mt_scalar_t t0,proj,dist;

		for(i=0;i<numtriangles;i++)
		{
			const unsigned int p=i*3;
			
			//See if we are going towards the triangle
			mt_triangleplane(vertices[node->indices[p]],
				vertices[node->indices[p+1]],
				vertices[node->indices[p+2]],plane);

			proj=mt_vec3dot(character->velocity,plane);
			dist=mt_planedistance(plane,character->transform.position);
			if(dist*proj>G_COLLISION_THRESHOLD)
				continue;

			//Move the triangles vertices to place the box at (0,0,0)
			mt_vec3diff(vertices[node->indices[p]],character->transform.position,a);
			mt_vec3diff(vertices[node->indices[p+1]],character->transform.position,b);
			mt_vec3diff(vertices[node->indices[p+2]],character->transform.position,c);

			//Test triangle
			if(mt_sweeptriangletobox(a,b,c,info->velocity,character->halfextents,&t0,n)&&t0<=info->tmin)
			{
				info->found=1;
				info->tmin=t0;
				mt_planecopy(info->slide,n);
			}
		}
	}
}

struct g_clipinfo_s
{
	mt_divider_t* divider;		//Divider object
	unsigned int index;			//Collider index

	mt_vector3_t min;			//Bounding box minimum
	mt_vector3_t max;			//Bounding box maximum
	mt_scalar_t tmin;			//Minimum intersection time
	mt_vector3_t invvelocity;	//Scaled inverse character velocity (tmin is between [0;1])
	mt_vector3_t normal;			//Collision normal (if any)

	int found:1;				//Collision flag
};

typedef struct g_clipinfo_s g_clipinfo_t;

static void g_clipvelocity(mt_dividernode_t* node,g_clipinfo_t* info)
{
	//We only need to consider the nodes hit by our box
	if(info->min[0]<=node->max[0]&&info->max[0]>=node->min[0]&&
		info->min[1]<=node->max[1]&&info->max[1]>=node->min[1]&&
		info->min[2]<=node->max[2]&&info->max[2]>=node->min[2])
	{
		unsigned int i;
		const mt_collider_t* collider=&info->divider->colliders[info->index];
		mt_scalar_t tmax,tmin;
		mt_vector3_t n;

		for(i=0;i<node->numcolliders;i++)
		{
			const mt_collider_t* clip=&info->divider->colliders[node->colliders[i]];

			//Do not check our own box
			if(node->colliders[i]==info->index)
				continue;

			//Clip the velocity against this collider
			if(!mt_sweepboxtobox(collider->min,collider->max,info->invvelocity,clip->min,clip->max,&tmin,&tmax,n)||tmin<-tmax/2)
				continue;

			if(tmin<info->tmin)
			{
				info->tmin=tmin;
				info->found=1;
				mt_vec3copy(info->normal,n);
			}
		}

		if(node->children)
		{
			//Continue recursion with the children
			for(i=0;i<8;i++)
				g_clipvelocity(&node->children[i],info);
		}
	}
}

void g_characterflags(g_character_t* character,unsigned int index,mt_divider_t* divider,const mt_mesh_t* mesh)
{
	const mt_vector3_t halfextents={character->halfextents[0],
		(mt_scalar_t)G_HEIGHT_LIMIT,
		character->halfextents[2]};
	mt_vector3_t position;
	
	position[0]=character->transform.position[0];
	position[1]=character->transform.position[1]-character->halfextents[1];
	position[2]=character->transform.position[2];

	character->flags&=~G_CHARACTER_CANJUMP;
	//Test the mesh against a small box placed under the character
	if(mt_boxtomesh(mesh,position,halfextents))
		character->flags|=G_CHARACTER_CANJUMP;

	if(!character->flags&G_CHARACTER_CANJUMP)
	{
		mt_vector3_t min,max;
		unsigned int i,count;
		const unsigned int* indices;

		mt_vec3sum(position,halfextents,max);
		mt_vec3diff(position,halfextents,min);

		indices=mt_boxquery(divider,&count,min,max);
		//This loop is very fast since the index is at most present only once (the "break" clause will be reached quickly)
		for(i=0;i<count;i++)
		{
			if(indices[i]!=index)
			{
				character->flags|=G_CHARACTER_CANJUMP;
				break;
			}
		}
	}
}

static void g_characterbump(g_character_t* character,const mt_vector3_t normal)
{
	const mt_scalar_t proj=mt_vec3dot(character->velocity,normal);
	const mt_scalar_t len=mt_vec3len2(normal);
	const mt_scalar_t f=-proj/len;

	//Here we make the character slide against the plane
	character->velocity[0]+=normal[0]*f;
	character->velocity[1]+=normal[1]*f;
	character->velocity[2]+=normal[2]*f;
}

#define G_INCREASEBOX(amount,min,max) \
	if(amount>0) \
		max+=amount; \
	else \
		min+=amount; \

void g_charactermove(g_character_t* character,unsigned int index,mt_divider_t* divider,const mt_mesh_t* mesh,mt_scalar_t dt)
{
	mt_scalar_t last=(mt_scalar_t)1.0;
	mt_vector3_t velocity;
	g_movinginfo_t move;
	g_clipinfo_t clip;
	unsigned int i;

	//Setup query structures
	move.character=character;
	move.mesh=mesh;
	clip.divider=divider;
	clip.index=index;

	for(i=1;i<=G_ITERATION_COUNT;i++)
	{
		const mt_scalar_t f=dt*last;

		velocity[0]=character->velocity[0]*f;
		velocity[1]=character->velocity[1]*f;
		velocity[2]=character->velocity[2]*f;

		//Recompute the character's AABB
		mt_vec3diff(character->transform.position,character->halfextents,move.min);
		mt_vec3sum(character->transform.position,character->halfextents,move.max);

		//We need to make the box fit the whole movement
		G_INCREASEBOX(velocity[0],move.min[0],move.max[0]);
		G_INCREASEBOX(velocity[1],move.min[1],move.max[1]);
		G_INCREASEBOX(velocity[2],move.min[2],move.max[2]);

		move.found=0;
		move.tmin=(mt_scalar_t)1.0;

		//We compute the opposite velocity since mt_sweeptriangletobox needs a triangle-to-box vector
		move.velocity[0]=-velocity[0];
		move.velocity[1]=-velocity[1];
		move.velocity[2]=-velocity[2];

		//Do the test recursively over the mesh
		g_charactervelocity(mesh->root,&move);

		//Clip the velocity against other characters and objects
		clip.invvelocity[0]=(mt_scalar_t)1.0/velocity[0];
		clip.invvelocity[1]=(mt_scalar_t)1.0/velocity[1];
		clip.invvelocity[2]=(mt_scalar_t)1.0/velocity[2];
		clip.found=0;
		clip.tmin=move.tmin;
		mt_vec3copy(clip.min,move.min);
		mt_vec3copy(clip.max,move.max);
		g_clipvelocity(divider->root,&clip);

		if(move.found||clip.found)
		{
			const mt_scalar_t t=clip.tmin-(mt_scalar_t)G_COLLISION_MARGIN/mt_vec3len(velocity);

			if(t>0)
			{
				mt_vector3_t min,max;

				//Move the player very close to the impact
				character->transform.position[0]+=velocity[0]*t;
				character->transform.position[1]+=velocity[1]*t;
				character->transform.position[2]+=velocity[2]*t;

				//Update the collider
				mt_vec3diff(character->transform.position,character->halfextents,min);
				mt_vec3sum(character->transform.position,character->halfextents,max);
				mt_updatecollider(divider,index,min,max);
			}

			//Update the character's velocity
			if(!clip.found)
				g_characterbump(character,move.slide);
			else
				g_characterbump(character,clip.normal);

			last-=clip.tmin;
			if(last<=MT_EPSILON)
				break;
		}
		else
		{
			mt_vector3_t min,max;

			//No collision found : leave the loop
			character->transform.position[0]+=velocity[0];
			character->transform.position[1]+=velocity[1];
			character->transform.position[2]+=velocity[2];

			//Update the collider
			mt_vec3diff(character->transform.position,character->halfextents,min);
			mt_vec3sum(character->transform.position,character->halfextents,max);
			mt_updatecollider(divider,index,min,max);
			break;
		}
	}
}

void g_charactercamera(mt_vector3_t position,mt_vector3_t destination,const g_character_t* character,const mt_mesh_t* mesh)
{
	mt_vector3_t start,dir;
	mt_scalar_t t0;

	//Setup ray parameters
	start[0]=character->transform.position[0];
	start[1]=character->transform.position[1]+character->halfextents[1];
	start[2]=character->transform.position[2];

	dir[0]=-mt_cos(character->heading);
	dir[1]=-mt_sin(character->pitch);
	dir[2]=-mt_sin(character->heading);

	t0=g_distance;
	//Test mesh
	mt_raytomesh(mesh,start,dir,&t0);

	//Camera threshold : prevents the player's model from being invisible
	if(t0<G_CAMERA_THRESHOLD)
		t0=0;

	//Update camera position and destination (the point we're looking at)
	destination[0]=start[0]+dir[0]*(mt_scalar_t)(-G_CAMERA_DISTANCE);
	destination[1]=start[1]+dir[1]*(mt_scalar_t)(-G_CAMERA_DISTANCE);
	destination[2]=start[2]+dir[2]*(mt_scalar_t)(-G_CAMERA_DISTANCE);

	position[0]=start[0]+dir[0]*t0;
	position[1]=start[1]+dir[1]*t0;
	position[2]=start[2]+dir[2]*t0;
}
