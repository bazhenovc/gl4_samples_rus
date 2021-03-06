/*
	������� ���������� �������� � ����������� �� ������� �������� �� ������.
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [0,+1]

uniform float		unGridScale;
uniform float		unMaxTessLevel;
uniform float		unHeightScale;
uniform mat4		unMVPMatrix;
uniform sampler2D	unHeightMap;

out	TVertData {
	vec2	vScrCoords;	// position in screen
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
} Output;


void main()
{
	gl_Position			= vec4( inPosition * unGridScale, 0.0, 1.0 ).xzyw;
	Output.vNormal		= vec3( 0.0, 1.0, 0.0 );
	Output.vTexcoord0	= inPosition * 100.0;	// for tiling
	Output.vTexcoord1	= inPosition;
	vec4	pos			= unMVPMatrix * vec4( gl_Position.xyz +
						  texture( unHeightMap, Output.vTexcoord1 ).r *
						  Output.vNormal * unHeightScale, 1.0 );
	Output.vScrCoords	= pos.xy / pos.w;
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 3) out;

uniform float	unMaxTessLevel;
uniform float	unDetailLevel;

in	TVertData {
	vec2	vScrCoords;	// position in screen
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
} Input[];

out TContData {
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
} Output[];


float Level(in vec2 p0, in vec2 p1)
{
	return clamp( distance( p0, p1 ) * unDetailLevel * 4.0, 0.1, unMaxTessLevel );
}

void main()
{
	if ( I == 0 )
	{
		gl_TessLevelOuter[0] = Level( Input[1].vScrCoords, Input[2].vScrCoords );
		gl_TessLevelOuter[1] = Level( Input[0].vScrCoords, Input[2].vScrCoords );
		gl_TessLevelOuter[2] = Level( Input[0].vScrCoords, Input[1].vScrCoords );
		gl_TessLevelInner[0] = max( max( gl_TessLevelOuter[0], gl_TessLevelOuter[1] ),
										 gl_TessLevelOuter[2] );
	}
	
	gl_out[I].gl_Position	= gl_in[I].gl_Position;
	Output[I].vNormal		= Input[I].vNormal;
	Output[I].vTexcoord0	= Input[I].vTexcoord0;
	Output[I].vTexcoord1	= Input[I].vTexcoord1;
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(triangles, fractional_even_spacing, ccw) in;

uniform mat4		unMVPMatrix;
uniform sampler2D	unHeightMap;
uniform sampler2D	unNormalMap;
uniform float		unHeightScale;

in TContData {
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
} Input[];

out	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Output;


#define Interpolate( _a, _p ) \
	(	gl_TessCoord.x * _a[0] _p + \
		gl_TessCoord.y * _a[1] _p + \
		gl_TessCoord.z * _a[2] _p )

float PCF(in vec2 vTexcoord)
{
	float	height = 0.0;
	height += textureOffset( unHeightMap, vTexcoord, ivec2(-1,-1) ).r;
	height += textureOffset( unHeightMap, vTexcoord, ivec2(-1, 0) ).r;
	height += textureOffset( unHeightMap, vTexcoord, ivec2(-1, 1) ).r;
	height += textureOffset( unHeightMap, vTexcoord, ivec2( 0,-1) ).r;
	height += textureOffset( unHeightMap, vTexcoord, ivec2( 0, 0) ).r * 2.0;
	height += textureOffset( unHeightMap, vTexcoord, ivec2( 0, 1) ).r;
	height += textureOffset( unHeightMap, vTexcoord, ivec2( 1,-1) ).r;
	height += textureOffset( unHeightMap, vTexcoord, ivec2( 1, 0) ).r;
	height += textureOffset( unHeightMap, vTexcoord, ivec2( 1, 1) ).r;
	return ( height * 0.1 );
}
	
void main()
{
	vec4	pos 		= Interpolate( gl_in, .gl_Position );
	vec3	norm 		= Interpolate( Input, .vNormal );
	Output.vTexcoord0	= Interpolate( Input, .vTexcoord0 );
	Output.fLevel		= gl_TessLevelOuter[2] * gl_TessCoord.x +
						  gl_TessLevelOuter[0] * gl_TessCoord.y +
						  gl_TessLevelOuter[1] * gl_TessCoord.z;
	vec2	texc		= Interpolate( Input, .vTexcoord1 );
	Output.vNormal		= normalize( texture( unNormalMap, texc ).rbg * 2.0 - 1.0 );
	
	pos.xyz += PCF( texc ) * norm * unHeightScale;
	gl_Position = unMVPMatrix * pos;
}


//-----------------------------------------------------------------------------
--fragment
#version 410 core

layout(location = 0) out vec4	outColor;
layout(location = 1) out vec4	outNormal;

uniform sampler2D	unDiffuseMap;
uniform float		unMaxTessLevel;

in	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Input;


void main()
{
	outColor.rgb	= texture( unDiffuseMap, Input.vTexcoord0 ).rgb;
	outColor.a		= 0.0;	// empty
	outNormal.rgb	= Input.vNormal * 0.5 + 0.5;
	outNormal.a		= 1.0 - clamp( Input.fLevel, 0.0, unMaxTessLevel ) / unMaxTessLevel;
}

--eof
