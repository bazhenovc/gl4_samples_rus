/** Shader for basic sample */

--vertex
#version 330 core

uniform mat4 mvp;

layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec2 in_texcoord;
layout (location = 2) in vec3 in_normal;

out vec3 fs_normal;
out vec2 fs_texcoord;

void main()
{
  fs_normal = in_normal;
	fs_texcoord = in_texcoord;
	gl_Position = mvp * vec4(in_vertex, 1.0);
}

--fragment
#version 330 core

uniform sampler2D diffuse;

in vec3 fs_normal;
in vec2 fs_texcoord;

layout (location = 0) out vec4 color;

void main()
{
	color = texture(diffuse, fs_texcoord);
}

--eof
