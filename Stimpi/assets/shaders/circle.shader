#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aThickness;
layout(location = 4) in float aFade;

out vec2 texCoord;
out vec4 vertexColor;
out float thickness;
out float fade;

uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * vec4(aPos, 1.0);
	texCoord = aTexCoord;
    
    vertexColor = vec4(1.0);
    vertexColor.rgb = aColor;

    thickness = aThickness;
    fade = aFade;
}

#fragment
#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec4 vertexColor;
in float thickness;
in float fade;

uniform float u_thickness;
uniform float u_fade;

void main()
{
    // Calculate distance and fill circle with white
    float distance = 1.0 - length(texCoord);
    float circle = smoothstep(0.0, fade, distance);
    circle *= smoothstep(thickness + fade, thickness, distance);
    
    // Set output color
    FragColor = vertexColor;
    FragColor.a *= circle;
}