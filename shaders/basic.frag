#version 330 core

in vec2 TexCoord;
out vec4 frag_color;
uniform sampler2D my_texture;
uniform sampler2D my_texture2;

void main()
{
	frag_color = mix(texture(my_texture, TexCoord), texture(my_texture2, TexCoord), 0.5);
}