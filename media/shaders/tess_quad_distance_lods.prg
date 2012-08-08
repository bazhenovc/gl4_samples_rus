/*
	������� ���������� �������� � ����������� �� ���������� �� ������.
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [-1,+1]

uniform float	unGridScale		= 100.0;
uniform float	unMaxTessLevel	= 32.0;
uniform float	unHeightScale	= 10.0;
uniform float	unDetailLevel	= 1000.0;
uniform mat4	unMVPMatrix;

out	TVertData {
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
} Output;


float Level(float dist)
{
	return clamp( unDetailLevel / dist - 2.0, 1.0, unMaxTessLevel );
}

void main()
{
	gl_Position			= vec4( inPosition * unGridScale, 0.0, 1.0 );
	Output.vNormal		= vec3( 0.0, 0.0, 1.0 );
	Output.vTexcoord0	= (inPosition + 1.0) * 100.0;	// for tiling
	Output.vTexcoord1	= (inPosition + 1.0) * 0.5;
	vec4	pos			= unMVPMatrix * vec4( gl_Position.xyz +
						  texture( unHeightMap, Output.vTexcoord1 ).r *
						  Output.vNormal * unHeightScale, 1.0 );
	Output.fLevel		= Level( length(pos) );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 4) out;

in	TVertData {
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
} Input[];

out TContData {
	vec3	vNormal;
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
	Output[I].vNormal		= Input[I].vNormal;
	Output[I].vTexcoord0	= Input[I].vTexcoord0;
	Output[I].vTexcoord1	= Input[I].vTexcoord1;
	Output[I].fLevel		= Input[I].fLevel;
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(quads, equal_spacing, ccw) in;

uniform mat4		unMVPMatrix;
uniform sampler2D	unHeightMap;
uniform float		unHeightScale	= 10.0;

in TContData {
	vec3	vNormal;
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
	float	height = texture( unHeightMap, vTexcoord ).r * 2.0;
	vec4	v0 = textureGatherOffsets( unHeightMap, vTexcoord, ivec2[]( ivec2(-1,-1),  ivec2(0,-1),  ivec2(1,-1),  ivec2(1,0) ) );
	vec4	v1 = textureGatherOffsets( unHeightMap, vTexcoord, ivec2[]( ivec2(-1,0),   ivec2(-1,1),  ivec2(0,1),   ivec2(1,1) ) );
	height += v0.x + v0.y + v0.z + v0.w;
	height += v1.x + v1.y + v1.z + v1.w;
	return height * 0.1;
}	
	
void main()
{
	vec4	pos 		= Interpolate( gl_in, .gl_Position );
	Output.vNormal 		= Interpolate( Input, .vNormal );
	Output.vTexcoord0	= Interpolate( Input, .vTexcoord0 );
	vec2	texc		= Interpolate( Input, .vTexcoord1 );
	
	pos.xyz += PCF( texc ) * Output.vNormal * unHeightScale;
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


void main()
{
	outColor.rgb	= texture( unDiffuseMap, Input.vTexcoord0 ).rgb;
	outColor.a		= 0.0;	// empty
	outNormal.rgb	= Input.vNormal;
	//outNormal.rg	= textureQueryLod( unDiffuseMap, Input.vTexcoord0 );
	//outNormal.b		= 0.0;	// empty
	outNormal.a		= Input.fLevel;
}

--eof
