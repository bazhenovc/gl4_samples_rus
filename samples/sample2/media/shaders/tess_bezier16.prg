/*
	Шейдер сглаживания ландшафта с использованием Безье патчей.
	Уровень детализации зависит от расстояния до камеры и размер патча на экране.
	Невидимые патчи отсекаются.
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [0,+1]

uniform float		unGridScale		= 100.0;
uniform float		unMaxTessLevel	= 32.0;
uniform float		unHeightScale	= 10.0;
uniform float		unDetailLevel	= 1000.0;
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
	return clamp( unDetailLevel*unGridScale*0.1/dist - 2.0, 0.1, unMaxTessLevel );
}

void main()
{
	Output.vTexcoord0	= inPosition * 100.0;
	vec2	texc		= inPosition;
	gl_Position			= unMVPMatrix * vec4( inPosition * unGridScale,
						  texture( unHeightMap, texc ).r * unHeightScale, 1.0 ).xzyw;
	Output.fLevel		= Level( gl_Position.z );
	Output.vNormal		= normalize( texture( unNormalMap, texc ).rbg * 2.0 - 1.0 );
	Output.vScrCoords	= gl_Position.xy / gl_Position.w;
	Output.bInScreen	= InScreen( Output.vScrCoords );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 16) out;

uniform float	unMaxTessLevel	= 32.0;
uniform float	unDetailLevel	= 1000.0;

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


#define Min4( _a, _b, _c, _d )	min( min( _a, _b ), min( _c, _d ) )
#define Max4( _a, _b, _c, _d )	max( max( _a, _b ), max( _c, _d ) )

#define Level( _a, _b, _c, _d ) \
	max( max( _a.fLevel, _d.fLevel ), ScreenSpaceLevel( _a.vScrCoords, _d.vScrCoords ) )
//	max( _a.fLevel, _b.fLevel )

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
	vec4	rect 	=  Rect( Input[0].vScrCoords,  Input[3].vScrCoords,
							 Input[12].vScrCoords, Input[15].vScrCoords );
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
		bool	in_screen = any( bvec4( Input[0].bInScreen,  Input[3].bInScreen,
										Input[12].bInScreen, Input[15].bInScreen ) );
		float	k = ( in_screen || QuadInScreen() ) ? 1.0 : 0.0;
		
		gl_TessLevelOuter[0] = Level( Input[0],  Input[4],  Input[8],  Input[12] ) * k;
		gl_TessLevelOuter[1] = Level( Input[0],  Input[1],  Input[2],  Input[3]  ) * k;
		gl_TessLevelOuter[2] = Level( Input[3],  Input[7],  Input[11], Input[15] ) * k;
		gl_TessLevelOuter[3] = Level( Input[12], Input[13], Input[14], Input[15] ) * k;
		float	max_level = max( max( gl_TessLevelOuter[0], gl_TessLevelOuter[1]),
								 max( gl_TessLevelOuter[2], gl_TessLevelOuter[3] ) );
		gl_TessLevelInner[0] = max_level * k;
		gl_TessLevelInner[1] = max_level * k;
	}
	
	gl_out[I].gl_Position	= gl_in[I].gl_Position;
	Output[I].vNormal		= Input[I].vNormal;
	Output[I].vTexcoord0	= Input[I].vTexcoord0;
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(quads, equal_spacing, ccw) in;

in TContData {
	vec3	vNormal;
	vec2	vTexcoord0;
} Input[];

out	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Output;


#define Interpolate( _a, _p, _bu, _bv ) \
	( _bu.x * (_bv.x * (_a[0] _p) + _bv.y * (_a[4] _p) + _bv.z * (_a[8]  _p) + _bv.w * (_a[12] _p)) + \
	  _bu.y * (_bv.x * (_a[1] _p) + _bv.y * (_a[5] _p) + _bv.z * (_a[9]  _p) + _bv.w * (_a[13] _p)) + \
	  _bu.z * (_bv.x * (_a[2] _p) + _bv.y * (_a[6] _p) + _bv.z * (_a[10] _p) + _bv.w * (_a[14] _p)) + \
	  _bu.w * (_bv.x * (_a[3] _p) + _bv.y * (_a[7] _p) + _bv.z * (_a[11] _p) + _bv.w * (_a[15] _p)) )
	
#define InterpolateQuad( _a, _p ) \
	( mix(	mix( _a[0] _p, _a[1] _p, gl_TessCoord.x ), \
			mix( _a[3] _p, _a[2] _p, gl_TessCoord.x ), \
			gl_TessCoord.y ) )
			
void main()
{
	vec4		u = vec4 ( 1.0, gl_TessCoord.x, gl_TessCoord.x * gl_TessCoord.x,
								gl_TessCoord.x * gl_TessCoord.x * gl_TessCoord.x );
	vec4		v = vec4 ( 1.0, gl_TessCoord.y, gl_TessCoord.y * gl_TessCoord.y,
								gl_TessCoord.y * gl_TessCoord.y * gl_TessCoord.y );
	const float	p = 3.0;	// can change to control smoothing
	const mat4	b = mat4 (	 1.0,  0,      0.0, 0.0,
							-p,    p,      0.0, 0.0,
							 p,   -p*2.0,  p,   0.0,
							-1.0,  p,     -p,   1.0 );
	vec4		bu = b * u;
	vec4		bv = b * v;

	gl_Position			= Interpolate( gl_in, .gl_Position, bu, bv );
	Output.vNormal		= Interpolate( Input, .vNormal, bu, bv );
	Output.vTexcoord0 	= Interpolate( Input, .vTexcoord0, bu, bv );
	Output.fLevel		= mix( gl_TessLevelInner[0], InterpolateQuad( gl_TessLevelOuter, ),
								distance( gl_TessCoord.xy, vec2(0.5) ) );
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
