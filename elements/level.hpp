#pragma once

/// Level related namespace.
namespace Level {
	/// Convert ticks into time.
	/// @param ticks Amount of ticks.
	float convTicks(uint16_t ticks) {
		return ticks / 240.0f;
	};

	/// Smoothly clamps a value to [-bound, bound].
	/// @param val Initial value.
	/// @param bound Value range bounds.
	/// @param dv Clamping speed.
	fix speedclamp(fix val, fix bound, fix dv) {
		const fix pos = bound;
		const fix neg = -bound;

		if (val > pos) return (val - dv).max(pos);
		if (val < neg) return (val + dv).min(neg);
		return val;
	};

	/// Returns a value that approaches the destination.
	/// @param val Initial value.
	/// @param dst Destination value.
	/// @param vel Approaching velocity.
	/// @return Next value.
	fix approach(fix val, fix dst, fix vel) {
		if (val < dst) return (val + vel).min(dst);
		if (val > dst) return (val - vel).max(dst);
		return dst;
	};

	/// Checks if position is inside of a field.
	/// @param mid Field middle position.
	/// @param rad Field radius.
	/// @param pos Tested position.
	bool inside(vec3x mid, vec2x rad, vec3x pos) {
		if (mid.z != pos.z) return false;
		return mid.x - rad.x <= pos.x && pos.x <= mid.x + rad.x
			&& mid.y - rad.y <= pos.y && pos.y <= mid.y + rad.y;
	};

	/// Checks if number is aligned.
	/// @param n Target number.
	/// @param mini Alignment unit size.
	bool aligned(fix n, bool mini) {
		if (mini) n *= 3fx;
		return (n - n.round()).abs() <= (1fx / 64fx);
	};

	/// Aligns a vector on XY plane.
	vec3x align(vec3x vec, bool mini) {
		if (mini) return vec3x(
			(vec.x * 3fx).round() / 3fx,
			(vec.y * 3fx).round() / 3fx,
			vec.z
		);
		return vec3x(vec.x.round(), vec.y.round(), vec.z);
	};

	/// Returns index of longest axis component.
	/// @param v Tested vector.
	/// @return `0` - X axis, `1` - Y axis, `2` - Z axis.
	int maxaxis(vec3x v) {
		fix arr[3] { v.x.abs(), v.y.abs(), v.z.abs() };
		if (arr[0] >= arr[1] && arr[0] >= arr[2]) return 0;
		if (arr[1] >= arr[0] && arr[1] >= arr[2]) return 1;
		return 2;
	};

	/// Get intersection length of 2 intervals.
	/// @param fp Start of first interval.
	/// @param fd Length of first interval.
	/// @param sp Start of second interval.
	/// @param sd Length of second interval.
	/// @return Intersection length.
	fix islen(fix fp, fix fd, fix sp, fix sd) {
		fix min = fp.max(sp);
		fix max = (fp + fd).min(sp + sd);
		if (max < min) return 0;
		return max - min;
	};

	/// Get intersection size of 2 volumes.
	/// @param a First volume.
	/// @param b Second volume.
	vec3x isvol(const vol3x& a, const vol3x& b) {
		return {
			islen(a.pos.x, a.dim.x, b.pos.x, b.dim.x),
			islen(a.pos.y, a.dim.y, b.pos.y, b.dim.y),
			islen(a.pos.z, a.dim.z, b.pos.z, b.dim.z)
		};
	};

	/// Returns a random integer.
	/// @param min Minimal value.
	/// @param max Maximal value.
	uint16_t randu16(uint16_t min, uint16_t max) {
		return rand() % (max - min) + min;
	};

	/// Generates a random float in [-1, 1].
	float uniform() {
		return rand() * 2.0f / RAND_MAX - 1.0f;
	};

	/// Generates a random float in [a, b].
	/// @param a Minimal value.
	/// @param b Maximal value.
	float uniformRange(float a, float b) {
		return lerpf(a, b, (float)rand() / RAND_MAX);
	};
};

// include level element units
#include "terrain.hpp"
#include "moving.hpp"
#include "falling.hpp"
#include "bumper.hpp"
#include "resizer.hpp"
#include "prism.hpp"
#include "keymap.hpp"
#include "cube.hpp"
#include "checkpoint.hpp"
#include "button.hpp"
#include "camera.hpp"
#include "darkcube.hpp"
#include "recorder.hpp"

/// Level related namespace.
namespace Level {
	// import particle classes
	using Models::Particle::Accel;
	using Models::Particle::Dust;
	using Models::Particle::System;

	/// Collision info object.
	struct Collision {
		vol3x vol; // Collision volume.
		vec3x vel; // Collision velocity.
		size_t id; // Collision source.
	};

	/// Level state object.
	struct State {
		std::vector<Moving>     movingPlats; // Moving platform list.
		std::vector<Falling>   fallingPlats; // Falling platform list.
		std::vector<Bumper>         bumpers; // Bumper list.
		std::vector<Resizer>       resizers; // Resizer list.
		std::vector<Prism>           prisms; // Prism list.
		std::vector<Checkpoint>      checks; // Checkpoint list.
		std::vector<Button>         buttons; // Button list.
		std::vector<CameraTrigger> triggers; // Camera trigger list.
		std::vector<DarkCube>     darkcubes; // Dark cube spawner list.
		std::deque <Cube>             cubes; // Cube list.

		/// Player cube index.
		size_t player;
		/// Last camera trigger index.
		size_t last;
		/// Camera lock object index.
		size_t lock;
		/// Last prism index.
		size_t prism;
		/// Last checkpoint gradient theme.
		Models::Theme grad;

		/// Particle system object.
		System particles;
	};

	/// Level object.
	struct Level {
		/// Level data
		Bytes::Level root;
		/// Terrain object.
		Terrain terrain;

