#version 330 core

// color data layout
in vec4 pixCol;
in vec2 pixTex;

// fragment color output
layout (location = 0) out vec4 pixel;

/// Fragment shader code.
void main() {
	// shade pixel with color
	pixel = pixCol;
}