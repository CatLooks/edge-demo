#pragma once

/// Camera target view object.
struct CameraTarget {
	/// Target offset.
	glm::vec3 off;
	/// Target viewing FOV.
	float fov;
	/// Transition time.
	float time;
};

/// Camera object.
struct Camera {
	/// Camera position.
	glm::vec3 pos;
	/// Camera viewing offset.
	glm::vec3 off;
	/// Camera "up" direction.
	glm::vec3 up;
	/// Camera viewing FOV.
	float fov;

	/// Camera viewing starting state.
	CameraTarget src;
	/// Camera viewing target.
	CameraTarget dest;
	/// Camera target mode.
	bool mode;
	/// Interpolation time.
	float time;
	/// Whether the camera interpolation is happening.
	bool interpolation;
	/// Camera view matrix.
	glm::mat4 view;
	/// Camera projection matrix.
	glm::mat4 proj;

	/// Default camera constructor.
	/// @param FOV Camera FOV.
	Camera (float FOV):
		pos(glm::vec3(0.0f, 0.0f, 0.0f)),
		off(glm::vec3(0.0f, 0.0f, 1.0f)),
		up (glm::vec3(0.0f, 1.0f, 0.0f)),
		fov(FOV), interpolation(false) {};

	/// Updates camera state.
	/// @param delta Time elapsed since last frame.
	void update(float delta) {
		// check for no active interpolation
		if (!interpolation) return;

		// update interpolation time
		time += delta;

		// check for bounds
		if (time >= dest.time) {
			interpolation = false;
			time = dest.time;
		};

		// calculate intermediate state
		float t = Easings::sineOut(time / dest.time);

		// interpolate offset
		if (mode) {
			off = lerpv(src.off, dest.off, t);
			return;
		};

		// interpolate FOV
		fov = lerpf(src.fov, dest.fov, t);

		// calculate smooth offset transition
		float norm = tanf(glm::radians(src.fov) / 2.0f) / tanf(glm::radians(fov) / 2.0f);
		off = lerpv(src.off, dest.off, t) * norm;
	};

	/// Sets camera offset and FOV.
	/// @param off Unnormalized offset.
	/// @param fov Camera FOV.
	void set(glm::vec3 off, float fov) {
		float norm = tanf(glm::radians(this->fov)) / tanf(glm::radians(fov) / 2.0f);
		this->fov = fov;
		this->off = off * norm;
	};

	/// Prepares camera offset interpolation.
	/// @param off New camera offset.
	/// @param time Transition time.
	void targetOff(glm::vec3 off, float time) {
		// calculate normalized offset
		float N = 0.5f / tanf(glm::radians(fov) / 2.0f);

		// prepare interpolation
		src = { this->off, fov, 0.0f };
		dest = { off * N, fov, time };
		interpolation = true;
		this->time = 0.0f;
		mode = true;
	};

	/// Prepares camera FOV interpolation.
	/// @param fov New camera FOV.
	/// @param time Transition time.
	void targetFOV(float fov, float time) {
		src = { off, this->fov, 0.0f };
		dest = { off, fov, time };
		interpolation = true;
		this->time = 0.0f;
		mode = false;
	};

	/// Returns a camera state object.
	CameraTarget store() const {
		if (interpolation)
			return dest;
		return CameraTarget { off, fov, 0.0f };
	};

	/// Restores camera state.
	/// @param state Camera state returned by `store()`.
	void restore(CameraTarget state) {
		off = state.off;
		fov = state.fov;
		interpolation = false;
	};

	/// Recalculates camera matrix.
	/// @param menu Whether to shift camera for a side menu display.
	void recalculate() {
		// get window aspect ratio
		sf::Vector2u winsize = (sf::Vector2u)app.getSize();
		float aspect = (float)winsize.x / (float)winsize.y;

		// calculate view matrix
		view = glm::lookAt(pos + off, pos, up);

		// calculate projection bounds
		float zN = 0.02f * off.x;
		float zF = 20.0f * off.x;

		// calculate projection matrix
		proj = glm::perspective(glm::radians(fov), aspect, zN, zF);
	};

	/// Computes screen bounding box of a volume.
	/// @param volume Target volume.
	/// @return 2D bounding box.
	sf::IntRect vertBounds(const vol3x& volume) {
		// min / max points
		sf::Vector2i min;
		sf::Vector2i max;

		// whether to override min / max
		bool overwrite = true;

		// recalculate camera
		recalculate();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
			printf("?\n");

		// process each vertex
		for (uint8_t i = 0; i < 8; i++) {
			// get vertex projection
			glm::vec3 conv = Models::vertID(volume.pos.gl(), volume.out().gl(), i);
			glm::vec3 fpos = glm::project(
				conv, view, proj,
				glm::vec4(0.0f, 0.0f, (float)Win::size.x, (float)Win::size.y)
			);

			// remap to screen coordinates
			sf::Vector2i coords = { (int)fpos.x, (int)Win::size.y - (int)fpos.y };

			// update min / max points
			if (overwrite) {
				overwrite = false;
				min = coords;
				max = coords;
			} else {
				min.x = min.x <= coords.x ? min.x : coords.x;
				min.y = min.y <= coords.y ? min.y : coords.y;
				max.x = max.x >= coords.x ? max.x : coords.x;
				max.y = max.y >= coords.y ? max.y : coords.y;
			};
		};

		// return bounding box
		return sf::IntRect(min, max - min);
	};

	/// Updates matrix uniform.
	/// @param shader Shader program.
	/// @param name Uniform name.
	void uniform(Shader& shader, const char* name) {
		shader.use();
		glm::mat4 mat = proj * view;
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, name), 1, GL_FALSE, glm::value_ptr(mat));
	};
};