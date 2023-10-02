#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 mvp;
uniform mat4 u_proj;
uniform mat4 u_model;

void main()
{
	gl_Position = u_proj * u_model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

#fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 vertexColor;

// texture samplers
uniform sampler2D u_texture;

void main()
{
	FragColor = texture(u_texture, TexCoord);
}