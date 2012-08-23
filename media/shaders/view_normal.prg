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

void main()
{
	vec3	norm = texture( unNormalMap, vTexcoord ).rgb;
	outColor.rgb = all( equal( norm, vec3(1.0) ) ) ? vec3(0.0) : norm;
	outColor.a = 1.0;
}

--eof
