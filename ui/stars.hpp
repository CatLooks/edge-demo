#pragma once

/// User interface namespace.
namespace UI {
	/// Star object.
	struct Star {
		/// Star fade start time.
		const float fade = 1.5f;

		sf::Vector2i pos; /// Star position.
		float       left; /// Time until despawn.
		float       time; /// Total star time.
		bool         inf; /// Whether star lifetime is infinite.
		uint16_t   lives; /// Amount of star respawns.

		/// Star brightness.
		float brightness;

		/// Dynamic star constructor.
		/// @param pos Star position.
		/// @param time Star lifetime.
		Star (sf::Vector2i pos, float time): pos(pos), left(time), time(time), inf(false), brightness(0.0f) {};

		/// Static star constructor.
		/// @param pos Star position.
		Star (sf::Vector2i pos): pos(pos), left(0.0f), inf(true), brightness(1.0f) {};

		/// Sets amount of respawns of the star.
		/// @param lives Amount of respawns.
		Star& setRespawns(uint16_t lives) {
			this->lives = lives;
			return *this;
		};

		/// Updates star state.
		/// @param delta Time elapsed since last frame.
		void update(float delta) {
			// ignore if infinite
			if (inf) return;

			// update time
			left -= delta;

			// initial glow up
			if (left > fade && brightness < 1.0f) {
				brightness = glm::clamp(brightness + delta, 0.0f, 1.0f);
				return;
			};

			// update brightness
			float t = glm::clamp(left, 0.0f, fade) / fade;
			brightness = sinf(M_PI_2 * t);
		};

		/// Tries to respawn a star.
		bool respawn() {
			if (inf) return true;
			if (lives == 0) return false;
			lives--; left = time; return true;
		};

		/// Checks whether the star is alive.
		bool alive() const { return inf || left > 0.0f; };
	};

	/// Star collection element.
	class StarSystem : public Element {
		protected:
		/// Star positions.
		std::list<Star> stars;
		/// Star sprite.
		Sprite* sprite = NULL;

		public:
		/// Star system constructor.
		StarSystem () {
			// setup star sprite
			sprite = new Sprite;
			sprite->set(Textures::TexMap(glm::vec2(0.0f), glm::vec3(1.0f)));
			sprite->size = {2, 2};

			// add star spawner
			updateCall([&](Element&, float) {
				// ignore if max amount of stars reached
				if (stars.size() >= 50)
					return;

				// pick a random position
				sf::Vector2i pos = { rand() % (int)Win::size.x, rand() % (int)Win::size.y };

				// pick a random life time
				float time = (Level::uniformRange(0.0f, 1.0f) < 0.1f)
					? Level::uniformRange(10.0f, 20.0f)
					: Level::uniformRange(3.0f, 6.0f);

				// pick respawn count
				uint16_t respawn = Level::randu16(0, 5);

				// generate new star
				stars.push_back(Star(pos, time).setRespawns(respawn));
			});

			// add system update
			updateCall([&](Element&, float delta) {
				auto it = stars.begin();
				while (it != stars.end()) {
					// update star
					it->update(delta);

					// remove if not active
					if (it->alive())
						it++;
					else if (!it->respawn())
						it = stars.erase(it);
				};
			});
		};

		/// Destructor.
		~StarSystem () { delete sprite; };

		protected:
		/// Draws the star system.
		/// @param self Bounding box.
		void drawSelf(sf::IntRect box) const override {
			Assets::white.bind();
			for (const Star& star : stars) {
				// draw star sprite
				sprite->pos = star.pos;
				sprite->alpha = star.brightness;
				sprite->draw();
			};
		};
	};
};