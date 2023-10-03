#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 u_proj;
uniform mat4 u_model;

void main()
{
	gl_Position = u_proj * u_model * vec4(aPos, 1.0);
	vertexColor = vec4(aColor, 1.0);
}

#fragment
#version 330 core
out vec4 FragColor;

in vec4 vertexColor;

void main()
{
	{
		FragColor = vertexColor;
	}
}