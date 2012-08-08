/*
	���������� �������� � �������������� ����������� ������� ���������� � ����� ��������.
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [-1,+1]

uniform float		unGridScale		= 100.0;
uniform float		unMaxTessLevel	= 32.0;
uniform sampler2D	unNormalMap;

out	TVertData {
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
} Output;


void main()
{
	gl_Position			= vec4( inPosition * unGridScale, 0.0, 1.0 );
	Output.vTexcoord0	= (inPosition + 1.0) * 100.0;	// for tiling
	Output.vTexcoord1	= (inPosition + 1.0) * 0.5;
	Output.fLevel		= texture( unNormalMap, Output.vTexcoord1 ).a * unMaxTessLevel;
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 4) out;


in	TVertData {
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
} Input[];

out TContData {
	vec2	vTexcoord0;
	vec2	vTexcoord1;
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
	Output[I].vTexcoord0	= Input[I].vTexcoord0;
	Output[I].Texcoord1		= Input[I].vTexcoord1;
	Output[I].fLevel		= Input[I].fLevel;
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(quads, equal_spacing, ccw) in;

uniform mat4		unMVPMatrix;
uniform sampler2D	unHeightMap;
uniform sampler2D	unNormalMap;	// normal (rgb), tess level (a)
uniform float		unHeightScale	= 10.0;

in TContData {
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
} Input[];

out	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Output;


#define Interpolate( a, p ) \
	( mix( \
		mix( a[0] p, a[1] p, gl_TessCoord.x ), \
		mix( a[3] p, a[2] p, gl_TessCoord.x ), \
		gl_TessCoord.y ) )


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
	return height * 0.1;
}	
	
void main()
{
	vec4	pos 		= Interpolate( gl_in, .gl_Position );
	Output.vTexcoord0	= Interpolate( Input, .vTexcoord0 );
	Output.fLevel		= Interpolate( Input, .fLevel );
	vec2	texc		= Interpolate( Input, .vTexcoord1 );
	Output.vNormal		= texture( unNormalMap, texc ).rgb * 2.0 - 1.0;
	
	pos.xyz += PCF( texc ) * vec3(0.0, 0.0, 1.0) * unHeightScale;
	gl_Position = unMVPMatrix * pos;
}


//-----------------------------------------------------------------------------
--fragment
#version 410 core

layout(location = 0) out vec4	outColor;
layout(location = 1) out vec4	outNormal;

uniform sampler2D	unDiffuseMap;

in	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Input;


void main(void)
{
	outColor.rgb	= texture( unDiffuseMap, Input.vTexcoord0 ).rgb;
	outColor.a		= 0.0;	// empty
	outNormal.rgb	= Input.vNormal;
	outNormal.a		= fLevel;
}

--eof