		/// Current level state.
		State state;
		/// Last checkpoint level state.
		State last;
		/// Initial checkpoint.
		State init;
		/// Last checkpoint camera state.
		CameraTarget camerastate;
		/// Initial camera state.
		CameraTarget initcamera;
		/// Last checkpoint position.
		sf::Vector3i checkpoint;
		/// Last checkpoint cube size.
		bool lastsize;
		/// Whether to reset camera after checkpoint.
		bool camerareset;
		/// Checkpoint camera reset position.
		vec3x camerapos;
		/// Death count.
		size_t deaths;
		/// Animation timer.
		float anim;
		/// Moving platform tick timer.
		float tick;
		/// Current hang time.
		std::map<size_t, float> hang;
		/// Level time.
		Data::Time timer;

		/// Whether the player has made the first contact.
		bool firstcontact;
		/// First contact callback.
		std::function<void()> contactcall = [](){};

		/// Current point of interest index.
		size_t POI;

		/// Camera follow position.
		glm::vec3 followPos;
		/// Whether to lock follow position.
		bool followLock;
		/// Exit animation request.
		bool exitReq;
		/// Level camera.
		Camera camera = Camera(22.0f);

		/// Whether the level is a worldmap.
		bool world;
		/// Mute sound of next prism.
		bool mutePrism;

		/// Structure constructor.
		/// @param data Level data.
		/// @param world Whether the level is a worldmap.
		Level (Bytes::Level data = {}, bool world = false):
			root(data), deaths(0), tick(0), firstcontact(false), POI(0),
			followLock(false), exitReq(false), world(world), mutePrism(false)
		{
			// default camera settings
			camera.pos = root.spawn.gl();
			camera.off = glm::vec3(8.0f);
			camera.targetOff(CameraTrigger::zooms(data.zoom), 0.5f);
			initcamera = camera.store();

			// generate terrain object
			terrain = Terrain(root.terrain);

			// spawn player cube
			Cube cube(root.spawn.int3(), false, false, true);
			state.cubes.push_back(cube);
			state.player = 0;
			state.last = -1ULL;
			state.lock = -1ULL;
			state.prism = -1ULL;
			state.grad = root.bg;

			// instantiate elements
			for (const Bytes::Moving& bytes : root.movings)
				state.movingPlats.push_back(Moving(&bytes));
			for (const Bytes::Falling& bytes : root.fallings)
				state.fallingPlats.push_back(Falling(&bytes));
			for (const Bytes::Bumper& bytes : root.bumpers)
				state.bumpers.push_back(Bumper(&bytes));
			for (const Bytes::Resizer& bytes : root.resizers)
				state.resizers.push_back(Resizer(&bytes));
			for (const Bytes::Prism& bytes : root.prisms)
				state.prisms.push_back(Prism(&bytes));
			for (const Bytes::Checkpoint& bytes : root.checks)
				state.checks.push_back(Checkpoint(&bytes));
			for (const Bytes::Button& bytes : root.buttons)
				state.buttons.push_back(Button(&bytes));
			for (const Bytes::CameraTrigger& bytes : root.triggers)
				state.triggers.push_back(CameraTrigger(&bytes));
			for (const Bytes::DarkCube& bytes : root.darkcubes)
				state.darkcubes.push_back(DarkCube(&bytes));
		};

		/// Creates a level ranking.
		Data::LevelData rank() const {
			// count prisms
			uint16_t pttl = (uint16_t)root.prisms.size();
			uint16_t pcol = pttl - (uint16_t)state.prisms.size();

			// return level data object
			return Data::LevelData::generate(root.ID, root.name, pcol, pttl, deaths).setTime(timer, root.thresholds);
		};

		/// Stores initial level state checkpoint.
		void initCheckpoint() {
			Models::Space::set(Models::Grad::fromTheme(state.grad), 0.0f);
			init = state;
			store(root.spawn.int3(), false);
		};

		/// Stores level state.
		/// @param pos Respawn point.
		/// @param size Cube size.
		/// @note Camera will not be reset during restore.
		/// @warning Must be used after `model()` call.
		void store(sf::Vector3i pos, bool size) {
			last = state;
			checkpoint = pos;
			lastsize = size;
			camerastate = camera.store();
			camerareset = false;
		};

		/// Stores level state.
		/// @param pos Respawn point.
		/// @param size Cube size.
		/// @param camera Camera reset position.
		/// @warning Must be used after `model()` call.
		void store(sf::Vector3i pos, bool size, vec3x camera) {
			last = state;
			checkpoint = pos;
			lastsize = size;
			camerastate = this->camera.store();
			camerareset = true;
			camerapos = camera;
		};

		/// Restores level state from last checkpoint.
		/// @param death Whether the cube has died.
		void restore(bool death) {
			state = last;
			getPlayer() = Cube(checkpoint, lastsize, false, true).setEnergy(getPlayer().energy);
			camera.restore(camerastate);
			if (camerareset) camera.pos = camerapos.gl();
			if (death) deaths++;
			reapply();
		};

		/// Restores level state to initial.
		void retry() {
			state = init;
			getPlayer() = Cube(root.spawn.int3(), false, false, true);
			camera.restore(initcamera);
			camera.pos = root.spawn.gl();
			deaths = 0;
			timer = Data::Time();
			reapply();
		};

		/// Warps the player to a point of interest.
		/// @param offset Point of interest offset.
		/// @return New point of interest index.
		size_t warp(int offset) {
			if (root.darkcubes.empty()) return -1ULL;

			// change point of interest index
			POI = (POI + offset) % root.darkcubes.size();
			if (POI < 0) POI += root.darkcubes.size();

			// teleport player
			const Bytes::DarkCube& dc = root.darkcubes[POI];
			getPlayer() = Cube(dc.spawn.int3(), dc.mini, true, true);
			return POI;
		};

		/// Reapplies current level gradient.
		void reapply() const {
			Models::Space::set(Models::Grad::fromTheme(state.grad), 0.0f);
		};

		/// Generates models for elements.
		void model() {
			// generate terrain model
			terrain.model(&root);

			// generate element models
			for (Moving& moving : state.movingPlats) moving.model();
			for (Bumper& bumper : state.bumpers)     bumper.model();
		};

