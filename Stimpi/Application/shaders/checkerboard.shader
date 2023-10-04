#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 u_Projection;

void main()
{
	// Only using Proj mat since this is for background and not movable render
	gl_Position = u_Projection * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}

#fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D u_texture;
uniform vec2 u_resolution;

void main()
{
	const float scale = 40.0;
	const float brightness = 0.6;
	vec2 pos = vec2(u_resolution.x * TexCoord.x, u_resolution.y * TexCoord.y);
	vec2 b = step(fract(pos / scale), vec2(0.5));
	FragColor = vec4(vec3(fract(dot(b, vec2(1.0)) * 0.5)) + brightness, 1.0);
}