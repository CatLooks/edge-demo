#version 330 core

// vertex data layout
layout (location = 0) in vec2 vertPos;

// position data pipe
out vec2 tex;

/// Vertex shader code.
void main() {
	// set vertex position
	gl_Position = vec4(vertPos, -1.0f, 1.0f);

	// pipe position vector
	tex = vertPos * 0.5f;
}