/*
	Шейдер сглаживания ландшафта с использованием техники PN-triangles.
	Уровень детализации зависит от расстояния до камеры и размер патча на экране.
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
uniform sampler2D	unHeightMap;
uniform sampler2D	unNormalMap;

out	TVertData {
	vec3	vNormal;
	vec2	vScrCoords;
	vec2	vTexcoord0;
	float	fLevel;
	bool	bInScreen;
} Output;


bool InScreen(in vec2 pos)
{
	const float		size = 1.2;
	return all( lessThan( abs(pos), vec2(size) ) );
}

float Level(float dist)
{
	return clamp( unDetailLevel*unGridScale*0.1/dist - 2.0, 1.0, unMaxTessLevel );
}

void main()
{
	Output.vTexcoord0	= inPosition * 100.0;
	vec2	texc		= inPosition;
	gl_Position			= vec4( inPosition * unGridScale,
						  texture( unHeightMap, texc ).r * unHeightScale, 1.0 ).xzyw;
	vec4	pos			= unMVPMatrix * gl_Position;
	Output.fLevel		= Level( pos.z );
	Output.vNormal		= normalize( texture( unNormalMap, texc ).rbg * 2.0 - 1.0 );
	Output.vScrCoords	= pos.xy / pos.w;
	Output.bInScreen	= InScreen( Output.vScrCoords );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 3) out;

uniform float	unMaxTessLevel;
uniform float	unDetailLevel;

in	TVertData {
	vec3	vNormal;
	vec2	vScrCoords;
	vec2	vTexcoord0;
	float	fLevel;
	bool	bInScreen;
} Input[];

out TContData {
	vec3	vNormal;
	vec2	vTexcoord0;
} Output[];

patch out	vec3	vB210;
patch out	vec3	vB120;
patch out	vec3	vB021;
patch out	vec3	vB012;
patch out	vec3	vB102;
patch out	vec3	vB201;
patch out	vec3	vB111;


#define Min3( _a, _b, _c )	min( min( _a, _b ), _c )
#define Max3( _a, _b, _c )	max( max( _a, _b ), _c )

#define Level( _a, _b ) \
	max( max( _a.fLevel, _b.fLevel ), ScreenSpaceLevel( _a.vScrCoords, _b.vScrCoords ) )

vec4 Rect(in vec2 p0, in vec2 p1, in vec2 p2)
{
	return 	 vec4(	Min3( p0.x, p1.x, p2.x ),
					Min3( p0.y, p1.y, p2.y ),
					Max3( p0.x, p1.x, p2.x ),
					Max3( p0.y, p1.y, p2.y ) );
}

bool TriangleInScreen()
{
	const float		size = 1.2;
	
	vec4	screen	= vec4( -size, -size, size, size );
	vec4	rect 	=  Rect( Input[0].vScrCoords, Input[1].vScrCoords,
							 Input[2].vScrCoords );
	return	( rect[0] < screen[2] && rect[2] > screen[0] &&
			  rect[1] < screen[3] && rect[3] > screen[1] ) ||
			( screen[2] < rect[0] && screen[0] > rect[2] &&
			  screen[3] < rect[1] && screen[1] > rect[3] );
}

float ScreenSpaceLevel(in vec2 p0, in vec2 p1)
{
	return clamp( distance( p0, p1 ) * unDetailLevel * 4.0, 0.1, unMaxTessLevel );
}

void main()
{
	if ( I == 0 )
	{
		bool	in_screen = any( bvec3( Input[0].bInScreen, Input[1].bInScreen, Input[2].bInScreen ) );
		float	k = ( in_screen || TriangleInScreen() ) ? 1.0 : 0.0;
		
		gl_TessLevelOuter[2] = Level( Input[1], Input[2] ) * k;
		gl_TessLevelOuter[0] = Level( Input[0], Input[2] ) * k;
		gl_TessLevelOuter[1] = Level( Input[0], Input[1] ) * k;
		gl_TessLevelInner[0] = max( max( gl_TessLevelOuter[0], gl_TessLevelOuter[1] ),
									gl_TessLevelOuter[2] ) * k;
									
		vec3	b300 = gl_in[0].gl_Position.xyz;
		vec3	b030 = gl_in[1].gl_Position.xyz;
		vec3	b003 = gl_in[2].gl_Position.xyz;
		vec3	n200 = Input[0].vNormal;
		vec3	n020 = Input[1].vNormal;
		vec3	n002 = Input[2].vNormal;
		
		vB210	= ( 2.0*b300 + b030 - dot( b030-b300, n200 )*n200 )/3.0;
		vB120	= ( 2.0*b030 + b300 - dot( b300-b030, n020 )*n020 )/3.0;
		vB021	= ( 2.0*b030 + b003 - dot( b003-b030, n020 )*n020 )/3.0;
		vB012	= ( 2.0*b003 + b030 - dot( b030-b003, n002 )*n002 )/3.0;
		vB102	= ( 2.0*b003 + b300 - dot( b300-b003, n002 )*n002 )/3.0;
		vB201	= ( 2.0*b300 + b003 - dot( b003-b300, n200 )*n200 )/3.0;
	
		vec3	ee	= ( vB210 + vB120 + vB021 + vB012 + vB102 + vB201 )/6.0;
		vec3	vv	= ( b300 + b030 + b003 )/3.0;
		vB111		= ee + (ee - vv)/2.0;
	}
	
	gl_out[I].gl_Position	= gl_in[I].gl_Position;
	Output[I].vNormal		= Input[I].vNormal;
	Output[I].vTexcoord0	= Input[I].vTexcoord0;
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4		unMVPMatrix;
uniform sampler2D	unHeightMap;
uniform sampler2D	unNormalMap;
uniform float		unHeightScale;
uniform float		unPositionBlend;

in TContData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Input[];

out	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Output;

patch in	vec3	vB210;
patch in	vec3	vB120;
patch in	vec3	vB021;
patch in	vec3	vB012;
patch in	vec3	vB102;
patch in	vec3	vB201;
patch in	vec3	vB111;


#define Interpolate( _a, _p ) \
	(	gl_TessCoord.z * _a[0] _p + \
		gl_TessCoord.x * _a[1] _p + \
		gl_TessCoord.y * _a[2] _p )
	
void main()
{
	vec3	u		= gl_TessCoord;
	vec3	v		= u * u;
	vec3	w		= v * 3.0;
	vec3	pn_pos	=	gl_in[0].gl_Position.xyz * v.z * u.z +
						gl_in[1].gl_Position.xyz * v.x * u.x +
						gl_in[2].gl_Position.xyz * v.y * u.y +
						vB210 * w.z * u.x +
						vB120 * u.z * w.x +
						vB201 * w.z * u.y +
						vB021 * w.x * u.y +
						vB102 * u.z * w.y +
						vB012 * u.x * w.y +
						vB111 * 6.0 * u.x * u.y * u.z;
	vec3	ln_pos	= Interpolate( gl_in, .gl_Position.xyz );
	Output.vNormal		= normalize( Interpolate( Input, .vNormal ) );
	Output.vTexcoord0	= Interpolate( Input, .vTexcoord0 );
	Output.fLevel		= gl_TessLevelOuter[2] * gl_TessCoord.x +
						  gl_TessLevelOuter[0] * gl_TessCoord.y +
						  gl_TessLevelOuter[1] * gl_TessCoord.z;
	vec3	blend_pos	= (1.0-unPositionBlend) * ln_pos + unPositionBlend * pn_pos;
	gl_Position			= unMVPMatrix * vec4( blend_pos, 1.0 );
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
