#pragma once

/// Vertex object.
struct Vertex {
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f); /// Vertex position.
	glm::vec3 col = glm::vec3(1.0f, 1.0f, 1.0f); /// Vertex color.
	glm::vec2 tex = glm::vec2(0.0f, 0.0f)      ; /// Vertex texture UV.

	/// Default empty constructor.
	Vertex () {};
	/// Vertex constructor.
	/// @param pos Vertex position.
	Vertex (glm::vec3 pos): pos(pos) {};
	/// Vertex constructor.
	/// @param pos Vertex position.
	/// @param col Vertex color.
	Vertex (glm::vec3 pos, glm::vec3 col): pos(pos), col(col) {};
	/// Vertex constructor.
	/// @param pos Vertex position.
	/// @param tex Vertex texture UV.
	Vertex (glm::vec3 pos, glm::vec2 tex): pos(pos), tex(tex) {};
	/// Vertex constructor.
	/// @param pos Vertex position.
	/// @param col Vertex color.
	/// @param tex Vertex texture UV.
	Vertex (glm::vec3 pos, glm::vec3 col, glm::vec2 tex): pos(pos), col(col), tex(tex) {};

	/// Sets vertex position.
	/// @param pos New position.
	Vertex& setPos(glm::vec3 pos) { this->pos = pos; return *this; };
	/// Sets vertex color.
	/// @param col New color.
	Vertex& setCol(glm::vec3 col) { this->col = col; return *this; };
	/// Sets vertex texture UV.
	/// @param tex New texture position.
	Vertex& setTex(glm::vec2 tex) { this->tex = tex; return *this; };

	/// Returns `float` pointer to vertex data.
	float* data() const { return (float*)this; };
};

/// Vertex buffer object.
struct VertexBuffer {
	/// Vertex buffer ID.
	GLuint ID;
	/// Vertex buffer size.
	GLsizeiptr size;
	/// Vertex buffer usage type.
	GLenum usage;

	/// Vertex buffer constructor.
	/// @param vertices Vertices data.
	/// @param size Size of data.
	/// @param usage Buffer usage type.
	VertexBuffer (const float* vertices, GLsizeiptr size, GLenum usage): size(size), usage(usage) {
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
	};

	/// Substitutes vertex buffer data.
	/// @param vertices Vertex data pointer.
	void sub(const float* vertices) const {
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};

	/// Allocates a new vertex buffer.
	/// @param vertices Vertex data pointer.
	/// @param size Vertex data size.
	void reset(const float* vertices, GLsizeiptr size) {
		this->size = size;
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};

	/// Binds vertex buffer.
	void bind  () const { glBindBuffer(GL_ARRAY_BUFFER, ID); };
	/// Unbinds vertex buffer.
	void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0 ); };

	/// Deallocates vertex buffer.
	~VertexBuffer () { glDeleteBuffers(1, &ID); };
};

/// Vertex buffer layout.
struct VertexLayout {
	/// Vertex layout ID.
	GLuint ID;
	/// Vertex count.
	GLsizei count;
	/// Vertex connection type.
	GLenum prim;

	/// Empty vertex layout constructor.
	VertexLayout (GLenum primitives) {
		glGenVertexArrays(1, &ID);
		count = 0;
		prim = primitives;
	};

	/// Links a layout.
	/// @param loc Layout location.
	/// @param width Data size.
	/// @param type Data type.
	/// @param stride Layout block size.
	/// @param offset Layout block offset.
	void link(GLuint loc, GLuint width, GLenum type, GLsizeiptr stride, GLsizeiptr offset) const {
		glVertexAttribPointer(loc, width, type, GL_FALSE, stride, (void*)offset);
		glEnableVertexAttribArray(loc);
	};

	/// Draws buffer as vertices.
	/// @return Drawn vertex count.
	size_t draw() const {
		glBindVertexArray(ID);
		glDrawArrays(prim, 0, count);
		glBindVertexArray(0);
		return count;
	};

	/// Draws buffer with index buffer.
	size_t drawIndex(const GLuint* indices, GLsizeiptr count) const {
		glBindVertexArray(ID);
		glDrawElements(prim, count, GL_UNSIGNED_INT, indices);
		glBindVertexArray(0);
		return count;
	};

	/// Binds vertex layout.
	void bind  () const { glBindVertexArray(ID); };
	/// Unbinds vertex layout.
	void unbind() const { glBindVertexArray(0 ); };

	/// Deallocates vertex layout.
	~VertexLayout () { glDeleteVertexArrays(1, &ID); };
};