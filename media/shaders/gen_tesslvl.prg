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
uniform float		unHeightScale;
uniform float		unGridScale;

in vec2		vTexcoord;

#define DeltaHeightInCol( _col ) \
	(	abs( mHeight[_col][0] - mHeight[_col][1] ) + \
		abs( mHeight[_col][1] - mHeight[_col][2] ) + \
		abs( mHeight[_col][2] - mHeight[_col][3] ) )
		
#define DeltaHeightInRow( _row ) \
	(	abs( mHeight[0][_row] - mHeight[1][_row] ) + \
		abs( mHeight[1][_row] - mHeight[2][_row] ) + \
		abs( mHeight[2][_row] - mHeight[3][_row] ) )


void ReadHeight(out mat4 mHeight)
{
	mHeight[0] = textureGatherOffsets( unHeightMap, vTexcoord, ivec2[]( ivec2(0,0), ivec2(0,1), ivec2(0,2), ivec2(0,3) ) );
	mHeight[1] = textureGatherOffsets( unHeightMap, vTexcoord, ivec2[]( ivec2(1,0), ivec2(1,1), ivec2(1,2), ivec2(1,3) ) );
	mHeight[2] = textureGatherOffsets( unHeightMap, vTexcoord, ivec2[]( ivec2(2,0), ivec2(2,1), ivec2(2,2), ivec2(2,3) ) );
	mHeight[3] = textureGatherOffsets( unHeightMap, vTexcoord, ivec2[]( ivec2(3,0), ivec2(3,1), ivec2(3,2), ivec2(2,3) ) );
}

float GenTessLevel(in mat4 mHeight)
{
	float	delta = DeltaHeightInCol( 0 ) + DeltaHeightInCol( 1 ) + DeltaHeightInCol( 2 ) +
					DeltaHeightInRow( 0 ) + DeltaHeightInRow( 1 ) + DeltaHeightInRow( 2 );
	return clamp( delta / 24.0 * unGridScale*128.0 / unHeightScale, 0.0, 1.0 );
}

void main()
{
	mat4	height;
	ReadHeight( height );
	outTessLevel = vec4( GenTessLevel( height ) );
}

--eof
