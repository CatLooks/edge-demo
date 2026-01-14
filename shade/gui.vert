#version 330 core

// vertex data layout
layout (location = 0) in vec2 vertPos;
layout (location = 1) in vec2 vertTex;

// color data layout
out vec4 pixCol;
out vec2 pixTex;

// display window size
uniform uint winX;
uniform uint winY;

// model configuration
uniform vec2 translate;
uniform mat2 transform;
uniform vec4 color;

/// Vertex shader code.
void main() {
	// calculate 2D position
	vec2 pos = translate + transform * vertPos;

	// calculate 3D position
	gl_Position = vec4(
		pos.x * 2.0f / winX - 1.0f,
		1.0f - pos.y * 2.0f / winY,
		-1.0f, 1.0f
	);

	// pipe color data
	pixCol = color;
	pixTex = vertTex;
}