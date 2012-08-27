/*
	Простой шейдер, демонстрирует разные техники разбиения ребер.
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [0,+1]

uniform float	unGridScale;
uniform float	unMaxTessLevel;
uniform float	unDetailLevel;

out	TVertData {
	float	fLevel;
} Output;


float Level(float dist)
{
	return clamp( unDetailLevel*0.1/dist - 2.0, 0.1, unMaxTessLevel );
}

void main()
{
	gl_Position		= vec4( inPosition * unGridScale, 0.0, 1.0 );
	Output.fLevel	= Level( distance( inPosition, vec2(0.5) ) );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 3) out;

in	TVertData {
	float	fLevel;
} Input[];


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
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(triangles, SPACING, ccw) in;

uniform mat4	unMVPMatrix;

#define Interpolate( _a, _p ) \
	(	gl_TessCoord.x * _a[0] _p + \
		gl_TessCoord.y * _a[1] _p + \
		gl_TessCoord.z * _a[2] _p )
	
void main()
{
	gl_Position = unMVPMatrix * Interpolate( gl_in, .gl_Position );
}


//-----------------------------------------------------------------------------
--fragment
#version 410 core

layout(location = 0) out vec4	outColor;
layout(location = 1) out vec4	outNormal;


void main()
{
	outColor = vec4(1.0);
	outNormal = vec4(1.0);
}

--eof
