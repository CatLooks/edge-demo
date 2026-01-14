#pragma once

/// Level related namespace.
namespace Level {
	/// Moving platform object.
	struct Moving {
		/// Moving platform data.
		const Bytes::Moving* root;

		vec3x   now; // Current position.
		vec3x  prev; // Previous position.
		vec3x start; // Waypoint start.

		uint16_t waypoint = 0xffff; // Waypoint pointer.
		float        time = 0;      // Current waypoint time.

		/// Platform object.
		Object* object = NULL;
		/// Previous active value.
		bool pactive;
		/// Whether the platform is active.
		bool active;

		/// Moving platform destructor.
		~Moving () { if (object) delete object; };

		/// Structure constructor.
		/// @param data Moving platform data.
		Moving (const Bytes::Moving* data): root(data) {
			start = data->start;
			waypoint = data->active ? 0 : 0xffff;
			pactive = false;
			active = false;
		};

		/// Copy constructor.
		/// @param plat Copied moving platform object.
		/// @note Copy will ignore model pointer.
		Moving (const Moving& plat):
			root(plat.root),
			now(plat.now),
			prev(plat.prev),
			start(plat.start),
			waypoint(plat.waypoint),
			time(plat.time),
			active(plat.active) {};

		/// Copy assignment.
		/// @param plat Copied moving platform object.
		/// @note Copy will ignore model pointer.
		Moving& operator=(const Moving& plat) {
			root = plat.root;
			now = plat.now;
			prev = plat.prev;
			start = plat.start;
			waypoint = plat.waypoint;
			time = plat.time;
			active = plat.active;
			return *this;
		};

		/// Generates a moving platform model.
		void model() {
			delete object; object = new Object;
			Models::Moving::platform(*object, root->theme, root->half);
		};

		/// Starts to move the platform from a waypoint.
		/// @param way Waypoint ID.
		void move(uint16_t way) {
			if (time != 0.0f) {
				start = now;
				time = 0.0f;
			};
			waypoint = way;
		};

		/// Sets waypoint and executes all commands until halted.
		/// @param way Waypoint ID.
		/// @note If an infinite loop is detected, execution will halt at the specified waypoint.
		void proc(uint16_t way) {
			// start moving
			move(way);

			// proc updates until halt
			while (true) {
				update(0.0f, true);
				if (waypoint == 0xffff || waypoint == way)
					break;
			};
		};

		/// Checks whether the platform got paused.
		bool paused() const { return pactive && !active; };

		/// Update moving platform state.
		/// @param delta Elapsed time since last frame.
		/// @param proc Whether to immediately process the waypoint.
		void update(float delta, bool proc = false) {
			prev = now;
			pactive = active;

			// update waypoint
			if (waypoint != 0xffff) {
				// update waypoint timer
				time += delta;

				// check for new waypoint
				if (proc || time >= convTicks(root->ways[waypoint].total)) {
					time = 0.0f;

					// update start position
					start = root->ways[waypoint].dest;

					// select new waypoint
					switch (root->ways[waypoint].after) {
						case Bytes::Waypoint::Halt: waypoint = 0xffff; break;
						case Bytes::Waypoint::Next: waypoint++;        break;
						case Bytes::Waypoint::Loop: waypoint = 0;      break;
					};
				};

				// check if new waypoint exists
				if (waypoint != 0xffff) {
					// update activity
					active = time < convTicks(root->ways[waypoint].time);

					// update position
					if (active) {
						fix t = (fix)(time / convTicks(root->ways[waypoint].time));
						now = vec3x::lerp(start, root->ways[waypoint].dest, t);
					} else
						now = root->ways[waypoint].dest;
				} else {
					// no new active waypoint
					active = false;
					now = start;
				};
			} else {
				// no active waypoint
				active = false;
				now = start;
			};

			// clamp position
			now = vec3x(
				(now.x * 64fx).round() / 64fx,
				(now.y * 64fx).round() / 64fx,
				(now.z * 64fx).round() / 64fx
			);

			// update model
			if (!proc) Models::Moving::update(*object, root->theme, root->half, (float)now.z);
		};

		/// Draws the moving platform.
		/// @return Drawn vertex count.
		size_t draw() {
			size_t vertcount = 0;
			if (object) {
				// draw platform
				Assets::terrain.bind();
				object->pos = now.gl();
				vertcount += object->draw(Shaders::tex);

				// draw highlight
				Assets::mask.bind();
				if (active)
					vertcount += Models::drawActive(object->pos, root->half);
				else
					vertcount += Models::drawInactive(object->pos, root->half);
			};
			return vertcount;
		};

		/// Returns a moving platform volume.
		vol3x volume() const { return Bytes::Block(root->half ? Bytes::Block::Half : Bytes::Block::Full).volume() + now; };

		/// Returns moving platform velocity.
		vec3x velocity() const { return now - prev; };
	};
};