		/// Updates all level elements.
		/// @param delta Elapsed time since last frame.
		/// @param debug Whether to overwrite world map keymaps.
		void update(float delta, bool debug = false) {
			// check for no updates
			if (!Win::updates) return;

			// update elements
			for (Moving& moving : state.movingPlats)    moving.update(delta);
			for (Bumper& bumper : state.bumpers)        bumper.update(delta);
			for (Falling& falling : state.fallingPlats) falling.update(delta);

			// force cube exit animation
			if (followLock) anim = 65536.0f;

			// update cubes
			std::deque<size_t> dead;
			for (size_t i = 0; i < state.cubes.size(); i++) {
				Cube& cube = state.cubes[i];

				// update cube keymap
				cube.keys.update(state.movingPlats, 1.0f / 60);

				// update cube position
				cube.syncOffset(state.movingPlats, i == state.player ? &tick : NULL);
				cube.update(delta, false);

				// displace by moving platforms
				move(i); if (i == state.player) collect(i);

				// apply steering
				if (debug) cube.keys.set(CubeInput::keyboardDebug());
				control(i, cube.getInput(i == state.player && !world), delta);

				// final update
				cube.update(delta, true);
				if (i == state.player) {
					// update first contact
					if (!firstcontact && cube.act != Cube::Fall) {
						// start playing level music
						contactcall();
						firstcontact = true;
					};
					continue;
				};

				// delete if cube is dead
				if (cube.pos().z <= -20fx || cube.inactive()) {
					dead.push_back(i);
					continue;
				};
			};

			// clean up dead cubes
			while (!dead.empty()) {
				state.cubes.erase(state.cubes.begin() + dead.front());
				dead.pop_front();
			};

			// update buttons
			for (Button& button : state.buttons) button.update();

			// update particles
			state.particles.update(delta);

			// update camera
			camera.update(delta);

			// update level background
			Models::Space::update(delta);

			// update level time
			if (!getPlayer().inactive())
				timer.updateGame(delta);
		};

		/// Updates level camera.
		/// @param offset Additional camera offset.
		void follow(glm::vec3 offset) {
			Cube& cube = getPlayer();
			if (cube.mod_cube == NULL)
				cube.update(0.0f, true);

			// update follow position
			if (!followLock) {
				if (state.lock != -1ULL)
					followPos = root.locks[state.lock].eye.gl();
				else if (cube.pos().z >= -1fx && !cube.inactive())
					followPos = Models::Cube::mass(cube.mod_cube);
			};

			// calculate follow speed
			float speed = (state.lock == -1ULL) ? (1.0f / 8) : (1.0f / 16);

			// update camera position
			glm::vec3 diff = followPos + offset - camera.pos;
			camera.pos += diff * speed;

			// update camera matrix
			camera.recalculate();
			camera.uniform(Shaders::tex, "camera");
		};

		/// Returns player cube.
		Cube& getPlayer() { return state.cubes[state.player]; };

		/// Renders the level.
		/// @return Drawn vertex count.
		size_t draw() {
			// draw terrain
			Assets::terrain.bind();
			size_t vertcount = terrain.draw();

			// draw exit
			Assets::mask.bind();
			vertcount += Models::drawExit(root.exit.gl());

			// draw elements
			for (Moving& moving : state.movingPlats)    vertcount += moving.draw();
			for (Falling& falling : state.fallingPlats) vertcount += falling.draw();
			for (Bumper& bumper : state.bumpers)        vertcount += bumper.draw();
			for (Resizer& resizer : state.resizers)     vertcount += resizer.draw();
			for (Prism& prism : state.prisms)           vertcount += prism.draw();
			for (Button& button : state.buttons)        vertcount += button.draw(state.movingPlats);
			for (Cube& cube : state.cubes)              vertcount += cube.draw();

			// draw particles
			return vertcount + state.particles.draw();
		};

		/// Renders level shadows.
		/// @return Drawn vertex count.
		size_t shadow() {
			size_t vertcount = 0;

			// draw shadows
			for (Prism& prism : state.prisms) vertcount += prism.shadow();
			for (Cube& cube : state.cubes)    vertcount += cube.shadow();
			return vertcount;
		};

		/// Checks if a platform exists at the mini-position.
		/// @param pos Platform position.
		/// @param type Whether to check for only full blocks.
		/// @param mini Platform check type.
		bool platmini(sf::Vector3i pos, bool type, bool mini) const {
			if (pos.x < 0 || pos.y < 0 || pos.z < 0) return false;
			Block::Type block = terrain.root.at(pos / 3).type();
			if (block == Block::Half && mini) switch (pos.z % 3) {
				case 0: block = Block::None; break;
				case 1: block = Block::Half; break;
				case 2: block = Block::Full; break;
				default: break;
			};
			return type ? block == Block::Full : block != Block::None;
		};

		/// Checks if a solid exists at the mini-position.
		/// @param pos Check position.
		/// @param type Whether to check for only full blocks.
		/// @param mini Check region size.
		/// @param source Source of collision.
		bool checkpos(vec3x pos, bool type, bool mini, size_t& source) const {
			// cube size
			fix size = (mini ? 1fx : 3fx) / 3fx;

			// generate cube volume
			vol3x cube = vol3x(vec3x(size)) + pos / 3fx;

			// check for moving platforms
			for (size_t i = 0; i < state.movingPlats.size(); i++) {
				if (state.movingPlats[i].volume().intersects(cube)) {
					vec3x diff = state.movingPlats[i].now - pos / 3fx;
					if (!aligned(diff.x, mini) || !aligned(diff.y, mini))
						source = -1ULL;
					else source = i;
					return true;
				};
			};

			// check for platforms
			if (platmini(pos.int3(), type, mini)) {
				source = -1ULL;
				return true;
			};

			// check for falling platforms
			for (const Falling& falling : state.fallingPlats) {
				if (!falling.active()) continue;
				if (falling.pos.int3() == pos.int3() / 3) {
					Block::Type block = Block::Half;
					if (mini) switch ((int)pos.z % 3) {
						case 0: block = Block::None; break;
						case 1: block = Block::Half; break;
						case 2: block = Block::Full; break;
						default: break;
					};
					if (type ? block == Block::Full : block != Block::None) {
						source = -1ULL;
						return true;
					};
				};
			};
			return false;
		};

