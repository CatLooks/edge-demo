#pragma once

/// User interface namespace.
namespace UI {
	/// import texture map
	using Textures::TexMap;

	/// Rendered sprite count.
	size_t sprite_count = 0;

	/// Vertex object
	struct Vertex {
		glm::vec2 pos = glm::vec2(0.0f); // Vertex position.
		glm::vec2 tex = glm::vec2(0.0f); // Vertex texture.

		/// Default empty constructor.
		Vertex () {};
		/// Default constructor.
		/// @param pos Vertex position.
		/// @param tex Vertex texture position.
		Vertex (glm::vec2 pos, glm::vec2 tex): pos(pos), tex(tex) {};
	};

	/// Sprite object
	class Sprite {
		protected:
		/// Sprite indices.
		constexpr static GLuint m_inds[6] { 0, 1, 3, 0, 3, 2 };

		/// Vertex buffer object.
		VertexBuffer m_buffer;
		/// Vertex layout object.
		VertexLayout m_layout;

		public:
		/// Object position.
		sf::Vector2i pos = sf::Vector2i(0, 0);
		/// Object size.
		sf::Vector2i size = sf::Vector2i(1, 1);
		/// Object color.
		glm::vec3 color = glm::vec3(1.0f);
		/// Object alpha.
		float alpha = 1.0f;

		/// Default empty constructor.
		Sprite (): m_buffer(NULL, 0, GL_DYNAMIC_DRAW), m_layout(GL_TRIANGLES) {
			// configure vertex layout
			m_layout.bind();
			m_layout.link(0, 2, GL_FLOAT, 4 * sizeof(GL_FLOAT), 0 * sizeof(GL_FLOAT));
			m_layout.link(1, 2, GL_FLOAT, 4 * sizeof(GL_FLOAT), 2 * sizeof(GL_FLOAT));
			m_layout.count = 4;
			m_layout.unbind();

			// generate empty buffer
			const Vertex buffer[4] {
				Vertex({0.0f, 1.0f}, {0.0f, 0.0f}),
				Vertex({1.0f, 1.0f}, {0.0f, 0.0f}),
				Vertex({0.0f, 0.0f}, {0.0f, 0.0f}),
				Vertex({1.0f, 0.0f}, {0.0f, 0.0f}),
			};

			// update vertex buffer
			m_buffer.reset((const float*)buffer, sizeof(buffer));
		};

		/// Sets sprite's texture positions.
		/// @param texPos Texture origin position.
		/// @param texSize Texture size.
		void set(TexMap map) {
			// generate vertex buffer
			const Vertex buffer[4] {
				Vertex({0.0f, 1.0f}, map.coords[0]),
				Vertex({1.0f, 1.0f}, map.coords[1]),
				Vertex({0.0f, 0.0f}, map.coords[2]),
				Vertex({1.0f, 0.0f}, map.coords[3]),
			};

			// update vertex buffer
			m_buffer.sub((const float*)buffer);
		};

		/// Draws the sprite.
		void draw() const {
			Shaders::gui.use();

			// get uniform locations
			static GLuint uf_translate = glGetUniformLocation(Shaders::gui.ID, "translate");
			static GLuint uf_transform = glGetUniformLocation(Shaders::gui.ID, "transform");
			static GLuint uf_color = glGetUniformLocation(Shaders::gui.ID, "color");

			// update sprite translation
			glm::vec2 pos = glm::vec2((float)this->pos.x, (float)this->pos.y);
			glUniform2fv(uf_translate, 1, glm::value_ptr(pos));

			// update transform matrix
			glm::mat2 transform = glm::mat2(1.0f);
			transform[0][0] = (float)size.x;
			transform[1][1] = (float)size.y;
			glUniformMatrix2fv(uf_transform, 1, GL_FALSE, glm::value_ptr(transform));

			// update sprite color
			glm::vec4 full = glm::vec4(color, alpha);
			glUniform4fv(uf_color, 1, glm::value_ptr(full));

			// draw sprite
			m_layout.drawIndex(m_inds, lengthof(m_inds));
			sprite_count++;
		};
	};

	/// Rendering sprite object.
	static Sprite* sprite_obj = NULL;

	/// Returns sprite rendering object.
	Sprite& sprite() {
		if (sprite_obj == NULL)
			sprite_obj = new Sprite();
		return *sprite_obj;
	};

	/// Unloads rendering sprite object.
	void unload() {
		if (sprite_obj) {
			delete sprite_obj;
			sprite_obj = NULL;
		};
	};
};