
--vertex
#version 330 core

layout (location = 0) in vec3 	inPosition;
layout (location = 1) in vec2 	inTexcoord;
layout (location = 2) in vec3 	inNormal;
layout (location = 3) in vec3	inTangent;
layout (location = 4) in vec3	inBinormal;

uniform mat4 unMVPMatrix;

out vec3	vNormal;

void main()
{
	vNormal		= inNormal;
	gl_Position	= unMVPMatrix * vec4(inPosition, 1.0);
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

in  vec3	vNormal;

void main()
{
	outColor = Mtr.vDiffuse;
	outColor.a = 0.3;
}

--eof