		/// Checks if the cube can move into new position.
		/// @param pos Checked position.
		/// @param dir Cube move direction.
		/// @param mini Align unit size.
		bool validmove(vec3x pos, Direction dir, bool mini) const {
			// generate cube volume
			fix size = (mini ? 1fx : 3fx) / 3fx;
			vol3x cube = vol3x(vec3x(size)) + pos / 3fx;

			// check for platforms
			for (const Moving& moving : state.movingPlats) {
				if (moving.volume().intersects(cube)) {
					// get distance between cube and platform
					fix dist = 0fx;
					switch (dir) {
						case East : dist = moving.volume().pos.x - dist;   break;
						case West : dist = dist - moving.volume().out().x; break;
						case North: dist = moving.volume().pos.y - dist;   break;
						case South: dist = dist - moving.volume().out().y; break;
					};

					// check if distance is unaligned
					if (!aligned(dist, mini))
						return false;
				};
			};
			return true;
		};

		/// Returns list of colliders that may collide with player.
		/// @param i Player index.
		std::vector<Collision> getcolliders(size_t i) const {
			std::vector<Collision> list;

			// get cube volume
			vol3x region = state.cubes[i].volume;

			// get base velocity
			vec3x base = state.cubes[i].rel == -1ULL ? vec3x(0) : state.movingPlats[state.cubes[i].rel].velocity();

			// get moving platform colliders
			for (size_t i = 0; i < state.movingPlats.size(); i++) {
				const Moving& moving = state.movingPlats[i];
				if (moving.volume().intersects(region))
					list.push_back(Collision { moving.volume(), moving.velocity() - base, i });
			};

			// get terrain block ranges
			sf::Vector3i min = region.pos.int3();
			sf::Vector3i max = region.out().up3();

			// get terrain colliders
			for (int z = min.z; z <= max.z; z++) {
				for (int y = min.y; y <= max.y; y++) {
					for (int x = min.x; x <= max.x; x++) {
						// get block
						Bytes::Block block = terrain.root.at({x, y, z});
						if (block.type() == Bytes::Block::None)
							continue;

						// add block collider
						vol3x volume = block.volume() + vec3x(x, y, z);
						if (volume.intersects(region))
							list.push_back(Collision { volume, vec3x(0) - base, -1ULL });
					};
				};
			};

			// return collider list
			return list;
		};

		/// Moves cube.
		/// @param grid Cube grid position.
		/// @param sub Cube mini offset.
		/// @param add Move offset in mini coordinates.
		static void addsplit(vec3x& grid, sf::Vector3i& sub, sf::Vector3i add) {
			sub += add;

			// normalize X component
			while (sub.x < 0)  { sub.x += 3; grid.x--; };
			while (sub.x >= 3) { sub.x -= 3; grid.x++; };

			// normalize Y component
			while (sub.y < 0)  { sub.y += 3; grid.y--; };
			while (sub.y >= 3) { sub.y -= 3; grid.y++; };

			// normalize Z component
			while (sub.z < 0)  { sub.z += 3; grid.z--; };
			while (sub.z >= 3) { sub.z -= 3; grid.z++; };
		};

		/// Checks if cube is unaligned.
		/// If so, forces the Align action.
		/// @param cube Target cube.
		/// @return Returns `true` if cube needs to be realigned.
		bool checkAlign(Cube& cube) {
			if (cube.alignimmune > 0)
				return false;
			if (cube.tile.z != cube.tile.z.round()) {
				cube.act = Cube::Fall;
				return true;
			};
			if (cube.tile != align(cube.tile, cube.settings.mini)) {
				cube.act = Cube::Align;
				return true;
			};
			return false;
		};

		/// Moves cube out of collision.
		/// @param id Cube ID.
		void move(size_t id) {
			Cube& cube = state.cubes[id];

			// get colliders
			std::vector<Collision> cols = getcolliders(id);
			if (cols.empty()) return;

			// apply each collider
			for (const Collision& col : cols) {
				// check if collider is stationary
				if (col.vel == vec3x(0)) {
					cube.act = Cube::Crush;
					cube.attach(state.movingPlats, -1ULL);
					animCrush(cube);
					if (id == state.player) anim = 2.0f;
					return;
				};

				// get collision correction offset
				int axis = maxaxis(col.vel);
				vec3x is = isvol(col.vol, cube.volume);
				vec3x off;
				switch (axis) {
					case 0: off = vec3x(is.x, 0, 0) * (col.vel.x < 0fx ? -1fx : 1fx); break;
					case 1: off = vec3x(0, is.y, 0) * (col.vel.y < 0fx ? -1fx : 1fx); break;
					case 2: off = vec3x(0, 0, is.z) * (col.vel.z < 0fx ? -1fx : 1fx); break;
					default: off = vec3x(0); break;
				};

				// apply offset
				cube.tile += off;
				cube.alignimmune = true;
				cube.attach(state.movingPlats, col.id);
			};
		};

