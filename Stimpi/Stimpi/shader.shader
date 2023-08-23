#version 330 core
layout(location = 0) in vec3 aPos;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
}

#fragment
#version 330 core
void main()
{
	FragColor = texture(texture1, TexCoord);
}