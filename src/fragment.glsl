#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D gameTexture;

void main(){
	color = texture(gameTexture, UV).rgb;
}