		/// Controls the cube.
		/// @param id Cube ID.
		/// @param input Cube input object.
		/// @param delta Elapsed time since last frame.
		void control(size_t id, CubeInput input, float delta) {
			Cube& cube = state.cubes[id];

			// frame length factor constant
			const fix T = delta == 0.0f ? 0fx : 1fx;

			// respawn player
			if (id == state.player && cube.pos().z < -20fx) {
				// restore last checkpoint state
				restore(true);
				return;
			};

			// tick align immunity counter
			if (cube.alignimmune) cube.alignimmune = false;

			// reduce cube energy
			if (world)
				cube.energy = 0fx;
			else
				cube.energy = approach(cube.energy, 0fx, cube.moving() ? (T / 1500fx) : (T / 2000fx));

			// update cube rainbow timer
			if (id == state.player) {
				if (world)
					Timers::cube.set(7500);
				else
					Timers::cube.set(10000 - int(cube.energy * 9500fx));
			};

			// up vector
			const int unit = cube.settings.mini ? 1 : 3;
			const vec3x up = {0, 0, unit};

			// mini multipliers
			const fix speedscale = cube.settings.mini ? 2.0fx : 1fx;
			const fix steerscale = cube.settings.mini ? 3.0fx : 1fx;

			// player energy multipliers
			const fix energy = cube.energy;
			const fix rollerg = 0.9fx + energy * 0.6fx * (cube.settings.mini ? 0.75fx : 1fx);
			const fix hangerg = 1.0fx + energy * 0.3fx * (cube.settings.mini ? 0.75fx : 1fx);

			// player speed limits
			const fix rollspeed = T * 0.07fx * speedscale * rollerg;
			const fix hangspeed = T * 0.09fx * speedscale * hangerg;
			const fix fallspeed = T * 0.22fx;

			// player steering scalars
			const fix rollsteer = T * 0.010fx * steerscale * rollerg;
			const fix hangsteer = T * 0.007fx * steerscale * hangerg;
			const fix fallsteer = T * 0.0125fx;
			const fix clamp = T * 0.02fx;

			// animation gravity constant
			const fix rollg = (T / 56fx) * (cube.settings.mini ? 2.0fx : 1fx);
			const fix hangg = (T / 64fx) * (cube.settings.mini ? 1.0fx : 1fx);

			// reset cube hang time
			if (!(cube.act == Cube::Idle || cube.act == Cube::Fall || cube.moving()))
				cube.hang = false;

			// ignore inputs if cube is not steerable
			bool ignorefall = false;
			bool ignorehang = false;
			if (!cube.steerable()) {
				// check for fall immunity
				if (cube.act == Cube::Fall && cube.fallimmune > 0) {
					if (id == state.player) Assets::sfx(Assets::Fallstop);
					cube.act = Cube::Idle;
					cube.fallimmune--;
					ignorefall = true;
					ignorehang = cube.hang;
				} else switch (cube.act) {
					case Cube::Fall: {
						// disable hangtime
						cube.hang = false;

						// normalize coordinates
						if (cube.mini.z != 0) {
							cube.tile.z += (fix)cube.mini.z / 3fx;
							cube.mini.z = 0;
						};

						// move cube down
						cube.speed.z = (cube.speed.z + fallsteer).clamp(0.0f, fallspeed);
						cube.tile.z -= cube.speed.z;

						// check for ground hit
						size_t src;
						if (checkpos(cube.minipos(), false, cube.settings.mini, src)) {
							// check if cube can still fall
							cube.volume.pos = cube.pos();
							if (src != -1ULL && getcolliders(id).empty()) {
								// attach cube
								cube.attach(state.movingPlats, src);
								return;
							};

							// transition into idle state
							cube.tile.z = cube.tile.z.ceil();
							cube.act = Cube::Idle;

							// check if cube is misaligned
							if (src == -1ULL) checkAlign(cube);
							else cube.tile = align(cube.tile, cube.settings.mini);
							cube.attach(state.movingPlats, src);
							cube.speed.z = 0fx;

							if (id == state.player) Assets::sfx(Assets::Fallstop);
							Assets::sfx(cube.settings.mini ? Assets::Minicube : Assets::Cube);
							return;
						};
					}; break;
					case Cube::Shrink: {
						// update animation
						cube.t += 0.024fx * T;

						// check for animation end
						if (cube.t >= 1fx) cube.shrink();
						return;
					}; break;
					case Cube::Grow: {
						// update animation
						cube.t += 0.024fx * T;

						// move cube to center
						cube.tile.x = approach(cube.tile.x, cube.tile.x.round(), 0.01fx * T);
						cube.tile.y = approach(cube.tile.y, cube.tile.y.round(), 0.01fx * T);

						// check for animation end
						if (cube.t >= 1fx) cube.grow(false);
						return;
					}; break;
					case Cube::Align: {
						// move cube to aligned coordinates
						cube.tile.x = approach(cube.tile.x, cube.tile.x.round(), 0.03fx * T);
						cube.tile.y = approach(cube.tile.y, cube.tile.y.round(), 0.03fx * T);

						// check if cube is aligned
						if (cube.tile.x.quo() == 0fx && cube.tile.y.quo() == 0fx)
							cube.act = Cube::Idle;
						else
							return;
					}; break;
					case Cube::Crush: {
						// ignore if not player
						if (id != state.player) return;

						// check if timer ran out
						if (delta > anim) {
							restore(true);
							break;
						};
						anim -= delta;
					}; return;
					case Cube::Exit: {
						// ignore if not player
						if (id != state.player) return;

						// check if timer ran out
						if (delta > anim) {
							retry();
							break;
						};
						anim -= delta;
					}; return;
					default: break;
				};
			};

			// get x axis impulse
			fix dx = 0fx;
			if (input.east) dx--;
			if (input.west) dx++;

			// get y axis impulse
			fix dy = 0fx;
			if (input.north) dy--;
			if (input.south) dy++;

			// add bump impulse
			if (cube.bump && !(cube.speed.x == 0fx && cube.speed.y == 0fx)) {
				glm::vec2 impulse = directionVec(directionFromSteer(cube.speed.x, cube.speed.y));
				dx += (fix)(impulse.x * ((cube.bump >= 3) ? 2 : 1));
				dy += (fix)(impulse.y * ((cube.bump >= 3) ? 2 : 1));
			};

			// check for active bumpers
			for (Bumper& bumper : state.bumpers) {
				if (bumper.in(cube.volume) && cube.grid() == bumper.root->pos + vec3x::fromDir(bumper.root->dir)) {
					// create bump time
					cube.bump = bumper.root->power;

					// calculate bump impulse
					fix speed = 0fx;
					switch (cube.bump) {
						case 3: speed = 0.47fx; break;
						case 4: speed = 0.52fx; break;
						default: break;
					};

					// apply speed
					glm::vec2 dir = directionVec(bumper.root->dir);
					cube.speed.x = (fix)dir.x * speed;
					cube.speed.y = (fix)dir.y * speed;

					// set cube state
					cube.dir = bumper.root->dir;
					cube.act = Cube::Roll;
					Assets::sfx(Assets::Bump);
				};
			};

			// check for idle action
			if (cube.act == Cube::Idle) {
				// reset hang time
				cube.hang = ignorehang;

				// check for alignment
				if (checkAlign(cube))
					return;

				// check for exit
				if (vol3x(root.exit, vec3x(1)).intersects(cube.volume)) {
					camera.targetFOV(22.0f, 0.8f);
					animExit(cube);
					cube.act = Cube::Exit;
					if (id == state.player) {
						anim = 5.0f;
						exitReq = true;
					};
					return;
				};

				// check for camera triggers
				if (id == state.player && !cube.inactive()) {
					for (size_t i = 0; i < state.triggers.size(); i++) {
						// ignore if active
						if (i == state.last) continue;
						const CameraTrigger& trigger = state.triggers[i];

						// check for collision
						if (inside(trigger.root->pos, trigger.root->rad, cube.grid())) {
							state.last = i;
							if (trigger.root->zoom)
								camera.targetOff(trigger.zoom(), trigger.time());
							else
								camera.targetFOV(trigger.root->fov, trigger.time());
						};
					};
				};

				// check for dark cube spawners
				for (DarkCube& spawner : state.darkcubes) {
					if (spawner.active && inside(spawner.root->pos, spawner.root->rad, cube.grid())) {
						// disable spawner
						spawner.active = false;

						// create dark cube
						Cube darkcube(spawner.root->spawn.int3(), spawner.root->mini, spawner.root->dark, false);
						darkcube.keys = KeyMap(&spawner.root->path);
						state.cubes.push_back(darkcube);
						continue;
					};
				};

				// check for checkpoints
				if (id == state.player) {
					for (Checkpoint& check : state.checks) {
						if (check.active && inside(check.root->pos, check.root->rad, cube.grid())) {
							// setup background gradient
							if (check.root->time != 0) {
								Models::Space::set(
									Models::Grad::fromTheme(check.root->theme),
									convTicks(check.root->time)
								);
								state.grad = check.root->theme;
							};

							// store level state
							check.active = false;
							if (check.root->reset)
								store(check.root->rsp.int3(), cube.settings.mini, check.root->rsp);
							else
								store(check.root->rsp.int3(), cube.settings.mini);
							continue;
						};
					};
				};

				// check for buttons
				for (Button& button : state.buttons) {
					// get button position
					vec3x pos = button.root->attached
						? state.movingPlats[button.root->moving].now + vec3x(0, 0, 1)
						: button.root->pos;

					// check for button press
					if (inside(pos, button.root->rad, cube.grid())) {
						button.pressed = true;

						// ignore if button is already pressed
						if (button.state) continue;

						// press the button
						button.state = true;
						if (button.root->visible) Assets::sfx(Assets::Button);

						// lock camera
						if (button.root->camera)
							state.lock = button.root->lock == 0xFFFF ? -1ULL : button.root->lock;

						// execute button commands
						button.execute(state.movingPlats, state.buttons, state.bumpers);
						continue;
					};
					button.pressed = false;
				};

				// check for teleporters
				for (const Bytes::Teleport& port : root.ports) {
					if (inside(port.pos, port.rad, cube.grid())) {
						cube.attach(state.movingPlats, -1ULL);

						// teleportation offset
						vec3x off = port.tp - cube.tile;

						// translate player
						cube.tile = port.tp;

						// translate camera
						if (id == state.player && state.lock == -1ULL)
							camera.pos += off.gl();
					};
				};

				// update bumper counter
				if (cube.bump) { cube.bump--; ignorefall = cube.bump >= 1; };

				// check for falling platform
				for (Falling& falling : state.fallingPlats) {
					if (falling.pos + vec3x(0, 0, 1) == cube.grid()) {
						// trigger falling platform
						falling.trigger();
					};
				};

				// check for missing ground
				size_t src = -1ULL;
				if (!checkpos(cube.minipos() - up, false, cube.settings.mini, src) && !ignorefall) {
					cube.act = Cube::Fall;
					cube.speed.z = 0fx;
					cube.fallimmune = 0;
					if (id == state.player) Assets::sfx(Assets::Fall);
					return;
				};
				if (!ignorefall) {
					cube.attach(state.movingPlats, src);
					if (src == -1ULL) checkAlign(cube);
					else cube.tile = align(cube.tile, cube.settings.mini);
				};

				// check for resizer
				for (const Resizer& resizer : state.resizers) {
					if (inside(resizer.root->pos, resizer.root->rad, cube.grid())) {
						// ignore if cube size matches
						if (resizer.root->shrinks() == cube.settings.mini)
							continue;

						// play resize sound effect
						Assets::sfx(resizer.root->shrinks() ? Assets::Shrink : Assets::Grow);

						// trigger resize animation
						cube.act = resizer.root->shrinks() ? Cube::Shrink : Cube::Grow;
						cube.t = 0;
						if (!resizer.root->shrinks()) {
							// translate to unit coordinates
							cube.animoff = {0, 0, 0};
							cube.mini.x--;
							cube.mini.y--;

							// convert mini offset to normal coordinates
							if (cube.mini.x != 0 || cube.mini.y != 0) {
								cube.tile.x += (fix)cube.mini.x / 3fx;
								cube.tile.y += (fix)cube.mini.y / 3fx;
								cube.mini.x = 0;
								cube.mini.y = 0;
							};
						};
						return;
					};
				};

				// get movement direction
				Direction dir;
				if (dx == 0fx && dy == 0fx) return;
				dir = directionFromSteer(dx, dy);

				// check obstacles
				size_t _;
				if (checkpos(cube.minipos() + up, true, cube.settings.mini, _))
					return;

				// calculate action type
				vec3x off = vec3x::fromDir(dir) * unit;
				bool hangpoint = checkpos(cube.minipos() + off, false, cube.settings.mini, src)
					|| checkpos(cube.minipos() + off + up, true, cube.settings.mini, src);

				// check for no hanging space
				if (hangpoint && (checkpos(cube.minipos() - off, false, cube.settings.mini, _)
					|| checkpos(cube.minipos() - off + up, true, cube.settings.mini, _)))
					return;

				// check for invalid movement
				if (!validmove(cube.minipos() + off, dir, cube.settings.mini))
					return;

				// reattach cube
				if (!ignorefall) {
					cube.attach(state.movingPlats, src);
					if (src == -1ULL) checkAlign(cube);
					else cube.tile = align(cube.tile, cube.settings.mini);
				};

				// start new action
				cube.act = hangpoint ? Cube::Hang : Cube::Roll;
				cube.dir = dir;
				cube.dt = 0fx;

				// reset momentum if not bumped
				if (cube.dir != directionFromSteer(cube.speed.x, cube.speed.y) && cube.bump == 0) {
					cube.speed.x = 0fx;
					cube.speed.y = 0fx;
				};
				return;
			};

			// check for action in progress
			vec3x base;
			switch (cube.act) {
				case Cube::Roll: {
					// apply steering
					switch (cube.dir) {
						case East : cube.speed.x = speedclamp(cube.speed.x + rollsteer * dx, rollspeed, clamp * 2fx); break;
						case West : cube.speed.x = speedclamp(cube.speed.x + rollsteer * dx, rollspeed, clamp * 2fx); break;
						case North: cube.speed.y = speedclamp(cube.speed.y + rollsteer * dy, rollspeed, clamp * 2fx); break;
						case South: cube.speed.y = speedclamp(cube.speed.y + rollsteer * dy, rollspeed, clamp * 2fx); break;
					};
					cube.dt = cube.t;
					switch (cube.dir) {
						case East : cube.t -= cube.speed.x; break;
						case West : cube.t += cube.speed.x; break;
						case North: cube.t -= cube.speed.y; break;
						case South: cube.t += cube.speed.y; break;
					};

					// apply redirection momentum decay
					if (!(dx == 0fx && dy == 0fx) && cube.dir != directionFromSteer(dx, dy)) {
						switch (cube.dir) {
							case East : cube.speed.x += (cube.t >= 0.5fx) ? -clamp : clamp; break;
							case West : cube.speed.x -= (cube.t >= 0.5fx) ? -clamp : clamp; break;
							case North: cube.speed.y += (cube.t >= 0.5fx) ? -clamp : clamp; break;
							case South: cube.speed.y -= (cube.t >= 0.5fx) ? -clamp : clamp; break;
						};
					};

					// apply momentum decay
					if (dx == 0fx && cube.speed.x != 0fx) cube.speed.x = approach(cube.speed.x, 0fx, rollsteer);
					if (dy == 0fx && cube.speed.y != 0fx) cube.speed.y = approach(cube.speed.y, 0fx, rollsteer);

					// apply gravity
					fix g = (cube.t >= 0.5fx ? 1 : -1);
					cube.t += g * rollg;
					cube.dt -= cube.t;
				}; break;
				case Cube::Hang: {
					// get hang base position
					base = cube.minipos();
					base.x += (fix)(directionVec(cube.dir).x * unit);
					base.y += (fix)(directionVec(cube.dir).y * unit);

					// check for hang base change
					size_t src_u;
					size_t src_l;
					bool check_u = checkpos(base + up, false, cube.settings.mini, src_u);
					bool check_l = checkpos(base, false, cube.settings.mini, src_l);
					if (check_u || check_l) {
						// calculate higher priority source
						size_t src;
						if (!check_l) src = src_u;
						else if (!check_u) src = src_l;
						else src = src_u == -1ULL ? src_l : src_u;

						// attach to new source
						cube.attach(state.movingPlats, src);
					} else {
						// force into idle state
						cube.act = Cube::HangQB;
						cube.clampspeed = 0.08fx;
						cube.clamp = -1ULL;
						break;
					};

					// update animation progress
					switch (cube.dir) {
						case East : cube.speed.x = speedclamp(cube.speed.x + hangsteer * dx, hangspeed, clamp * 2fx); break;
						case West : cube.speed.x = speedclamp(cube.speed.x + hangsteer * dx, hangspeed, clamp * 2fx); break;
						case North: cube.speed.y = speedclamp(cube.speed.y + hangsteer * dy, hangspeed, clamp * 2fx); break;
						case South: cube.speed.y = speedclamp(cube.speed.y + hangsteer * dy, hangspeed, clamp * 2fx); break;
					};
					cube.dt = cube.t;
					switch (cube.dir) {
						case East : cube.t -= cube.speed.x; break;
						case West : cube.t += cube.speed.x; break;
						case North: cube.t -= cube.speed.y; break;
						case South: cube.t += cube.speed.y; break;
					};

					// apply momentum decay
					if (dx == 0fx && cube.speed.x != 0fx) cube.speed.x = approach(cube.speed.x, 0fx, rollsteer);
					if (dy == 0fx && cube.speed.y != 0fx) cube.speed.y = approach(cube.speed.y, 0fx, rollsteer);

					// apply gravity
					fix g = -(1fx + cube.t * 1.5fx) * 1.125fx;
					cube.t += g * hangg;
					cube.dt -= cube.t;

					// check for hangtime start
					if (!cube.hang && cube.dt > (cube.settings.mini ? (1fx / 128fx) : (1fx / 64fx)) && !(dx == 0fx && dy == 0fx))
						cube.hang = cube.dir == directionFromSteer(dx, dy);
				}; break;
				case Cube::HangQB: {
					// force clamp
					if (cube.clamp == -1ULL) {
						cube.dt = cube.t;
						cube.t -= cube.clampspeed * T;
						cube.dt -= cube.t;
						break;
					};
				}; break;
				default: break;
			};

			// update cube hang time
			if (cube.hang) {
				// increment hang time
				auto it = hang.find(id);
				if (it == hang.end())
					hang[id] = delta;
				else
					it->second += delta;

				// update level timer
				if (id == state.player)
					timer.updateEdge(delta);
			} else {
				// remove hang time
				auto it = hang.find(id);
				if (it != hang.end())
					hang.erase(it);
			};

			// check for state transitions
			switch (cube.act) {
				// check for roll animation boundaries
				case Cube::Roll: case Cube::RollQF: case Cube::RollQB: {
					if (cube.t <= 0fx) {
						cube.act = Cube::Idle;
						cube.t = 0fx;
					} else if (cube.t >= 1fx) {
						glm::vec2 off = directionVec(cube.dir);
						sf::Vector3i offi = sf::Vector3i(off.x, off.y, 0) * unit;

						addsplit(cube.tile, cube.mini, offi);
						cube.act = Cube::Idle;
						cube.t = 0fx;

						// reset momentum
						if (!(dx == 0fx && dy == 0fx) && directionFromSteer(dx, dy) != cube.dir) {
							cube.speed.x = 0fx;
							cube.speed.y = 0fx;
						};

						// play step sound
						size_t _;
						if (checkpos(cube.minipos() - up, false, cube.settings.mini, _) || cube.fallimmune)
							if (id == state.player) Assets::sfx(cube.settings.mini ? Assets::Minicube : Assets::Cube);
					};
				}; break;
				// check for hang animation boundaries
				case Cube::Hang: case Cube::HangQF: case Cube::HangQB: {
					if (cube.t <= 0fx) {
						cube.act = Cube::Idle;
						cube.t = 0fx;
					} else if (cube.t >= 1fx) {
						cube.act = Cube::Fall;
						cube.t = 0fx;
						addsplit(cube.tile, cube.mini, up.int3());

						// prepare roll transition
						if (cube.settings.mini) {
							cube.fallimmune = 1;
							if (id == state.player) Assets::sfx(Assets::Minicube);
						} else {
							size_t _;
							cube.fallimmune = checkpos(base + up, true, cube.settings.mini, _) ? 0 : 1;
							if (!cube.fallimmune && id == state.player) Assets::sfx(Assets::Cube);
						};
						cube.speed.z = 0fx;
					};
				}; break;
				default: break;
			};
		};

