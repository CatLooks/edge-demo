#version 330 core

// piped position data
in vec2 tex;

// fragment color output
layout (location = 0) out vec4 pixel;

/// Fragment shader code.
void main() {
	// set output color
	pixel = vec4(0, 0, 0, 96.0f / 255.0f);
}