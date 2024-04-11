#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform sampler2D outlineTexture;

void main() 
{ 
	vec4 rgba = texture(outlineTexture, TexCoords);

	// check if the pixel is part of the outline
	if (rgba.r > 0.5) // the background of the screen need to be under 0.5 rgb
	{
		FragColor = rgba;
	}
	else
	{
		FragColor = texture(sceneTexture, TexCoords);
	}
}