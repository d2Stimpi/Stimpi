#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

#fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D u_texture;

uniform float u_TilingFactor;

void main()
{
	{
		vec2 uv;

		if (u_TilingFactor == 0)
			uv = TexCoord;
		else
			uv = TexCoord / vec2(u_TilingFactor, 1.0);

		FragColor = texture(u_texture, uv);
	}
}