		/// Perform collection checks on cube.
		/// @param id Cube ID.
		void collect(size_t id) {
			Cube& cube = state.cubes[id];

			// check each prism
			for (size_t i = 0; i < state.prisms.size(); i++) {
				const Prism& prism = state.prisms[i];
				if (prism.in(cube.volume)) {
					// check if prism was already triggered
					if (state.prism == i)
						continue;

					// play prism sound effect
					if (mutePrism)
						mutePrism = false;
					else prism.sfx();

					// store last prism index if in worldmap
					if (world) state.prism = i;

					// remove prism
					else {
						state.prisms.erase(state.prisms.begin() + i);
						cube.energy = 1fx;
					};
				};
			};
		};

		/// Processes route collection effects.
		/// @param routes Routes collection.
		/// @param active List of active level numbers.
		void effect(const Bytes::Routes& routes, const std::vector<size_t>& active) {
			// activate appropriate buttons
			for (uint16_t idx : active) {
				for (uint16_t btn : routes.routes[idx].buttons) {
					state.buttons[btn].proc(state.movingPlats, state.buttons, state.bumpers);
					state.buttons[btn].state = true;
				};
			};

			// recolor prisms
			for (uint16_t idx : routes.dark_prisms)
				if (idx < state.prisms.size())
					state.prisms[idx].dark = true;
		};

