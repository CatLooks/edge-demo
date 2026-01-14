#pragma once

/// 3D object structure.
class Object {
	protected:
	/// Object vertices.
	std::vector<Vertex> verts;
	/// Object indices.
	std::vector<GLuint> indices;

	/// Vertex buffer object.
	VertexBuffer buffer;
	/// Vertex layout object.
	VertexLayout layout;

	/// Generates object layout.
	void genLayout() {
		layout.link(0, 3, GL_FLOAT, 8 * sizeof(float), 0 * sizeof(float));
		layout.link(1, 3, GL_FLOAT, 8 * sizeof(float), 3 * sizeof(float));
		layout.link(2, 2, GL_FLOAT, 8 * sizeof(float), 6 * sizeof(float));
	};

	/// Generates vertex data.
	/// @param vert Vertex.
	/// @param floats Data buffer.
	static void genVertex(const Vertex& vert, std::vector<float>& floats) {
		const float* repr = (const float*)&vert;
		floats.insert(floats.end(), repr, repr + sizeof(repr));
	};

	public:
	/// Object position.
	glm::vec3 pos;
	/// Object rotation around "up" axis.
	float rotZ;
	/// Object scale.
	glm::vec3 scale;
	/// Object shade color.
	glm::vec3 color;
	/// Object alpha.
	float alpha;
	/// Custom object vertex index count.
	GLuint idCount;

	/// Default empty object constructor.
	Object (GLenum primitive = GL_TRIANGLES):
		buffer((const float*)verts.data(), 0, GL_DYNAMIC_DRAW), layout(primitive),
		pos(glm::vec3(0.0f)), rotZ(0.0f), scale(glm::vec3(1.0f)), color(glm::vec3(1.0f)), alpha(1.0f),
		idCount(0) {
		layout.bind();
		genLayout();
		layout.count = 0;
		layout.unbind();
	};

	/// Updates object geometry.
	/// @param verts Vertex array.
	/// @param count Vertex count.
	void set(const Vertex* verts, size_t count) {
		// update vertices
		buffer.reset((const float*)verts, sizeof(Vertex) * count);
		layout.count = count;

		// copy data into local storage
		this->verts = std::vector<Vertex>(verts, verts + count);
	};
	/// Updates object geometry.
	/// @param verts Vertex vector.
	void set(const std::vector<Vertex>& verts) { set(verts.data(), verts.size()); };

	/// Updates vertex indices.
	/// @param indices Index array.
	/// @param count Index count.
	void index(const GLuint* indices, size_t count) {
		this->indices = std::vector<GLuint>(indices, indices + count);
	};

	/// Updates vertex indices.
	/// @param indices Index vector.
	void index(const std::vector<GLuint>& indices) { index(indices.data(), indices.size()); };

	/// Generate vertex index table for quads.
	/// @note Vertex ordering should be:
	/// 2---3
	/// |   |
	/// 0---1
	/// @note Generated triangles are `(0, 2, 3)` and `(0, 3, 1)`.
	void indexQuads() {
		indices.clear();
		GLuint quad_count = verts.size() / 4;

		// check for leftover vertices
		GLuint left = verts.size() - quad_count * 4;
		if (left != 0) {
			Log::out.symW();
			Log::out.format("Vertex buffer misalignment during quad indexing (%d unused vert%s).",
				left, left == 1 ? "ex" : "ices");
			Log::out.print();
		};

		// generate indices
		for (GLuint q = 0; q < quad_count; q++) {
			GLuint i = q * 4;
			pushQuad(indices, i, i + 1, i + 2, i + 3);
		};
	};

	/// Generate vertex index table for triangles.
	void indexTriangles() {
		indices.clear();
		GLuint quad_count = verts.size() / 3;

		// check for leftover vertices
		GLuint left = verts.size() - quad_count * 3;
		if (left != 0) {
			Log::out.symW();
			Log::out.format("Vertex buffer misalignment during triangle indexing (%d unused vert%s).",
				left, left == 1 ? "ex" : "ices");
			Log::out.print();
		};

		// generate indices
		for (GLuint q = 0; q < quad_count * 3; q++)
			indices.push_back(q);
	};

	/// Pushes a quad into index array.
	/// @param inds Index array.
	/// @param v0 Bottom-left vertex index.
	/// @param v1 Bottom-right vertex index.
	/// @param v2 Top-left vertex index.
	/// @param v3 Top-right vertex index.
	static void pushQuad(std::vector<GLuint>& inds, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
		inds.insert(inds.end(), {
			v0, v1, v3, v0, v3, v2
		});
	};

	/// Returns vertex buffer.
	Vertex* vertices() { return verts.data(); };

	/// Returns vertex buffer.
	const Vertex* vertices() const { return verts.data(); };

	/// Return vertex list.
	const std::vector<Vertex>& vertlist() const { return verts; };

	/// Updates vertices from vertex buffer.
	void update() const {
		std::vector<float> floats;
		for (const Vertex& vert : verts)
			genVertex(vert, floats);
		buffer.sub(floats.data());
	};

	/// Generates a model matrix.
	glm::mat4 matrix() const {
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, pos);
		transform = glm::scale(transform, scale);
		transform = glm::rotate(transform, rotZ, glm::vec3(0.0f, 1.0f, 0.0f));
		return transform;
	};

	/// Draws object.
	/// @param Shader program.
	/// @return Drawn vertex count.
	size_t draw(const Shader& shader) const {
		shader.use();

		// update object transform matrix
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "object"), 1, GL_FALSE, glm::value_ptr(matrix()));

		// update object color & alpha
		glUniform3fv(glGetUniformLocation(shader.ID, "color"), 1, glm::value_ptr(color));
		glUniform1f(glGetUniformLocation(shader.ID, "alpha"), alpha);

		// draw object
		if (idCount == 0)
			return layout.drawIndex(indices.data(), indices.size());
		return layout.drawIndex(indices.data(), idCount);
	};
};