/*
	Уровень тесселяции меняется в зависимости от положения полигона относительно камеры.
	Невидимые патчи отсекаются.
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [0,+1]

uniform float		unGridScale;
uniform float		unMaxTessLevel;
uniform float		unHeightScale;
uniform float		unDetailLevel;
uniform mat4		unMVPMatrix;
uniform mat3		unNormalMatrix;
uniform sampler2D	unHeightMap;
uniform sampler2D	unNormalMap;

out	TVertData {
	vec3	vNormal;
	vec2	vScrCoords;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	bool	bInScreen;
	float	fLevel;
} Output;


bool InScreen(in vec2 pos)
{
	const float		size = 1.2;
	return all( lessThan( abs(pos), vec2(size) ) );
}

float Level(float dist, in vec3 norm)
{
	float	norm_to_cam = (	abs( dot( vec3( 1.0,0.0,0.0), norm ) ) +
							abs( dot( vec3(-1.0,0.0,0.0), norm ) ) ) * 1.25;
	return clamp( unDetailLevel*unGridScale*0.05/dist * (norm_to_cam+0.35), 0.1, unMaxTessLevel );
}

void main()
{
	gl_Position			= vec4( inPosition * unGridScale, 0.0, 1.0 ).xzyw;
	Output.vTexcoord0	= inPosition * 100.0;	// for tiling
	Output.vTexcoord1	= inPosition;
	vec4	pos			= unMVPMatrix * vec4( inPosition * unGridScale,
						  texture( unHeightMap, Output.vTexcoord1 ).r * unHeightScale, 1.0 ).xzyw;
	Output.vScrCoords	= pos.xy / pos.w;
	Output.vNormal		= normalize(texture( unNormalMap, Output.vTexcoord1 ).rbg * 2.0 - 1.0) * unNormalMatrix;
	Output.fLevel		= Level( pos.z, Output.vNormal );
	Output.bInScreen	= InScreen( Output.vScrCoords );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 4) out;

in	TVertData {
	vec3	vNormal;
	vec2	vScrCoords;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	bool	bInScreen;
	float	fLevel;
} Input[];

out TContData {
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
} Output[];


#define Min4( _a, _b, _c, _d )	min( min( _a, _b ), min( _c, _d ) )
#define Max4( _a, _b, _c, _d )	max( max( _a, _b ), max( _c, _d ) )

vec4 Rect(in vec2 p0, in vec2 p1, in vec2 p2, in vec2 p3)
{
	return 	 vec4(	Min4( p0.x, p1.x, p2.x, p3.x ),
					Min4( p0.y, p1.y, p2.y, p3.y ),
					Max4( p0.x, p1.x, p2.x, p3.x ),
					Max4( p0.y, p1.y, p2.y, p3.y ) );
}

bool QuadInScreen()
{
	const float		size = 1.2;
	
	vec4	screen	= vec4( -size, -size, size, size );
	vec4	rect 	=  Rect( Input[0].vScrCoords, Input[1].vScrCoords,
							 Input[2].vScrCoords, Input[3].vScrCoords );
	return	( rect[0] < screen[2] && rect[2] > screen[0] &&
			  rect[1] < screen[3] && rect[3] > screen[1] ) ||
			( screen[2] < rect[0] && screen[0] > rect[2] &&
			  screen[3] < rect[1] && screen[1] > rect[3] );
}

void main()
{
	if ( I == 0 )
	{
		bool	in_screen = any( bvec4( Input[0].bInScreen, Input[1].bInScreen,
										Input[2].bInScreen, Input[3].bInScreen ) );
		float	k = ( in_screen || QuadInScreen() ) ? 1.0 : 0.0;
		
		gl_TessLevelOuter[0] = max( Input[0].fLevel, Input[3].fLevel ) * k;
		gl_TessLevelOuter[1] = max( Input[0].fLevel, Input[1].fLevel ) * k;
		gl_TessLevelOuter[2] = max( Input[1].fLevel, Input[2].fLevel ) * k;
		gl_TessLevelOuter[3] = max( Input[2].fLevel, Input[3].fLevel ) * k;
		float	max_level = max( max( gl_TessLevelOuter[0], gl_TessLevelOuter[1]),
								 max( gl_TessLevelOuter[2], gl_TessLevelOuter[3] ) );
		gl_TessLevelInner[0] = max_level * k;
		gl_TessLevelInner[1] = max_level * k;
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

layout(quads, fractional_even_spacing, ccw) in;

uniform mat4		unMVPMatrix;
uniform sampler2D	unHeightMap;
uniform sampler2D	unNormalMap;
uniform float		unHeightScale;

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
	( mix(	mix( a[0] p, a[1] p, gl_TessCoord.x ), \
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
	return ( height * 0.1 );
}
	
void main()
{
	vec4	pos 		= Interpolate( gl_in, .gl_Position );
	Output.vTexcoord0	= Interpolate( Input, .vTexcoord0 );
	Output.fLevel		= Interpolate( Input, .fLevel );
	vec2	texc		= Interpolate( Input, .vTexcoord1 );
	Output.vNormal		= normalize( texture( unNormalMap, texc ).rbg * 2.0 - 1.0 );
	
	pos.xyz += PCF( texc ) * vec3(0.0, 1.0, 0.0) * unHeightScale;
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
	outNormal.a		= 1.0 - Input.fLevel / unMaxTessLevel;
}

--eof
