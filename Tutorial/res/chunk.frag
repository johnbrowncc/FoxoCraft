#version 460 core

in vec3 frag_Normal;
in vec3 frag_TexCoord;

layout (location = 0) out vec4 out_Color;

uniform sampler2DArray u_Albedo;

void main()
{
	float nDot1 = max(dot(frag_Normal, -normalize(vec3(2, -10, 3))), 0.2);

	out_Color = texture(u_Albedo, frag_TexCoord);
	out_Color.rgb *= nDot1;
}