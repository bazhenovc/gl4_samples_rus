
--vertex
#version 330 core

layout (location = 0) in vec3 	inPosition;
layout (location = 1) in vec2 	inTexcoord;
layout (location = 2) in vec3 	inNormal;
layout (location = 3) in vec3	inTangent;
layout (location = 4) in vec3	inBinormal;

uniform mat4 unMVPMatrix;

out vec3	vNormal;
out vec2	vTexcoord;

void main()
{
	vNormal		 = inNormal;
	vTexcoord	 = inTexcoord;
	gl_Position	 = unMVPMatrix * vec4(inPosition, 1.0);
}

--fragment
#version 330 core

layout (location = 0) out vec4 outColor;

layout(std140)
uniform Material
{
	vec4	vDiffuse;
	vec4	vSpecular;
	vec4	vAmbient;
	vec4	vEmission;
	float	fShininess;
	float	fStrength;
} Mtr;

uniform sampler2D	unDepthMap;
uniform vec2		unViewport;

in  vec3	vNormal;
in  vec2	vTexcoord;

void main()
{
	float	depth = texture( unDepthMap, gl_FragCoord.xy / unViewport ).x;

	if ( gl_FragCoord.z >= depth )
		discard;
	
	outColor = Mtr.vDiffuse;
	outColor.a = 0.3;
}

--eof
