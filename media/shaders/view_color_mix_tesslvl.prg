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

uniform sampler2D	unDiffuseMap;
uniform sampler2D	unNormalMap;

in vec2		vTexcoord;

const int		colors = 4;
const vec3		vColors[colors] = vec3[] (
					vec3( 1.0, 0.0, 0.0 ),
					vec3( 0.8, 1.0, 0.0 ),
					vec3( 0.0, 0.8, 1.0 ),
					vec3( 0.0, 0.0, 0.5 )
				);
				
vec3 GetColor(float f)
{
	float	a = f * colors;
	int		i = int( clamp( a, 0.0, float(colors-2) ) );
	
	a -= float(i);
	
	return mix( vColors[i], vColors[i+1], a );
}

void main()
{
	const vec3	const_norm 	= normalize( vec3( 0.f, 0.5f, 1.f ) );
	vec4		data 		= texture( unNormalMap, vTexcoord );
	vec3		norm		= data.rgb * 2.0 - 1.0;
	
	if ( vTexcoord.x > 0.5 ) {
		outColor.rgb = GetColor( data.a );
	} else {
		outColor = texture( unDiffuseMap, vTexcoord );
		outColor.rgb *= clamp( dot( norm, const_norm ), 0.15, 1.0 );
	}
	outColor.a = 1.0;
}

--eof
