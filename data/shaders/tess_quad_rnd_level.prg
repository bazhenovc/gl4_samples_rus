/*
	Пример шейдера с рандомным уровнем тесселяции патей.
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [-1,+1]

uniform float	unGridScale		= 100.0;
uniform float	unMaxTessLevel	= 12.0;

out	TVertData {
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
} Output;


float Rand(in vec2 v) {
    return fract( sin( dot( v, vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

void main()
{
	gl_Position			= vec4( inPosition * unGridScale, 0.0, 1.0 );
	Output.vNormal		= vec3( 0.0, 0.0, 1.0 );
	Output.vTexcoord0	= (inPosition + 1.0) * 100.0;	// for tiling
	Output.vTexcoord1	= (inPosition + 1.0) * 0.5;
	Output.fLevel		= clamp( ( Rand( inPosition ) + 1.0 ) * unMaxTessLevel,
								 1.0, unMaxTessLevel );
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
	
	gl_out[I]			 = gl_in[I].gl_Position;
	Output[I].vNormal	 = Input[I].vNormal;
	Output[I].vTexcoord0 = Input[I].vTexcoord0;
	Output[I].Texcoord1	 = Input[I].vTexcoord1;
	Output[I].fLevel	 = Input[I].fLevel;
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


#define Interpolate( _a, _p ) \
	( mix(	mix( _a[0] _p, _a[1] _p, gl_TessCoord.x ), \
			mix( _a[3] _p, _a[2] _p, gl_TessCoord.x ), \
			gl_TessCoord.y ) )


float PCF(in vec2 vTexcoord)
{
	float	height = 0.0;
	height += textureOffset( unHeightMap, texcoord, ivec2(-1,-1) ).r;
	height += textureOffset( unHeightMap, texcoord, ivec2(-1, 0) ).r;
	height += textureOffset( unHeightMap, texcoord, ivec2(-1, 1) ).r;
	height += textureOffset( unHeightMap, texcoord, ivec2( 0,-1) ).r;
	height += textureOffset( unHeightMap, texcoord, ivec2( 0, 0) ).r * 2.0;
	height += textureOffset( unHeightMap, texcoord, ivec2( 0, 1) ).r;
	height += textureOffset( unHeightMap, texcoord, ivec2( 1,-1) ).r;
	height += textureOffset( unHeightMap, texcoord, ivec2( 1, 0) ).r;
	height += textureOffset( unHeightMap, texcoord, ivec2( 1, 1) ).r;
	return height * 0.1;
}	
	
void main()
{
	vec4	pos 		= Interpolate( gl_in, .gl_Position );
	Output.vNormal 		= Interpolate( Input, .vNormal );
	Output.vTexcoord0	= Interpolate( Input, .vTexcoord0 );
	Output.fLevel		= Interpolate( Input, .fLevel );
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
	outNormal.a		= Input.fLevel;
}

// [END]
