/*
	Шейдер сглаживания ландшафта с использованием техники Phong tessellation.
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
	gl_Position			= vec4( inPosition, 1.0 );
	vec4	pos			= unMVPMatrix * gl_Position;
	Output.vNormal		= inNormal;
	Output.vScrCoords	= pos.xy / pos.w;
	Output.bInScreen	= InScreen( Output.vScrCoords );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 410 core

#define I	gl_InvocationID

layout(vertices = 3) out;

uniform float	unMaxTessLevel;
uniform float	unPositionBlend;

struct PhongPatch
{
	float termIJ;
	float termJK;
	float termIK;
};

in	TVertData {
	vec3	vNormal;
	vec2	vScrCoords;
	vec2	vTexcoord;
	bool	bInScreen;
} Input[];

out TContData {
	vec3		vNormal;
	vec2		vTexcoord;
	PhongPatch	sPatch;
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

float PIi(int i, vec3 q)
{
	vec3 q_minus_p = q - gl_in[i].gl_Position.xyz;
	return ( q[I] - dot( q_minus_p, Input[i].vNormal ) * Input[i].vNormal[I] * unPositionBlend * 1.0 );
}

float TessLevel()
{
	vec3	n01 = abs( Input[0].vNormal - Input[1].vNormal );
	vec3	n02 = abs( Input[0].vNormal - Input[2].vNormal );
	return clamp( ( n01.x + n01.y + n01.z + n02.x + n02.y + n02.z ) / 6 * unMaxTessLevel, 1.0, unMaxTessLevel );
}

void main()
{
	if ( I == 0 )
	{
		bool	in_screen = any( bvec3( Input[0].bInScreen, Input[1].bInScreen, Input[2].bInScreen ) );
		float	k = ( in_screen || TriangleInScreen() ) ? 1.0 : 1.0;
		//float	scr_x = Max3( Input[0].vScrCoords.x, Input[1].vScrCoords.x, Input[2].vScrCoords.x );
		//float	tesslevel = (scr_x > 0.0 ? unMaxTessLevel : 1.0) * k;
		float	tesslevel = unMaxTessLevel * k;
		
		gl_TessLevelOuter[0] = tesslevel;
		gl_TessLevelOuter[1] = tesslevel;
		gl_TessLevelOuter[2] = tesslevel;
		gl_TessLevelInner[0] = TessLevel() * k;
	}
	
	vec3	Pi	= gl_in[0].gl_Position.xyz;
	vec3	Pj	= gl_in[1].gl_Position.xyz;
	vec3	Pk	= gl_in[2].gl_Position.xyz;

	Output[I].sPatch.termIJ = PIi( 0, Pj ) + PIi( 1, Pi );
	Output[I].sPatch.termJK = PIi( 1, Pk ) + PIi( 2, Pj );
	Output[I].sPatch.termIK = PIi( 2, Pi ) + PIi( 0, Pk );
	
	gl_out[I].gl_Position	= gl_in[I].gl_Position;
	Output[I].vNormal		= Input[I].vNormal;
	Output[I].vTexcoord		= Input[I].vTexcoord;
}



//-----------------------------------------------------------------------------
--tesseval
#version 410 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4		unMVPMatrix;
//uniform float		unPositionBlend;

struct PhongPatch
{
	float termIJ;
	float termJK;
	float termIK;
};

in TContData {
	vec3		vNormal;
	vec2		vTexcoord;
	PhongPatch	sPatch;
} Input[];

out	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord;
	float	fLevel;
} Output;


#define Interpolate( _a, _p ) \
	(	gl_TessCoord.x * _a[0] _p + \
		gl_TessCoord.y * _a[1] _p + \
		gl_TessCoord.z * _a[2] _p )
		
#define PATCH( _a, _b, _c ) \
	Input[0].sPatch._a, Input[1].sPatch._b, Input[2].sPatch._c
	
void main()
{
	vec3	Pi		= gl_in[0].gl_Position.xyz;
	vec3	Pj		= gl_in[1].gl_Position.xyz;
	vec3	Pk		= gl_in[2].gl_Position.xyz;
	vec3	tc1		= gl_TessCoord;
	vec3	tc2 	= tc1*tc1;
	vec3 	termIJ	= vec3( PATCH( termIJ, termIJ, termIJ ) );
	vec3 	termJK	= vec3( PATCH( termJK, termJK, termJK ) );
	vec3 	termIK	= vec3( PATCH( termIK, termIK, termIK ) );

	// position
	vec3	ln_pos	= Interpolate( gl_in, .gl_Position.xyz );
	vec3	ph_pos	= tc2[0] * Pi +
					  tc2[1] * Pj +
					  tc2[2] * Pk +
					  tc1[0] * tc1[1] * termIJ +
					  tc1[1] * tc1[2] * termJK +
					  tc1[2] * tc1[0] * termIK;
					  
	bool	is_edge		= any( equal( gl_TessCoord, vec3(0.0) ) );
	float	blend		= is_edge ? 0.0 : 1.0;//unPositionBlend;
	vec3	blend_pos	= (1.0-blend) * ln_pos + blend * ph_pos;
	gl_Position			= unMVPMatrix * vec4( blend_pos, 1.0 );

	Output.vNormal		= normalize( Interpolate( Input, .vNormal ) );
	Output.vTexcoord	= Interpolate( Input, .vTexcoord );
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
