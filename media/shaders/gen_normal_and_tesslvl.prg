/*
	√енерирует текстуру нормалей и уровней тессел€ции
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [0,1]

uniform mat4	unMVPMatrix;

out vec2	vTexcoord;

void main()
{
	gl_Position = unMVPMatrix * vec4( inPosition, 0.0, 1.0 );
	vTexcoord	= inPosition;
}


//-----------------------------------------------------------------------------
--fragment
#version 410 core

layout(location = 0) out vec4	outNormal;	// RGBA8

uniform sampler2D	unHeightMap;
uniform float		unHeightScale	= 10.0;
uniform float		unGridScale 	= 100.0;

in vec2		vTexcoord;

#define AddNormal( _x1, _y1, _x2, _y2 ) \
	v1 = vec3( _x1-1, _y1-1, mHeight[_x1][_y1] ) * scale; \
	v2 = vec3( _x2-1, _y2-1, mHeight[_x2][_y2] ) * scale; \
	normal += cross( v1 - v0, v2 - v0 )
		
#define DeltaHeightInCol( _col ) \
	(	abs( mHeight[_col][0] - mHeight[_col][1] ) + \
		abs( mHeight[_col][1] - mHeight[_col][2] ) )
		
#define DeltaHeightInRow( _row ) \
	(	abs( mHeight[0][_row] - mHeight[1][_row] ) + \
		abs( mHeight[1][_row] - mHeight[2][_row] ) )


void ReadHeight(out mat3 mHeight)
{
	mHeight[0] = textureGatherOffsets( unHeightMap, vTexcoord, ivec2[]( ivec2(-1,1),  ivec2(0,1),  ivec2(1,1),  ivec2(2,1)  ) ).rgb;
	mHeight[1] = textureGatherOffsets( unHeightMap, vTexcoord, ivec2[]( ivec2(-1,0),  ivec2(0,0),  ivec2(1,0),  ivec2(2,0)  ) ).rgb;
	mHeight[2] = textureGatherOffsets( unHeightMap, vTexcoord, ivec2[]( ivec2(-1,-1), ivec2(0,-1), ivec2(1,-1), ivec2(2,-1) ) ).rgb;
}

vec3 GetNormal(in mat3 mHeight)
{
	vec3	normal = vec3(0.0);
	vec3	scale = vec3( unGridScale, unGridScale, unHeightScale );
	vec3	v0	  = vec3( 0, 0, mHeight[1][1] );
	vec3	v1,
			v2;
	
	AddNormal( 1, 2,  0, 2 );
	AddNormal( 0, 1,  0, 0 );
	AddNormal( 1, 0,  2, 0 );
	AddNormal( 2, 1,  2, 2 );
	
	return normalize( normal * 0.25 );
}

float GenTessLevel(in mat3 mHeight)
{
	float	delta = DeltaHeightInCol( 0 ) + DeltaHeightInCol( 1 ) + DeltaHeightInCol( 2 ) +
					DeltaHeightInRow( 0 ) + DeltaHeightInRow( 1 ) + DeltaHeightInRow( 2 );
	return clamp( delta / 12.0, 0.0, 1.0 );
}

void main()
{
	mat3	height;
	ReadHeight( height );
	
	vec3	normal	= GenNormal( height );
	float	tesslvl	= GenTessLevel( height );
	
	outNormal = vec4( normal, tesslvl );
}

--eof
