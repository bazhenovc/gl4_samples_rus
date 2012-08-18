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

uniform sampler2D	unDepthMap;

in vec2		vTexcoord;

void main()
{
	float	depth = texture( unDepthMap, vTexcoord ).r;
	
	outColor = vec4(depth);
	//outColor = depth > 0.01 && depth < 1.0 ? vec4(depth,0.5,0.0,0.0) : vec4(0.0,depth,0.5,0.0);
}

--eof