		/// Creates a crush animation.
		/// @param cube Cube object.
		void animCrush(Cube& cube) {
			// create sound effect
			Assets::sfx(Assets::Bump);
			Assets::sfx(Assets::Fallstop);

			// create particles
			glm::vec3 offset = (cube.pos() + vec3x(0.5fx)).gl();
			for (int z = -1; z <= 1; z++) {
				for (int y = -1; y <= 1; y++) {
					for (int x = -1; x <= 1; x++) {
						Dust dust;
						dust.pos = Accel<glm::vec3>(
							glm::vec3(
								x / 2.0f + uniform() * 0.25f,
								y / 2.0f + uniform() * 0.25f,
								z / 2.0f + uniform() * 0.25f
							) + offset,
							glm::vec3(
								x / 2.0f + uniform() * 0.25f,
								y / 2.0f + uniform() * 0.25f,
								z / 2.0f + uniform() * 0.25f
							) * 5.0f,
							glm::vec3(0.0f)
						);
						dust.dim = Accel<float>(0.5, -1, -0.25);
						dust.off = rand() & 0xFF;
						dust.mode = (rand() % 3) + (cube.dark ? 3 : 0);
						state.particles.add(dust);
					};
				};
			};
		};

		/// Creates an exit animation.
		/// @param cube Cube object.
		void animExit(Cube& cube) {
			// create sound effect
			Assets::sfx(Assets::Exit);
			Assets::sfx(Assets::Fallstop);

			// create swirl particles
			glm::vec3 offset = (cube.pos() + vec3x(0.5fx)).gl();
			for (int z = -3; z <= 3; z++) {
				for (int y = -3; y <= 3; y++) {
					for (int x = -3; x <= 3; x++) {
						Dust dust;
						dust.pos = Accel<glm::vec3>(
							glm::vec3(x / 6.0f, y / 6.0f, z / 6.0f) + offset,
							glm::vec3(0.0f, uniformRange(0.1f, 3.0f), 0.0f),
							glm::vec3(0.0f)
						);
						dust.dim = Accel<float>(
							0.5 + uniform() * 0.1,
							-uniformRange(0.06f, 0.3f),
							0.0f
						);
						dust.off = rand() & 0xFF;
						dust.mode = (rand() % 3) + (cube.dark ? 3 : 0);
						state.particles.add(dust);
					};
				};
			};
			// create crush particles
			for (int z = -1; z <= 1; z++) {
				for (int y = -1; y <= 1; y++) {
					for (int x = -1; x <= 1; x++) {
						Dust dust;
						dust.pos = Accel<glm::vec3>(
							glm::vec3(
								x / 2.0f + uniform() * 0.25f,
								y / 2.0f + uniform() * 0.25f,
								z / 2.0f + uniform() * 0.25f
							) + offset,
							glm::vec3(
								x / 2.0f + uniform() * 0.25f,
								y / 2.0f + uniform() * 0.25f,
								z / 2.0f + uniform() * 0.25f
							) * 2.0f,
							glm::vec3(0.0f)
						);
						dust.dim = Accel<float>(0.5, -1, -0.25);
						dust.off = rand() & 0xFF;
						dust.mode = (rand() % 3) + (cube.dark ? 3 : 0);
						state.particles.add(dust);
					};
				};
			};
		};
	};
};