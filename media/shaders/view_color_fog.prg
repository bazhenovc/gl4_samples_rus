/*
	
*/

--vertex
#version 410 core

layout(location = 0)	in vec2 inPosition;		// [-1,1]

uniform mat4	unMVPMatrix;

out vec2	vTexcoord;
out vec3	vPosition;

void main()
{
	gl_Position = unMVPMatrix * vec4( inPosition, 0.0, 1.0 );
	vPosition	= gl_Position.xyz / gl_Position.w;
	vTexcoord	= inPosition * 0.5 + 0.5;
}


//-----------------------------------------------------------------------------
--fragment
#version 410 core

layout(location = 0) out vec4	outColor;

uniform sampler2D	unDiffuseMap;
uniform sampler2D	unNormalMap;
uniform sampler2D	unDepthMap;
uniform mat4		unProjInvMatrix;

in vec2		vTexcoord;
in vec3		vPosition;


vec3 UnpackMVpos(in vec2 scrPos, in vec2 texcoord)
{
	float	depth	= texture( unDepthMap, texcoord ).r * 2.0 - 1.0;
	vec4	pos		= unProjInvMatrix * vec4( scrPos, depth, 1.0 );
	return pos.xyz / pos.w;
}

void main()
{
	const vec3	const_norm 	= normalize( vec3( 0.f, 0.5f, 1.f ) );
	const vec3	fog_color	= vec3( 0.2, 0.25, 0.3 );
	const vec2	fog			= vec2( 100.0, 500.0 );
	vec3		norm 		= texture( unNormalMap, vTexcoord ).rgb * 2.0 - 1.0;
	float		depth		= UnpackMVpos( vPosition.xy, vTexcoord ).z;
	float		f			= 1.0 - clamp( exp( depth * 0.001 ), 0.0, 1.0 );
	
	
	outColor = texture( unDiffuseMap, vTexcoord );
	outColor.rgb *= clamp( dot( norm, const_norm ), 0.15, 1.0 );
	outColor.rgb = mix( outColor.rgb, fog_color, f );
	outColor.a = 1.0;
}

--eof
