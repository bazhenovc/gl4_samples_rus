/*
	√енерирует текстуру нормалей и уровней тессел€ции
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [-1,1]

uniform mat4	unMVPMatrix;

out vec2	vTexcoord;

void main()
{
	gl_Position = unMVPMatrix * vec4( inPosition, 0.0, 1.0 );
	vTexcoord	= inPosition * 0.5 + 0.5;
}


//-----------------------------------------------------------------------------
--fragment
#version 410 core

layout(location = 0) out vec4	outTessLevel;	// R8

uniform sampler2D	unHeightMap;
uniform float		unHeightScale	= 10.0;
uniform float		unGridScale 	= 100.0;

in vec2		vTexcoord;

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

float GenTessLevel(in mat3 mHeight)
{
	float	delta = DeltaHeightInCol( 0 ) + DeltaHeightInCol( 1 ) + DeltaHeightInCol( 2 ) +
					DeltaHeightInRow( 0 ) + DeltaHeightInRow( 1 ) + DeltaHeightInRow( 2 );
	return clamp( delta / 12.0 * 5.0, 0.0, 1.0 );
}

void main()
{
	mat3	height;
	ReadHeight( height );
	outTessLevel = vec4( GenTessLevel( height ) );
}

--eof
