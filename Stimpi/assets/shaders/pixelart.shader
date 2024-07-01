#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 u_ViewProjection;
uniform vec4 u_TexelSize;

void main()
{
	gl_Position = u_ViewProjection * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y) * u_TexelSize.zw;
}

#fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D u_texture;
uniform vec4 u_TexelSize;
uniform float u_TexelsPerPixel;

void main()
{
	vec2 locationWithinTexel = fract(TexCoord);
	vec2 interpolationAmount = clamp(locationWithinTexel / u_TexelsPerPixel,
          0, .5) + clamp((locationWithinTexel - 1) / u_TexelsPerPixel + .5, 0, .5);
	vec2 finalTextureCoords = (floor(TexCoord) +
          interpolationAmount) / u_TexelSize.zw;
	FragColor = texture(u_texture, finalTextureCoords);
}