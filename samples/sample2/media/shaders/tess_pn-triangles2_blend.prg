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

struct PnPatch
{
	float b210;
	float b120;
	float b021;
	float b012;
	float b102;
	float b201;
	float b111;
	float n110;
	float n011;
	float n101;
};

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
	PnPatch	sPatch;
} Output[];


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

float wij(int i, int j)
{
	return dot( gl_in[j].gl_Position.xyz - gl_in[i].gl_Position.xyz, Input[i].vNormal );
}

float vij(int i, int j)
{
	vec3 Pj_minus_Pi = gl_in[j].gl_Position.xyz
	                 - gl_in[i].gl_Position.xyz;
	vec3 Ni_plus_Nj  = Input[i].vNormal + Input[j].vNormal;
	return 2.0 * dot( Pj_minus_Pi, Ni_plus_Nj ) /
				 dot( Pj_minus_Pi, Pj_minus_Pi );
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
	}
	
	float P0 = gl_in[0].gl_Position[I];
	float P1 = gl_in[1].gl_Position[I];
	float P2 = gl_in[2].gl_Position[I];
	float N0 = Input[0].vNormal[I];
	float N1 = Input[1].vNormal[I];
	float N2 = Input[2].vNormal[I];

	Output[I].sPatch.b210 = (2.0*P0 + P1 - wij(0,1)*N0)/3.0;
	Output[I].sPatch.b120 = (2.0*P1 + P0 - wij(1,0)*N1)/3.0;
	Output[I].sPatch.b021 = (2.0*P1 + P2 - wij(1,2)*N1)/3.0;
	Output[I].sPatch.b012 = (2.0*P2 + P1 - wij(2,1)*N2)/3.0;
	Output[I].sPatch.b102 = (2.0*P2 + P0 - wij(2,0)*N2)/3.0;
	Output[I].sPatch.b201 = (2.0*P0 + P2 - wij(0,2)*N0)/3.0;
	float E = ( Output[I].sPatch.b210 +
	            Output[I].sPatch.b120 +
	            Output[I].sPatch.b021 +
	            Output[I].sPatch.b012 +
	            Output[I].sPatch.b102 +
	            Output[I].sPatch.b201 ) / 6.0;
	float V = (P0 + P1 + P2)/3.0;
	Output[I].sPatch.b111 = E + (E - V)*0.5;
	Output[I].sPatch.n110 = N0+N1-vij(0,1)*(P1-P0);
	Output[I].sPatch.n011 = N1+N2-vij(1,2)*(P2-P1);
	Output[I].sPatch.n101 = N2+N0-vij(2,0)*(P0-P2);
	
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

struct PnPatch
{
	float b210;
	float b120;
	float b021;
	float b012;
	float b102;
	float b201;
	float b111;
	float n110;
	float n011;
	float n101;
};

in TContData {
	vec3	vNormal;
	vec2	vTexcoord0;
	PnPatch	sPatch;
} Input[];

out	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Output;


#define Interpolate( _a, _p ) \
	(	gl_TessCoord.z * _a[0] _p + \
		gl_TessCoord.x * _a[1] _p + \
		gl_TessCoord.y * _a[2] _p )
		
#define PATCH( _a, _b, _c ) \
	Input[0].sPatch._a, Input[1].sPatch._b, Input[2].sPatch._c
	
void main()
{
	float	b300	= gl_in[0].gl_Position.xyz;
	float	b030	= gl_in[1].gl_Position.xyz;
	float	b003	= gl_in[2].gl_Position.xyz;
	float	n200	= Input[0].vNormal;
	float	n020	= Input[1].vNormal;
	float	n002    = Input[2].vNormal;
	vec3	uvw		= gl_TessCoord;

	vec3 uvwSquared	= uvw*uvw;
	vec3 uvwCubed	= uvwSquared*uvw;

	vec3 b210 = vec3( PATCH( b210, b210, b210 ) );
	vec3 b120 = vec3( PATCH( b120, b120, b120 ) );
	vec3 b021 = vec3( PATCH( b021, b021, b021 ) );
	vec3 b012 = vec3( PATCH( b012, b012, b012 ) );
	vec3 b102 = vec3( PATCH( b102, b102, b102 ) );
	vec3 b201 = vec3( PATCH( b201, b201, b201 ) );
	vec3 b111 = vec3( PATCH( b111, b111, b111 ) );

	vec3 n110 = normalize( vec3( PATCH( n110, n110, n110 ) ) );
	vec3 n011 = normalize( vec3( PATCH( n011, n011, n011 ) ) );
	vec3 n101 = normalize( vec3( PATCH( n101, n101, n101 ) ) );

	Output.vTexcoord0	= Interpolate( Input, .vTexcoord0 );

	vec3	ln_norm		= normalize( Interpolate( Input, .vNormal ) );
	vec3	pn_norm		= n200 * uvwSquared[2] +
						  n020 * uvwSquared[0] +
						  n002 * uvwSquared[1] +
						  n110 * uvw[2] * uvw[0] +
						  n011 * uvw[0] * uvw[1] +
						  n101 * uvw[2] * uvw[1];
	Output.vNormal		= (1.0-unPositionBlend) * ln_norm +
						  unPositionBlend * pn_norm;

	uvwSquared *= 3.0;
	vec3	ln_pos		= Interpolate( gl_in, .gl_Position.xyz );
	vec3	pn_pos		= b300 * uvwCubed[2] +
						  b030 * uvwCubed[0] +
						  b003 * uvwCubed[1] +
						  b210 * uvwSquared[2] * uvw[0] +
						  b120 * uvwSquared[0] * uvw[2] +
						  b201 * uvwSquared[2] * uvw[1] +
						  b021 * uvwSquared[0] * uvw[1] +
						  b102 * uvwSquared[1] * uvw[2] +
						  b012 * uvwSquared[1] * uvw[0] +
						  b111 * 6.0 * uvw[0] * uvw[1] * uvw[2];

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
