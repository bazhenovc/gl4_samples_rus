/** Shader for basic sample */

--vertex
#version 330 core

uniform mat4 mvp;

layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec2 in_texcoord;
layout (location = 2) in vec3 in_normal;

out vec3 fs_normal;

void main()
{
	gl_Position = mvp * vec4(in_vertex, 1.0);
}

--fragment
#version 330 core

layout (location = 0) out vec4 color;

void main()
{
	color = vec4(0, 1, 0, 1);
}

--eof
