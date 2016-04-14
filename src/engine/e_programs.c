#include "config.h"
#include "e_device.h"
#include "e_programs.h"

unsigned int e_programids[E_MAX_PROGRAMS];
const char* e_programs[E_MAX_PROGRAMS]={
//E_PROGRAM_PASSTOFRAGMENT
"!!ARBvp1.0"
"PARAM mvp[4]={state.matrix.mvp};"
"ATTRIB iPos=vertex.position;"
"ATTRIB iCol=vertex.color;"
"ATTRIB iNorm=vertex.normal;"
"ATTRIB iTex0=vertex.texcoord[0];"
"OUTPUT oPos=result.position;"
"OUTPUT oCol=result.color;"
"OUTPUT oTex0=result.texcoord[0];"
"OUTPUT oTex1=result.texcoord[1];"
"OUTPUT oTex2=result.texcoord[2];"
//Output position
"DP4 oPos.x,mvp[0],iPos;"
"DP4 oPos.y,mvp[1],iPos;"
"DP4 oPos.z,mvp[2],iPos;"
"DP4 oPos.w,mvp[3],iPos;"
//Output texture coordinate and color
"MOV oTex0,iTex0;"
"MOV oCol,iCol;"
//Here position and normal go as texture coordinates 1 and 2
"MOV oTex1,iPos;"
"MOV oTex2,iNorm;"
"END"
,//E_PROGRAM_NOSHADING
"!!ARBfp1.0"
"PARAM mvp[4]={state.matrix.mvp};"
"PARAM hasTexture=program.env[0];"		//(1.0 1.0 1.0 1.0) if you want texturing, otherwise (0.0 0.0 0.0 0.0)
"PARAM colorBias={0.2,0.2,0.2,0.2};"
"ATTRIB iTex0=fragment.texcoord[0];"
"ATTRIB iCol=fragment.color;"
"OUTPUT oCol=result.color;"
"TEMP texColor,fragColor;"
//Compute texture color
"TEX texColor,iTex0,texture[0],2D;"
//Compute fragment color
"LRP fragColor,hasTexture,texColor,iCol;"
"MUL oCol,fragColor,colorBias;"
"END"
,//E_PROGRAM_CELSHADING
"!!ARBfp1.0"
"PARAM mvp[4]={state.matrix.mvp};"
"PARAM hasTexture=program.env[0];"			//(1.0 1.0 1.0 1.0) if you want texturing, otherwise (0.0 0.0 0.0 0.0)
"PARAM lightPosition=program.env[1];"		//Light source position
"PARAM lightAttenuation={0.5,0.1,0.001,0.0};"	//Light source attenuation
"ATTRIB iTex0=fragment.texcoord[0];"
"ATTRIB iCol=fragment.color;"
"ATTRIB iPos=fragment.texcoord[1];"
"ATTRIB iNorm=fragment.texcoord[2];"
"OUTPUT oCol=result.color;"
"TEMP lightVector,lightDist2,invLightDist;"
"TEMP texCoord,texColor,celColor,fragColor;"
"TEMP mulFactor,distVector;"
//Compute light contribution
"SUB lightVector,lightPosition,iPos;"
"DP3 lightDist2,lightVector,lightVector;"
"RSQ invLightDist,lightDist2.w;"
"MUL lightVector,invLightDist,lightVector;"
"DP3 texCoord.x,lightVector,iNorm;"
"MAX texCoord.x,texCoord.x,0;"
//Compute texture color
"TEX texColor,iTex0,texture[0],2D;"
"LRP texColor,hasTexture,texColor,iCol;"
//Compute shading color
"TEX celColor,texCoord,texture[1],1D;"
//Compute light attenuation
"DST distVector,lightDist2,invLightDist;"
"DP3 mulFactor,lightAttenuation,distVector;"
"RCP mulFactor,mulFactor.x;"
//Compute fragment color
"MUL fragColor,celColor,texColor;"
"MUL oCol,fragColor,mulFactor;"
"END"
,//E_PROGRAM_SHADOW
"!!ARBvp1.0"
"PARAM mvp[4]={state.matrix.mvp};"
"PARAM lightPosition=program.env[0];"
"ATTRIB iPos=vertex.position;"
"OUTPUT oPos=result.position;"
"TEMP temp0;"
"TEMP temp1;"
"TEMP position;"
//Extrude if iPos.w=0
"SWZ temp0,iPos,w,w,w,1;"
"SLT temp1,temp0,1;"
"MUL temp0,lightPosition,temp1;"
//Here temp0={lightPosition.x,lightPosition.y,lightPosition.z,0} if iPos.w<1
//And temp0={0,0,0,0} if iPos.w=1
"SUB position,iPos,temp0;"
"DP4 oPos.x,mvp[0],position;"
"DP4 oPos.y,mvp[1],position;"
"DP4 oPos.z,mvp[2],position;"
"DP4 oPos.w,mvp[3],position;"
"END"
};

void e_registerprograms(void)
{
	unsigned int i;

	glGenProgramsARB(E_MAX_PROGRAMS,e_programids);
	for(i=0;i<E_MAX_PROGRAMS;i++)
	{
#ifdef DEBUG
		int error,native;
		const char* type;
#endif

		if(!strncmp(e_programs[i],"!!ARBvp1.0",10))
		{
			//Assuming no errors are made in the source code (Well I've already checked it...)
			glBindProgramARB(GL_VERTEX_PROGRAM_ARB,e_programids[i]);
			glProgramStringARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(e_programs[i]),e_programs[i]);
#ifdef DEBUG
			type="GL_ARB_VERTEX_PROGRAM";
			glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB,&native);
#endif
		}
		else if(!strncmp(e_programs[i],"!!ARBfp1.0",10))
		{
			//Assuming no errors are made in the source code (Well I've already checked it...)
			glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,e_programids[i]);
			glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(e_programs[i]),e_programs[i]);
#ifdef DEBUG
			type="GL_ARB_FRAGMENT_PROGRAM";
			glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB,&native);
#endif
		}

#ifdef DEBUG
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB,&error);
		if(error!=-1)
			printf("Program %d, %s error : %s\n",i,type,glGetString(GL_PROGRAM_ERROR_STRING_ARB));
		if(!native)
			printf("Program %d, %s warning : Program not compiled as native\n",i,type);
#endif
	}
}

void e_unregisterprograms(void)
{
	glDeleteProgramsARB(E_MAX_PROGRAMS,e_programids);
}
