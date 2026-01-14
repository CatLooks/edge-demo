#pragma once

/// Level related namespace.
namespace Level {
	// import model theme
	using Models::Theme;

	/// Bumper object.
	struct Bumper {
		/// Bumper data.
		const Bytes::Bumper* root;

		/// Whether the bumper is active.
		bool active;
		/// Current animation time left.
		float left;
		/// Whether the bumper exerts impulse.
		bool bumps;
		/// Whether the bumper should be disabled after next bump.
		bool disable;

		/// Bumper object.
		Object* object = NULL;

		/// Destructor.
		~Bumper () { if (object) delete object; };

		/// Structure constructor.
		/// @param data Bumper data.
		Bumper (const Bytes::Bumper* data): root(data) {
			active = data->active;
			left = convTicks(data->init);
			object = NULL;
			bumps = false;
			disable = false;
		};

		/// Copy constructor.
		/// @param bumper Copied bumper object.
		/// @note Copy will ignore model pointer.
		Bumper (const Bumper& bumper):
			root(bumper.root),
			active(bumper.active),
			left(bumper.left),
			bumps(bumper.bumps),
			disable(bumper.disable) {};

		/// Copy assignment.
		/// @param bumper Copied bumper object.
		/// @note Copy will ignore model pointer.
		Bumper& operator=(const Bumper& bumper) {
			root = bumper.root;
			active = bumper.active;
			left = bumper.left;
			bumps = bumper.bumps;
			disable = bumper.disable;
			return *this;
		};

		/// Generates a bumper model.
		void model() {
			delete object; object = new Object;
			object->pos = root->pos.gl();
			Models::Bumper::generate(*object, root->dir, root->theme, Textures::Terrain::shadeZ((int)root->pos.z));
		};

		/// Triggers a bumper.
		/// @param act Bumper activation type.
		void trigger(Bytes::Bumper::Activation act) {
			switch (act) {
				// disable the bumper
				case Bytes::Bumper::Disable:
					active = false;
					return;
				// enable only once
				case Bytes::Bumper::Once:
					active = true;
					disable = true;
					left = 0.0f;
					break;
				// enable the bumper
				case Bytes::Bumper::Enable:
					active = true;
					disable = false;
					left = 0.0f;
					return;
			};
		};

		/// Update bumper state.
		/// @param delta Elapsed time since last frame.
		void update(float delta) {
			// update animation time
			if (left > 0.0f) {
				if (left < delta) left = 0.0f;
				else left -= delta;
				if (disable) {
					active = false;
					disable = false;
				};
				bumps = false;
			};

			// restart animation
			if (left == 0.0f && active) {
				bumps = true;
				left = convTicks(root->time);
			};

			// update bumper model
			float t = glm::clamp((convTicks(root->time) - left) / convTicks(root->bump), 0.0f, 1.0f);
			Models::Bumper::animate(*object, root->dir, t);
		};

		/// Draws the moving platform.
		/// @return Drawn vertex count.
		size_t draw() const {
			Assets::terrain.bind();
			size_t vert = object->draw(Shaders::tex);
			float t = glm::clamp((convTicks(root->time) - left) / convTicks(root->bump), 0.0f, 1.0f);
			if (t < 0.4f) {
				Assets::mask.bind();
				vert += Models::drawHint((root->pos + vec3x(0, 0, 1)).gl());
			};
			return vert;
		};

		/// Checks if the cube is in range of bumper impulse volume.
		bool in(const vol3x& cube) {
			// checks if bumper is active
			if (!bumps) return false;

			// generate impulse volume
			vol3x volume;
			fix w = 0.125fx;
			switch (root->dir) {
				case East : volume = vol3x(root->pos - vec3x(w, 0, 0), vec3x(w, 1, 1)); break;
				case West : volume = vol3x(root->pos + vec3x(1, 0, 0), vec3x(w, 1, 1)); break;
				case North: volume = vol3x(root->pos - vec3x(0, w, 0), vec3x(1, w, 1)); break;
				case South: volume = vol3x(root->pos + vec3x(0, 1, 0), vec3x(1, w, 1)); break;
			};

			// compare volumes
			return volume.intersects(cube);
		};
	};
};