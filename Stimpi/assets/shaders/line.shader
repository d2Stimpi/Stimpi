#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec4 vertexColor;

uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * vec4(aPos, 1.0);
    
    vertexColor = vec4(1.0);
    vertexColor.rgb = aColor;
}

#fragment
#version 330 core
out vec4 FragColor;

in vec4 vertexColor;

void main()
{
    // Set output color
    FragColor = vertexColor;
}