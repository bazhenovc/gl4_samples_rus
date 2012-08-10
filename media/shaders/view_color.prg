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

void main()
{
	const vec3	const_norm 	= normalize( vec3( 0.f, 0.5f, 1.f ) );
	vec3		norm 		= texture( unNormalMap, vTexcoord ).rgb * 2.0 - 1.0;
	
	outColor = texture( unDiffuseMap, vTexcoord );
	outColor.rgb *= clamp( dot( norm, const_norm ), 0.15, 1.0 );
	outColor.a = 1.0;
}

--eof
