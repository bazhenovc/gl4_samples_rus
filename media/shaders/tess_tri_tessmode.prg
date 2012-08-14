/*
	Простой шейдер, демонстрирует разные техники разбиения ребер.
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [0,+1]

uniform float	unGridScale		= 100.0;
uniform float	unMaxTessLevel	= 32.0;
uniform float	unDetailLevel	= 1000.0;

out	TVertData {
	float	fLevel;
} Output;


float Level(in vec2 pos)
{
	return clamp( distance( pos, vec2(0.5) ) * unDetailLevel, 0.1, unMaxTessLevel );
}

void main()
{
	gl_Position		= vec4( inPosition * unGridScale, 0.0, 1.0 ).xzyw;
	Output.fLevel	= Level( inPosition );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 3) out;

in	TVertData {
	float	fLevel;
} Input[];

out TContData {
	float	fLevel;
} Output[];


void main()
{
	if ( I == 0 )
	{
		gl_TessLevelInner[0] = max( max( Input[0].fLevel, Input[1].fLevel ), Input[2].fLevel );
		gl_TessLevelOuter[0] = max( Input[1].fLevel, Input[2].fLevel );
		gl_TessLevelOuter[1] = max( Input[0].fLevel, Input[2].fLevel );
		gl_TessLevelOuter[2] = max( Input[0].fLevel, Input[1].fLevel );
	}
	
	gl_out[I].gl_Position	= gl_in[I].gl_Position;
	Output[I].fLevel		= Input[I].fLevel;
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(triangles, SPACING, ccw) in;

uniform mat4	unMVPMatrix;

in TContData {
	float	fLevel;
} Input[];

out	TEvalData {
	float	fLevel;
} Output;


#define Interpolate( _a, _p ) \
	(	gl_TessCoord.x * _a[0] _p + \
		gl_TessCoord.y * _a[1] _p + \
		gl_TessCoord.z * _a[2] _p )
	
void main()
{
	gl_Position		= unMVPMatrix * Interpolate( gl_in, .gl_Position );
	Output.fLevel	= Interpolate( Input, .fLevel );
}


//-----------------------------------------------------------------------------
--fragment
#version 410 core

layout(location = 0) out vec4	outColor;
layout(location = 1) out vec4	outNormal;

in	TEvalData {
	float	fLevel;
} Input;


void main()
{
	outColor	= vec4( 1.0 );
	outNormal	= vec4( vec3(0.0), Input.fLevel );
}

--eof
