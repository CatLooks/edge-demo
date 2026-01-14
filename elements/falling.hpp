#pragma once

/// Level related namespace.
namespace Level {
	// import model theme
	using Models::Theme;

	/// Falling platform object.
	struct Falling {
		/// Falling platform state type.
		enum State {
			Solid, // Platform floats.
			Decay, // Platform prepares to fall.
			Fall , // Platform falls.
		};

		/// Falling platform data.
		const Bytes::Falling* root;

		vec3x   pos; // Platform position.
		fix  speedz; // Platform vertical speed.
		float  left; // Unstable time left.
		State state; // Whether the platform is falling.

		/// Structure constructor.
		/// @param data Falling platform data.
		Falling (const Bytes::Falling* data): root(data) {
			pos = root->pos;
			state = Solid;
		};

		/// Checks if falling platform counts as ground.
		bool active() const { return state != Fall; };

		/// Checks if falling platform should be despawned.
		bool despawn() const { return pos.z <= -16fx; };

		/// Triggers falling platform fall animation.
		void trigger() { if (state != Solid) return; left = convTicks(root->time); state = Decay; Assets::sfx(Assets::Decay); };

		/// Updates falling platform state.
		/// @param delta Elapsed time since last frame.
		void update(float delta) {
			switch (state) {
				// update platform decay time
				case Decay: {
					if (left < delta) {
						speedz = 0fx;
						state = Fall;
						Assets::sfx(Assets::Drop);
						break;
					};
					left -= delta;
				}; break;
				// accelerate platform downwards
				case Fall: {
					if (pos.z <= -16fx) break;
					speedz = (speedz + 0.125fx).clamp(0fx, 0.22fx);
					pos.z -= speedz;
				}; break;
				// no action otherwise
				default: break;
			};
		};

		/// Draws the falling platform.
		/// @return Drawn vertex count.
		size_t draw() {
			if (!despawn()) {
				Assets::plat.bind();
				return Models::drawFalling(pos.gl(), root->theme);
			};
			return 0;
		};

		/// Generates falling platform volume.
		vol3x volume() const {
			if (state != Solid)
				return vol3x(vec3x(0), vec3x(0));
			return vol3x(vec3x(0, 0, 1.0f - Models::Falling::height), vec3x(1, 1, Models::Falling::height)) + pos;
		};
	};
};