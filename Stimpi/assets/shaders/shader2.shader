#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTilingFactor;

out vec2 TexCoord;
out float TilingFactor;

uniform mat4 u_ViewProjection;

void main()
{
	TilingFactor = aTilingFactor;

	gl_Position = u_ViewProjection * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

#fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float TilingFactor;

// texture samplers
uniform sampler2D u_texture;

uniform float u_TilingFactor;

void main()
{
	{
		vec2 uv;
		float tf = TilingFactor;

		if (tf < 0) tf = 0;
		if (tf > 1) tf = 1;

		if (tf == 0)
			uv = TexCoord;
		else
			uv = TexCoord / vec2(tf, 1.0);

		FragColor = texture(u_texture, uv);
	}
}