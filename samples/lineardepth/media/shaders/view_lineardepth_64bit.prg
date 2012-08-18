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

uniform sampler2DArray	unDiffuseMap;
uniform sampler2DArray	unDepthMap;

in vec2		vTexcoord;

void main()
{
	vec4	color0 = texture( unDiffuseMap, vec3(vTexcoord,0.0) );
	float	depth0 = texture( unDepthMap, vec3(vTexcoord,0.0) ).r;
	float	depth1 = texture( unDepthMap, vec3(vTexcoord,1.0) ).r;

	outColor = color0.a == 0.0 ? vec4( 0.1, depth1, 0.4, 1.0 ) : vec4( depth0, 0.3, 0.0, 1.0 );
}

--eof
