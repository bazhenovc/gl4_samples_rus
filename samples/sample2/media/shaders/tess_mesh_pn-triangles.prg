/*
	Шейдер сглаживания ландшафта с использованием техники PN-triangles.
	Уровень детализации зависит от расстояния до камеры и размер патча на экране.
	Невидимые патчи отсекаются.
*/

--vertex
#version 410 core

layout(location = 0)	in vec3 inPosition;
layout(location = 1)	in vec2	inTexcoord;
layout(location = 2)	in vec3	inNormal;

uniform mat4	unMVPMatrix;

out	TVertData {
	vec3	vNormal;
	vec2	vScrCoords;
	vec2	vTexcoord;
	bool	bInScreen;
} Output;


bool InScreen(in vec2 pos)
{
	const float		size = 1.2;
	return all( lessThan( abs(pos), vec2(size) ) );
}

void main()
{
	Output.vTexcoord	= inTexcoord;
	gl_Position			= vec4( inPosition, 1.0 ).yzxw;
	vec4	pos			= unMVPMatrix * gl_Position;
	Output.vNormal		= inNormal.yzx * -1.0;
	Output.vScrCoords	= pos.xy / pos.w;
	Output.bInScreen	= InScreen( Output.vScrCoords );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 3) out;

uniform float	unMaxTessLevel;

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
	vec2	vTexcoord;
	bool	bInScreen;
} Input[];

out TContData {
	vec3	vNormal;
	vec2	vTexcoord;
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
		float	scr_x = max( Input[0].vScrCoords.x, max( Input[1].vScrCoords.x, Input[2].vScrCoords.x ) );
		float	tesslevel = unMaxTessLevel;//(scr_x > 0.0 ? unMaxTessLevel : 1.0) * k;
		
		gl_TessLevelOuter[2] = k;
		gl_TessLevelOuter[0] = k;
		gl_TessLevelOuter[1] = k;
		gl_TessLevelInner[0] = tesslevel;
	}
	
	gl_out[I].gl_Position	= gl_in[I].gl_Position;
	Output[I].vNormal		= Input[I].vNormal;
	Output[I].vTexcoord		= Input[I].vTexcoord;
	
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
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4		unMVPMatrix;
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
	vec2	vTexcoord;
	PnPatch	sPatch;
} Input[];

out	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord;
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
	vec3	b300	= gl_in[0].gl_Position.xyz;
	vec3	b030	= gl_in[1].gl_Position.xyz;
	vec3	b003	= gl_in[2].gl_Position.xyz;
	vec3	n200	= Input[0].vNormal;
	vec3	n020	= Input[1].vNormal;
	vec3	n002    = Input[2].vNormal;
	vec3	uvw		= gl_TessCoord;

	vec3 uvwSquared	= uvw * uvw;
	vec3 uvwCubed	= uvwSquared * uvw;
	uvwSquared *= 3.0;

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

	Output.vTexcoord	= Interpolate( Input, .vTexcoord );

	// normal
	vec3	ln_norm		= normalize( Interpolate( Input, .vNormal ) );
	/*vec3	pn_norm		= normalize( 
						  n200 * uvwSquared[2] +
						  n020 * uvwSquared[0] +
						  n002 * uvwSquared[1] +
						  n110 * uvw[2] * uvw[0] +
						  n011 * uvw[0] * uvw[1] +
						  n101 * uvw[2] * uvw[1] );
	Output.vNormal		= (1.0-unPositionBlend) * ln_norm +
						  unPositionBlend * pn_norm;*/
	Output.vNormal		= ln_norm;

	// position
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
	vec3	blend_pos	= (1.0-unPositionBlend) * ln_pos + unPositionBlend * pn_pos;
	gl_Position			= unMVPMatrix * vec4( blend_pos, 1.0 );

	Output.fLevel		= gl_TessLevelOuter[2] * gl_TessCoord.x +
						  gl_TessLevelOuter[0] * gl_TessCoord.y +
						  gl_TessLevelOuter[1] * gl_TessCoord.z;
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
	vec2	vTexcoord;
	float	fLevel;
} Input;


void main()
{
	outColor.rgb	= texture( unDiffuseMap, Input.vTexcoord ).rgb;
	outColor.a		= 0.0;	// empty
	outNormal.rgb	= Input.vNormal * 0.5 + 0.5;
	outNormal.a		= 1.0 - Input.fLevel / unMaxTessLevel;
}

--eof
