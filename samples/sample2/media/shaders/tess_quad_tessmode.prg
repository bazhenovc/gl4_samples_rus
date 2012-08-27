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

layout(vertices = 4) out;

in	TVertData {
	float	fLevel;
} Input[];


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
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(quads, SPACING, ccw) in;

uniform mat4	unMVPMatrix;


#define Interpolate( a, p ) \
	( mix(	mix( a[0] p, a[1] p, gl_TessCoord.x ), \
			mix( a[3] p, a[2] p, gl_TessCoord.x ), \
			gl_TessCoord.y ) )
	
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
