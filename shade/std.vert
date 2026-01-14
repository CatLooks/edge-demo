#version 330 core

// vertex data layout
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertCol;
layout (location = 2) in vec2 vertTex;

// color data layout
out vec4 pixCol;
out vec2 pixTex;

// world camera matrix
uniform mat4 camera;

// object properties
uniform mat4 object;
uniform vec3 color;
uniform float alpha;

/// Vertex shader code.
void main() {
	// calculate position
	gl_Position = camera * object * vec4(vertPos, 1.0f);

	// pipe color data
	pixCol = vec4(vertCol * color, alpha);
	pixTex = vertTex;
}