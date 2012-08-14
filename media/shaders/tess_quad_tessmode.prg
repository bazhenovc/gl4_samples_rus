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
	return clamp( distance( pos, vec2(0.5) ) * unDetailLevel * unMaxTessLevel, 0.1, unMaxTessLevel );
}

void main()
{
	gl_Position		= vec4( inPosition * unGridScale, 0.0, 1.0 );
	Output.fLevel	= Level( inPosition );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 4) out;

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
		float	max_level = max( max( Input[0].fLevel, Input[1].fLevel ),
								 max( Input[2].fLevel, Input[3].fLevel ) );
		gl_TessLevelInner[0] = max_level;
		gl_TessLevelInner[1] = max_level;
		gl_TessLevelOuter[0] = max( Input[0].fLevel, Input[3].fLevel );
		gl_TessLevelOuter[1] = max( Input[0].fLevel, Input[1].fLevel );
		gl_TessLevelOuter[2] = max( Input[1].fLevel, Input[2].fLevel );
		gl_TessLevelOuter[3] = max( Input[2].fLevel, Input[3].fLevel );
	}
	
	gl_out[I].gl_Position	= gl_in[I].gl_Position;
	Output[I].fLevel		= Input[I].fLevel;
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(quads, SPACING, ccw) in;

uniform mat4	unMVPMatrix;

in TContData {
	float	fLevel;
} Input[];

out	TEvalData {
	float	fLevel;
} Output;


#define Interpolate( a, p ) \
	( mix(	mix( a[0] p, a[1] p, gl_TessCoord.x ), \
			mix( a[3] p, a[2] p, gl_TessCoord.x ), \
			gl_TessCoord.y ) )
	
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
