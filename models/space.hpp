#pragma once

/// Model generation namespace.
namespace Models {
	/// Space color theme.
	struct Grad {
		glm::vec3 light; /// Top gradient color.
		glm::vec3 dark;  /// Bottom gradient color.

		/// Creates a space gradient from a theme.
		static Grad fromTheme(Theme theme) {
			switch (theme) {
				case White: return { RGB(0x2F4047), RGB(0x151D21) };
				case Gray : return { RGB(0x25292C), RGB(0x0C0E0E) };
				case Blue : return { RGB(0x101113), RGB(0x010202) };
				default   : return { RGB(0x000000), RGB(0x000000) };
			};
		};
	};

	/// Background rendering object.
	namespace Space {
		/// Background animation state.
		struct State {
			Grad beg; /// Starting gradient.
			Grad end; /// Ending gradient.
			Grad cur; /// Current gradient.

			float time; /// Animation time.
			float dura; /// Animation duration.
			bool  prog; /// Whether the animation is running.
		};

		/// Background quad vertices.
		glm::vec2 vertices[6] {
			{-1.0f, -1.0f}, { 1.0f, -1.0f}, { 1.0f,  1.0f},
			{-1.0f, -1.0f}, { 1.0f,  1.0f}, {-1.0f,  1.0f}
		};

		/// Background animation state.
		State state;

		/// Background vertex buffer.
		VertexBuffer* buffer = NULL;

		/// Background vertex layout.
		VertexLayout* layout = NULL;

		/// Generates background object.
		void generate() {
			// generate vertex buffer
			buffer = new VertexBuffer((const float*)vertices, sizeof(vertices), GL_DYNAMIC_DRAW);
			buffer->bind();
			// generate vertex layout
			layout = new VertexLayout(GL_TRIANGLES);

			// set layout
			layout->bind();
			layout->link(0, 2, GL_FLOAT, 2 * sizeof(float), 0);
			layout->count = 6;
			layout->unbind();

			// set buffer data
			buffer->unbind();
		};

		/// Sets background gradient colors.
		/// @param grad Space gradient color.
		/// @param time Transition time.
		void set(const Grad& grad, float time) {
			state = {
				state.cur, grad, state.cur,
				0.0f, time, true
			};
		};

		/// Updates background gradient color.
		/// @param delta Elapsed time since last frame.
		void update(float delta) {
			// update current gradient
			if (state.prog) {
				// calculate animated gradient
				state.time += delta;
				float t = 1.0f;
				if (state.time >= state.dura) {
					state.time = state.dura;
					state.prog = false;
				} else t = Easings::sine(state.time / state.dura);
				state.cur.light = lerpv(state.beg.light, state.end.light, t);
				state.cur.dark  = lerpv(state.beg.dark , state.end.dark , t);
			};

			// send color data to background shader
			static GLuint lpos = glGetUniformLocation(Shaders::bgs.ID, "light");
			static GLuint dpos = glGetUniformLocation(Shaders::bgs.ID, "dark");
			Shaders::bgs.use();
			glUniform3fv(lpos, 1, glm::value_ptr(state.cur.light));
			glUniform3fv(dpos, 1, glm::value_ptr(state.cur.dark));
		};

		/// Draws background.
		void draw() {
			Shaders::bgs.use();
			layout->draw();
		};

		/// Draws shadow mask.
		void shadow() {
			Shaders::sdw.use();
			layout->draw();
		};

		/// Unloads background object.
		void unload() {
			delete buffer;
			delete layout;
		};
	};
};