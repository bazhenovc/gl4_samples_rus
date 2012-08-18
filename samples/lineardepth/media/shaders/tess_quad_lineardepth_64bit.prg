/*
	Уровень тесселяции меняется в зависимости от расстояния до камеры.
	Невидимые патчи отсекаются.
*/

--vertex
#version 420 core

layout(location = 0)	in vec2 inPosition;		// [0,+1]

uniform float		unGridScale;
uniform float		unMaxTessLevel;
uniform float		unHeightScale;
uniform float		unDetailLevel;
uniform mat4		unMVPMatrix;
uniform sampler2D	unHeightMap;

out	TVertData {
	vec3	vNormal;
	vec2	vScrCoords;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
	float	fDistance;
	bool	bInScreen;
} Output;


bool InScreen(in vec2 pos)
{
	const float		size = 1.2;
	return ( abs(pos.x) <= size && abs(pos.y) <= size );
}

float Level(float dist)
{
	return clamp( unDetailLevel*unGridScale*0.1/dist - 2.0, 1.0, unMaxTessLevel );
}

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
	Output.fDistance	= pos.z * 0.5;
	Output.fLevel		= Level( length(pos.xyz) );
	Output.bInScreen	= InScreen( Output.vScrCoords );
}



//-----------------------------------------------------------------------------
--tesscontrol
#version 420 core

#define I	gl_InvocationID

layout(vertices = 4) out;

in	TVertData {
	vec3	vNormal;
	vec2	vScrCoords;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
	float	fDistance;
	bool	bInScreen;
} Input[];

out TContData {
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
	float	fDistance;
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
		float	max_level = max( max( Input[0].fLevel, Input[1].fLevel ),
								 max( Input[2].fLevel, Input[3].fLevel ) );
		float	k = ( in_screen || QuadInScreen() ) ? 1.0 : 0.0;
		
		gl_TessLevelInner[0] = max_level * k;
		gl_TessLevelInner[1] = max_level * k;
		gl_TessLevelOuter[0] = max( Input[0].fLevel, Input[3].fLevel ) * k;
		gl_TessLevelOuter[1] = max( Input[0].fLevel, Input[1].fLevel ) * k;
		gl_TessLevelOuter[2] = max( Input[1].fLevel, Input[2].fLevel ) * k;
		gl_TessLevelOuter[3] = max( Input[2].fLevel, Input[3].fLevel ) * k;
	}
	
	gl_out[I].gl_Position	= gl_in[I].gl_Position;
	Output[I].vNormal		= Input[I].vNormal;
	Output[I].vTexcoord0	= Input[I].vTexcoord0;
	Output[I].vTexcoord1	= Input[I].vTexcoord1;
	Output[I].fLevel		= Input[I].fLevel;
	Output[I].fDistance		= Input[I].fDistance;
}



//-----------------------------------------------------------------------------
--tesseval
#version 420 core

layout(quads, equal_spacing, ccw) in;

uniform sampler2D	unHeightMap;
uniform sampler2D	unNormalMap;
uniform float		unHeightScale;
uniform vec2		unFarPlanes;

in TContData {
	vec3	vNormal;
	vec2	vTexcoord0;
	vec2	vTexcoord1;
	float	fLevel;
	float	fDistance;
} Input[];

out	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
	float	fDistance;
	int		iLayerBits;
} Output;


#define Interpolate( _a, _p ) \
	( mix(	mix( _a[0] _p, _a[1] _p, gl_TessCoord.x ), \
			mix( _a[3] _p, _a[2] _p, gl_TessCoord.x ), \
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
	vec3	norm 		= Interpolate( Input, .vNormal );
	Output.vTexcoord0	= Interpolate( Input, .vTexcoord0 );
	Output.fLevel		= Interpolate( Input, .fLevel );
	Output.fDistance	= Interpolate( Input, .fDistance );
	vec2	texc		= Interpolate( Input, .vTexcoord1 );
	Output.vNormal		= normalize( texture( unNormalMap, texc ).rbg * 2.0 - 1.0 );
	float	max_dist	= max( 	max( Input[0].fDistance, Input[1].fDistance ),
								max( Input[2].fDistance, Input[3].fDistance ) );
	Output.iLayerBits	= int(max_dist < unFarPlanes[0]*1.3) | (1<<int(max_dist >= unFarPlanes[0]*0.98));
	
	pos.xyz += PCF( texc ) * norm * unHeightScale;
	gl_Position = pos;
}


//-----------------------------------------------------------------------------
--geometry
#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4	unMVPMatrices[2];
uniform vec2	unFarPlanes;

in	TEvalData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
	float	fDistance;
	int		iLayerBits;
} Input[3];

out	TGeomData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Output;


void Vertex(int i)
{
	gl_Position			= unMVPMatrices[gl_Layer] * gl_in[i].gl_Position;
	gl_Position.z 		= (gl_Position.z / unFarPlanes[gl_Layer] - 1.0) * gl_Position.w;
	Output.vNormal		= Input[i].vNormal;
	Output.vTexcoord0	= Input[i].vTexcoord0;
	Output.fLevel		= Input[i].fLevel;
	EmitVertex();
}

void main()
{
	float	max_dist = max( Input[0].fDistance, max( Input[1].fDistance, Input[2].fDistance ) );
	//int		bits = Input[0].iLayerBits | Input[1].iLayerBits | Input[2].iLayerBits;
	
	//if ( (bits & 1) != 0 )
	if ( max_dist < unFarPlanes[0]*1.01 )
	{
		gl_Layer = 0;
		Vertex( 0 );
		Vertex( 1 );
		Vertex( 2 );
		EndPrimitive();
	}
	
	//if ( (bits & 2) != 0 )
	if ( max_dist >= unFarPlanes[0]*0.95 )
	{
		gl_Layer = 1;
		Vertex( 0 );
		Vertex( 1 );
		Vertex( 2 );
		EndPrimitive();
	}
}


//-----------------------------------------------------------------------------
--fragment
#version 330 core

layout(location = 0) out vec4	outColor;

uniform sampler2D	unDiffuseMap;
uniform float		unMaxTessLevel;

in	TGeomData {
	vec3	vNormal;
	vec2	vTexcoord0;
	float	fLevel;
} Input;


void main()
{
	const vec3	const_norm 	= normalize( vec3( 0.f, 0.5f, 1.f ) );
	
	outColor.rgb	= texture( unDiffuseMap, Input.vTexcoord0 ).rgb;
	outColor.rgb	*= clamp( dot( const_norm, Input.vNormal ), 0.1, 1.0 );
	outColor.a		= 1.0;
}

--eof
