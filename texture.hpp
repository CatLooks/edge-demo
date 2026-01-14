#pragma once

/// Texture container.
struct Tex {
	/// Texture ID.
	GLuint ID;

	/// Empty texture constructor.
	Tex (): ID(0) {};

	/// Texture constructor.
	/// @param img Texture image.
	/// @param slot Texture unit slot.
	/// @param filter Texture filtering.
	/// @param w Interpreted width (0 for default).
	/// @param h Interpreted height (0 for default).
	void set(const sf::Image& img, GLenum slot, GLenum filter) {
		// free previous texture
		this->~Tex();

		// generate texture
		glGenTextures(1, &ID);
		glActiveTexture(slot);
		glBindTexture(GL_TEXTURE_2D, ID);

		// configure texture settings
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// upload texture data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
		glBindTexture(GL_TEXTURE_2D, 0);
	};

	/// Uses texture slot for rendering.
	/// @param shader Shader program.
	/// @param name Texture uniform name.
	/// @param unit Texture unit slot.
	static void useSlot(Shader& shader, const char* name, GLuint unit) {
		shader.use();
		GLuint uniform = glGetUniformLocation(shader.ID, name);
		glUniform1i(uniform, unit);
	};

	/// Binds texture.
	void bind () const {
		static GLuint last = -1;
		if (last != ID)
			glBindTexture(GL_TEXTURE_2D, last = ID);
	};
	/// Unbinds texture.
	void unbind() const { glBindTexture(GL_TEXTURE_2D, 0 ); };

	/// Deallocates texture.
	~Tex () { glDeleteTextures(1, &ID); };
};