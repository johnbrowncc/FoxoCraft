#version 460 core

layout (location = 0) in vec3 vert_Position;
layout (location = 1) in vec3 vert_Normal;
layout (location = 2) in vec3 vert_TexCoord;

out vec3 frag_TexCoord;
out vec3 frag_Normal;

uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat4 u_Projection;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * vec4(vert_Position, 1.0);
	frag_Normal = vert_Normal;
	frag_TexCoord = vert_TexCoord;
}