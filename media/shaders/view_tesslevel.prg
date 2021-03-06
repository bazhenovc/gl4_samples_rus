/*
	
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

layout(location = 0) out vec4	outColor;

uniform sampler2D	unNormalMap;

in vec2		vTexcoord;

const int		colors = 4;
uniform vec3	vColors[colors] = vec3[] (
					vec3( 1.0, 0.0, 0.0 ),
					vec3( 0.8, 1.0, 0.0 ),
					vec3( 0.0, 0.8, 1.0 ),
					vec3( 0.0, 0.0, 0.5 )
				);
			

vec3 GetColor(float f)
{
	float	a = clamp( f * colors, 0.0, float(colors) );
	int		i = int( clamp( a, 0.0, float(colors-2) ) );
	return mix( vColors[i], vColors[i+1], fract(a) );
}

void main()
{
	float	f = texture( unNormalMap, vTexcoord ).a;
	outColor = vec4( GetColor( f ), 1.0 );
}

--eof
