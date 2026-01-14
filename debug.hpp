#pragma once

/// Debugging stuff.
namespace Debug {
	/// Profiling textbox.
	UI::Text* profile = NULL;
	/// Debugging textbox.
	UI::Element* debug = NULL;

	// FPS measuring
	sf::Clock updateClock;
	sf::Clock frameClock;
	sf::Clock uptimeClock;
	size_t frames = 0;
	float FPS = 0.0f;

	// quad measuring
	size_t vert_normal = 0;
	size_t vert_shadow = 0;

	/// Updates FPS measuring stats.
	void updateFPS() {
		if (updateClock.getElapsedTime().asMilliseconds() >= 250) {
			FPS = frames / updateClock.restart().asSeconds();
			frames = 0;
		};
	};

	/// Update frame counter.
	void updateFrame() {
		frameClock.restart();
		frames++;
	};

	/// Processes an event.
	/// @param evt Event object.
	void event(const sf::Event& evt) {
		// check for profile text toggle
		if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::F10) {
			if (profile) profile->active = !profile->active;
		}

		// check for debug text toggle
		else if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::F12 && evt.key.control) {
			if (debug) debug->active = !debug->active;
		};
	};

	/// Generates profiling textbox.
	/// @param screen Target UI screen.
	void genProfile(UI::Screen& screen) {
		// create text element
		profile = new UI::Text;
		profile->active = false;
		profile->box.pos = { 8px, 1ps - 1ws };
		profile->font = 2;
		profile->updateCall([](UI::Element& elem, float delta) {
			// get rendering time
			float ms = frameClock.getElapsedTime().asSeconds() * 1000;

			// get uptime in hh:mm:ss
			uint64_t secs = (uint64_t)uptimeClock.getElapsedTime().asSeconds();

			uint64_t s = secs % 60; secs /= 60;
			uint64_t m = secs % 60; secs /= 60;
			uint64_t h = secs;

			// update profiling textbox
			profile->setFmt(
				"\2Uptime\4:\r \6%02llu\7 : \6%02llu\7 : \6%02llu\7\n"
				"\6%.1f\7 FPS \4|\r \6%.1f\7 ms\n"
				"\6%llu\7Q \4|\r \6%llu\7S \4|\r \6%llu\7F",
				h, m, s,
				FPS, ms,
				vert_normal / 6, vert_shadow / 6, UI::sprite_count
			);
		});
		screen.add(profile);
	};

	/// Generates debugging textbox.
	/// @param screen Target UI screen.
	/// @param level Level pointer.
	void genDebug(UI::Screen& screen, Level::Level* level) {
		// create text element
		UI::Text* text = new UI::Text;
		text->box.pos = { 8px, 0px };
		text->font = 2;
		text->updateCall([=](UI::Element& elem, float delta) {
			if (level == NULL) return;

			// get player cube
			Level::Cube& cube = level->state.cubes[level->state.player];

			// fall immunity value
			char* fallimmune = (cube.bump >= 1)
				? formatf("\6`\7")
				: formatf("%u", cube.fallimmune);

			// camera lock value
			char* lockvalue = NULL;
			if (level->state.lock == -1ULL)
				lockvalue = formatf("\6None\7");
			else {
				const Bytes::CameraLock& lock = level->root.locks[level->state.lock];
				lockvalue = formatf("%d, %d, %d", (int)lock.eye.x, (int)lock.eye.y, (int)lock.eye.z);
			};

			// draw cube info
			text->setFmt(
				"\2Player \6%llu\7:\n"
				"\n"
				"# Volume = %.2f %.2f %.2f (%.2f x %.2f x %.2f)\n"
				"# Origin = %.2f, %.2f, %.2f\n"
				"# Align Immunity = %u\n"
				"# Position = %.2f, %.2f, %.2f\n"
				"# Mini = %d, %d, %d\n"
				"\n"
				"# Direction = \6%s\7\n"
				"# Action = %s\n"
				"# Interpolation = %d / 64\n"
				"# Derivative = %d / 64\n"
				"\n"
				"# Speed = %+.2f, %+.2f, %+.2f\n"
				"# Fall Immunity = %s, Bump = %u\n"
				"# Energy = %.1f%%\n"
				"\n"
				"# Checkpoint = %d, %d, %d (State = %s)\n"
				"# Game Time = %d>%02d'%03d\n"
				"# Edge Time = %d>%02d'%03d\n"
				"\n"
				"# Camera Offset = %.2f\n"
				"# Camera FOV = %.1f\n"
				"# Camera Lock = %s\n",

				level->state.player,

				(float)cube.volume.pos.x, (float)cube.volume.pos.y, (float)cube.volume.pos.z,
				(float)cube.volume.dim.x, (float)cube.volume.dim.y, (float)cube.volume.dim.z,

				(float)cube.offset.x, (float)cube.offset.y, (float)cube.offset.z,
				cube.alignimmune,
				(float)cube.tile.x, (float)cube.tile.y, (float)cube.tile.z,
				cube.mini.x, cube.mini.y, cube.mini.z,

				directionName(cube.dir),
				Level::Cube::actionName(cube.act),
				(int)(cube.t * 64fx),
				(int)(cube.dt * 64fx),

				(float)cube.speed.x, (float)cube.speed.y, (float)cube.speed.z,
				fallimmune, cube.bump,
				(float)cube.energy * 100,

				level->checkpoint.x, level->checkpoint.y, level->checkpoint.z,
				level->lastsize ? "Mini" : "Normal",
				Data::Time::min(level->timer.game), Data::Time::sec(level->timer.game), Data::Time::ms(level->timer.game),
				Data::Time::min(level->timer.edge), Data::Time::sec(level->timer.edge), Data::Time::ms(level->timer.edge),

				(float)level->camera.off.x,
				level->camera.fov,
				lockvalue
			);

			// free strings
			delete fallimmune;
			delete lockvalue;
		});

		// create cube highlight
		UI::Panel* highlight = new UI::Panel;
		highlight->border = glm::vec3(1.0f, 0.0f, 0.0f);
		highlight->alpha = 0.3f;
		highlight->thickness = 4;
		highlight->updateCall([=](UI::Element& elem, float delta) {
			// get player cube
			Level::Cube& cube = level->state.cubes[level->state.player];

			// get player bounds
			sf::IntRect rect = level->camera.vertBounds(cube.volume);

			// remap highlight to bounds
			highlight->box = UI::DimBox::fromRect(rect);
		});

		// create debug stuff container
		debug = new UI::Element;
		debug->box = { 0px, 0px, 1ps, 1ps };
		debug->add(text);
		debug->add(highlight);
		debug->active = false;
		screen.add(debug);
	};
};