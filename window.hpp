#pragma once

/// Window instance.
sf::Window app;

/// Window related namespace.
namespace Win {
	/// Window size.
	sf::Vector2u size;

	/// Whether the window is fullscreen.
	bool fullscreen = false;

	/// Whether to enable V-Sync.
	bool vsync = false;

	/// Whether the game should be updating.
	bool updates = true;

	/// Sends window size to GUI shader.
	void sendWindowSize() {
		Shaders::gui.use();
		glUniform1ui(glGetUniformLocation(Shaders::gui.ID, "winX"), size.x);
		glUniform1ui(glGetUniformLocation(Shaders::gui.ID, "winY"), size.y);
	};

	/// Creates a new window instance.
	void create() {
		// window settings
		sf::ContextSettings cs;
		cs.antialiasingLevel = 4;
		cs.depthBits = 24;
		cs.stencilBits = 8;

		// instantiate window
		if (app.isOpen()) app.close();
		if (fullscreen)
			app.create(sf::VideoMode::getDesktopMode(), "Cubes", sf::Style::Fullscreen, cs);
		else
			app.create(sf::VideoMode(1600, 900), "Cubes", sf::Style::Close | sf::Style::Titlebar, cs);
		size = app.getSize();

		// set framerate
		if (vsync)
			app.setVerticalSyncEnabled(true);
		else
			app.setFramerateLimit(60);

		// disable key repeating
		app.setKeyRepeatEnabled(false);
	};

	/// Handles a window event.
	/// @param evt Polled event.
	/// @param screen Screen element.
	void event(const sf::Event& evt) {
		// check close event
		if (evt.type == sf::Event::Closed) {
			app.close();
			return;
		};

		// check resize event
		if (evt.type == sf::Event::Resized) {
			size = sf::Vector2u(evt.size.width, evt.size.height);
			return;
		};
	};

	/// Updates game state.
	/// @param stop Whether to stop game updates.
	/// @return Elapsed time since last frame.
	float update(bool stop) {
		// ignore all updates if not focused
		if (!app.hasFocus()) {
			updates = false;
			Timers::clock.restart();
			return 0.0f;
		};
		updates = true;

		// update timers
		float delta = Timers::update(stop);

		// update model instances
		if (!stop)
			Models::Instances::animate(delta);
		return delta;
	};

	/// Initializes OpenGL.
	void init() {
		// enable depth testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// enable face culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		// enable alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	};

	/// Clears the screen.
	/// @param space Background space object.
	void clear() {
		// update GUI view
		sendWindowSize();

		// clear buffer
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// draw background
		glDepthMask(0);
		Models::Space::draw();
		glDepthMask(1);
	};

	/// Enables shadow buffering mode.
	void buffer() {
		// enable stencil testing
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, ~0);
		glColorMask(0, 0, 0, 0);
		glDepthMask(0);
		glStencilMask(~0);
		glDepthMask(GL_FALSE);

		// setup shadow stencils
		glDisable(GL_CULL_FACE);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
		glStencilOpSeparate(GL_BACK , GL_KEEP, GL_KEEP, GL_DECR_WRAP);
	};

	/// Draws shadow buffer.
	/// @param space Shadow mask object.
	void shadow() {
		glEnable(GL_CULL_FACE);
		glColorMask(1, 1, 1, 1);
		glDepthMask(1);
		glStencilFunc(GL_NOTEQUAL, 0, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		Models::Space::shadow();
	};

	/// Prepare for GUI rendering.
	void gui() {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
	};
};