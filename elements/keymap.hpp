#pragma once

/// Level related namespace.
namespace Level {
	/// Cube input object.
	struct CubeInput {
		bool east = false;
		bool west = false;
		bool north = false;
		bool south = false;

		/// Executes an event on cube input state.
		/// @param event Event data.
		void update(const Bytes::Event& event) {
			switch (event.dir) {
				case East : east  = event.key; return;
				case West : west  = event.key; return;
				case North: north = event.key; return;
				case South: south = event.key; return;
			};
		};

		/// Returns a cube input based on current keyboard state.
		static CubeInput keyboard() {
			return (CubeInput) {
				sf::Keyboard::isKeyPressed(sf::Keyboard::Left),
				sf::Keyboard::isKeyPressed(sf::Keyboard::Right),
				sf::Keyboard::isKeyPressed(sf::Keyboard::Up),
				sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
			};
		};

		/// Returns a cube input based on current keyboard state.
		/// @note Uses a WASD layout.
		static CubeInput keyboardDebug() {
			return (CubeInput) {
				sf::Keyboard::isKeyPressed(sf::Keyboard::A),
				sf::Keyboard::isKeyPressed(sf::Keyboard::D),
				sf::Keyboard::isKeyPressed(sf::Keyboard::W),
				sf::Keyboard::isKeyPressed(sf::Keyboard::S)
			};
		};
	};

	/// Cube movement instruction processor.
	class KeyMap {
		protected:
		/// Movement instruction list.
		const Bytes::Path* root;
		/// Time left until next event.
		float left;
		/// Current event index.
		size_t idx;
		/// Whether the events are synced.
		bool sync;
		/// Synced moving platform index.
		uint16_t moving;
		/// Synced moving platform waypoint index.
		uint16_t waypoint;
		/// Current input state.
		CubeInput input;

		public:
		/// Key map constructor.
		/// @param data Path data.
		KeyMap (const Bytes::Path* data = NULL):
			root(data), left(0.0f), idx(-1ULL),
			sync(false), moving(0), waypoint(0) {

			// ignore if not complete
			if (data == NULL) return;

			// fetch first event waiting time
			if (!data->evts.empty()) {
				idx = -1ULL;
				advance();
			};
		};

		/// Sets current cube input state.
		void set(CubeInput state) { input = state; };

		/// Get current input state.
		CubeInput state() const { return input; };

		/// Advances event pointer.
		/// @return Whether to exit the drain loop.
		bool advance() {
			// execute new instruction
			if (idx != -1ULL) {
				if (root->evts[idx].sync) {
					// setup syncing
					left = 0.0f;
					sync = true;
					moving = root->evts[idx].moving;
					waypoint = root->evts[idx].waypoint;
				} else
					input.update(root->evts[idx]);
			};

			// check if further instructions are available
			if (++idx >= root->evts.size()) {
				idx = -1ULL;
				return true;
			};
			if (sync) return true;

			// fetch new instruction delay
			left = convTicks(root->evts[idx].time);
			return false;
		};

		/// Checks if a keymap is active.
		bool active() const { return !(root == NULL || idx == -1ULL); };

		/// Updates key map.
		/// @param delta Time elapsed since last frame.
		/// @param plats Moving platform list.
		void update(const std::vector<Moving>& plats, float delta) {
			// check if key map needs updates
			if (!active()) return;

			// check for syncing
			if (sync) {
				// check if moving platform is inactive
				if (plats[moving].waypoint == waypoint && plats[moving].paused()) {
					sync = false;
				} else return;
			};

			// drain delta value
			while (delta > 0.0f) {
				// check if new instruction is needed
				if (delta >= left) {
					// drain delta
					delta -= left;
					if (advance()) return;
					continue;
				};

				// update current time
				left -= delta;
				break;
			};
		};
	};
};