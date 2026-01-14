#pragma once

/// Level related namespace.
namespace Level {
	/// Cube object.
	struct Cube {
		/// Cube action type.
		enum Action {
			Idle  , // No action.
			Fall  , // Falling.
			Roll  , // Horizontal movement.
			Hang  , // Vertical movement.
			RollQF, // Forward roll clamping.
			RollQB, // Backward roll clamping.
			HangQF, // Forward hang clamping.
			HangQB, // Backward hang clamping.
			Shrink, // Shrinking animation.
			Grow  , // Growing animation.
			Align , // Terrain alignment.
			Crush , // Crush animation.
			Exit  , // Level complete animation.
		};

		/// Returns a string representation of an action.
		/// @param action Converted action variable.
		static const char* actionName(Action action) {
			const char* table[] {
				"Idle", "\4Fall\r", "Roll", "Hang",
				"Roll Clamp \6+\7", "Roll Clamp \6-\7",
				"Hang Clamp \6+\7", "Hang Clamp \6-\7",
				"\4Shrink\r", "\4Grow\r",
				"\4Align\r", "\6Crush\7", "\6Exit\7"
			};
			return (action < 0 || action > Exit) ? "\6???\7" : table[action];
		};

		/// Cube keymap.
		KeyMap keys;

		/// Cube relative offset.
		vec3x offset;
		/// Cube tile position.
		vec3x tile;
		/// Cube mini tile offset.
		sf::Vector3i mini;
		/// Cube animation offset.
		vec3x animoff;
		/// Relative moving platform index.
		size_t rel;
		/// Clamping moving platform index.
		size_t clamp;
		/// Invalid state clamp speed.
		fix clampspeed;
		/// Clamping direction.
		Direction clampdir;

		/// Cube grid position.
		vec3x grid() const { return offset + tile; };
		/// Cube position in mini coordinates.
		vec3x minipos() const { return (offset + tile) * 3fx + vec3x(mini.x, mini.y, mini.z); };
		/// Cube true position.
		vec3x pos() const { return minipos() / 3fx - animoff; };
		/// Cube collision volume.
		vol3x volume;

		/// Cube action type.
		Action act;
		/// Cube action direction.
		Direction dir;
		/// Cube action progress.
		fix t;
		/// Cube action speed.
		fix dt;
		/// Cube speed vector.
		vec3x speed;
		/// Cube speed multiplier.
		fix energy;
		/// Cube bump count.
		uint32_t bump;
		/// Cube hang fall immunity frames.
		uint32_t fallimmune;
		/// Cube align state immunity frames.
		uint32_t alignimmune;
		/// Whether the cube has just climbed up.
		bool climb;
		/// Whether the cube is gaining hang time.
		bool hang;
		/// Whether the cube is a darkcube.
		bool dark;
		/// Whether the cube is a player.
		bool player;

		/// Cube model.
		Object* mod_cube = NULL;
		/// Cube shadow model.
		Object* mod_shadow = NULL;
		/// Cube render settings.
		Models::Cube::Settings settings = {
			.shadow = Models::Cube::shadowZ,
			.mini = false
		};

		/// Cube destructor.
		~Cube () { delete mod_cube; delete mod_shadow; mod_cube = NULL; mod_shadow = NULL; };

		/// Default constructor.
		Cube (sf::Vector3i spawn, bool mini, bool dark, bool player):
			rel(-1ULL), clamp(-1ULL), clampspeed(0fx),
			alignimmune(0), hang(false), dark(dark), player(player)
		{
			// set default state
			offset = vec3x(0);
			tile = { spawn.x, spawn.y, spawn.z };
			act = Cube::Fall;
			dir = East;
			t = 0;
			dt = 0;
			clampdir = East;
			speed = vec3x(0);
			energy = 0;
			bump = 0;
			fallimmune = 0;
			climb = false;

			// cube size
			if (mini) {
				this->mini = { 1, 1, 0 };
				settings.size = Models::small;
				settings.shadow = Models::Prism::shadowZ;
				settings.mini = true;
				animoff = vec3x(Models::small, Models::small, 0);
			} else {
				this->mini = { 0, 0, 0 };
				settings.size = 1fx;
				settings.shadow = Models::Cube::shadowZ;
				settings.mini = false;
				animoff = vec3x(0);
			};

			// generate cube models
			model();
		};

		/// Copy constructor.
		/// @param cube Copied cube object.
		/// @note Copy will ignore model pointers.
		Cube (const Cube& cube):
			keys(cube.keys),
			offset(cube.offset),
			tile(cube.tile),
			mini(cube.mini),
			animoff(cube.animoff),
			rel(cube.rel),
			clamp(cube.clamp),
			clampspeed(cube.clampspeed),
			clampdir(cube.clampdir),
			volume(cube.volume),
			act(cube.act),
			dir(cube.dir),
			t(cube.t),
			dt(cube.dt),
			speed(cube.speed),
			energy(cube.energy),
			bump(cube.bump),
			fallimmune(cube.fallimmune),
			alignimmune(cube.alignimmune),
			climb(cube.climb),
			hang(cube.hang),
			dark(cube.dark),
			player(cube.player),
			settings(cube.settings) {};

		/// Copy assignment.
		/// @param cube Copied cube object.
		/// @note Copy will ignore model pointers.
		Cube& operator=(const Cube& cube) {
			keys = cube.keys;
			offset = cube.offset;
			tile = cube.tile;
			mini = cube.mini;
			animoff = cube.animoff;
			rel = cube.rel;
			clamp = cube.clamp;
			clampspeed = cube.clampspeed;
			clampdir = cube.clampdir;
			volume = cube.volume;
			act = cube.act;
			dir = cube.dir;
			t = cube.t;
			dt = cube.dt;
			speed = cube.speed;
			energy = cube.energy;
			bump = cube.bump;
			fallimmune = cube.fallimmune;
			alignimmune = cube.alignimmune;
			climb = cube.climb;
			hang = cube.hang;
			dark = cube.dark;
			player = cube.player;
			settings = cube.settings;
			return *this;
		};

		/// Generates a cube model if needed.
		void model() {
			if (mod_cube == NULL) {
				mod_cube   = new Object;
				mod_shadow = new Object;
				Models::Cube::generate(*mod_cube, *mod_shadow);
			};
		};

		/// Updates cube state.
		/// @param delta Elapsed time since last frame.
		/// @param model Whether to update the models.
		void update(float delta, bool model) {
			this->model();

			// update model position
			mod_cube->pos = pos().gl();
			mod_shadow->pos = mod_cube->pos;

			// animate model
			switch (act) {
				case Crush: case Exit:
					volume = vol3x();
					break;
				case Roll: case RollQF: case RollQB:
					volume = Models::Cube::roll(*mod_cube, *mod_shadow, dir, t, settings);
					break;
				case Hang: case HangQF: case HangQB:
					volume = Models::Cube::hang(*mod_cube, *mod_shadow, dir, t, settings);
					break;
				case Shrink:
					volume = Models::Cube::resize(*mod_cube, *mod_shadow, t, settings.shadow, settings.size);
					break;
				case Grow:
					volume = Models::Cube::resize(*mod_cube, *mod_shadow, 1fx - t, settings.shadow, settings.size);
					break;
				default:
					volume = Models::Cube::roll(*mod_cube, *mod_shadow, dir, t, settings);
					break;
			};
			volume += pos();

			// update model colors
			if (dark) {
				switch (act) {
					case Crush: case Exit:
						break;
					case Roll: case RollQF: case RollQB: case Hang: case HangQF: case HangQB: {
						switch (dir) {
							case East: case West:
								Models::Prism::colorX(*mod_cube, Rainbow::darkGradXY(t));
								Models::Prism::colorY(*mod_cube, Rainbow::darkGradYX(t));
								Models::Prism::colorZ(*mod_cube, Rainbow::darkPrismZ());
								break;
							case North: case South:
								Models::Prism::colorX(*mod_cube, Rainbow::darkPrismX());
								Models::Prism::colorY(*mod_cube, Rainbow::darkGradYZ(t));
								Models::Prism::colorZ(*mod_cube, Rainbow::darkGradZY(t));
								break;
						};
					}; break;
					default:
						Models::Prism::colorX(*mod_cube, Rainbow::darkPrismX());
						Models::Prism::colorY(*mod_cube, Rainbow::darkPrismY());
						Models::Prism::colorZ(*mod_cube, Rainbow::darkPrismZ());
						break;
				};
			} else {
				switch (act) {
					case Crush: case Exit:
						break;
					case Roll: case RollQF: case RollQB: case Hang: case HangQF: case HangQB: {
						switch (dir) {
							case East: case West:
								Models::Prism::colorX(*mod_cube, Rainbow::gradXY(player, t));
								Models::Prism::colorY(*mod_cube, Rainbow::gradYX(player, t));
								Models::Prism::colorZ(*mod_cube, Rainbow::cubeZ(player));
								break;
							case North: case South:
								Models::Prism::colorX(*mod_cube, Rainbow::cubeX(player));
								Models::Prism::colorY(*mod_cube, Rainbow::gradYZ(player, t));
								Models::Prism::colorZ(*mod_cube, Rainbow::gradZY(player, t));
								break;
						};
					}; break;
					default:
						Models::Prism::colorX(*mod_cube, Rainbow::cubeX(player));
						Models::Prism::colorY(*mod_cube, Rainbow::cubeY(player));
						Models::Prism::colorZ(*mod_cube, Rainbow::cubeZ(player));
						break;
				};
			};

			// update models
			mod_cube->update();
			mod_shadow->update();
		};

		/// Draws the cube.
		/// @return Drawn vertex count.
		size_t draw() {
			model();
			if (act == Crush || act == Exit) return 0;
			Assets::mask.bind();
			mod_cube->pos = pos().gl();
			return mod_cube->draw(Shaders::tex);
		};

		/// Draws cube's shadow.
		/// @return Drawn vertex count.
		size_t shadow() {
			model();
			if (act == Crush || act == Exit) return 0;
			Assets::mask.bind();
			mod_shadow->pos = pos().gl();
			return mod_shadow->draw(Shaders::tex);
		};

		/// Get current cube input.
		/// @param player Whether the cube is a player.
		CubeInput getInput(bool player) {
			if (player)
				keys.set(CubeInput::keyboard());
			return keys.state();
		};

		/// Immediately shrinks the cube.
		void shrink() {
			act = Cube::Idle;
			t = 0fx;
			settings.mini = true;
			animoff = {Models::small, Models::small, 0};
			mini = {1, 1, 0};
		};

		/// Immediately grows the cube.
		/// @param normalize Whether to normalize cube position.
		void grow(bool normalize) {
			if (normalize) {
				// translate to unit coordinates
				animoff = {0, 0, 0};
				mini.x--;
				mini.y--;

				// convert mini offset to normal coordinates
				if (mini.x != 0 || mini.y != 0) {
					tile.x += (fix)mini.x / 3fx;
					tile.y += (fix)mini.y / 3fx;
					mini.x = 0;
					mini.y = 0;
				};
			};

			act = Cube::Idle;
			t = 0fx;
			settings.mini = false;
			tile.x = tile.x.round();
			tile.y = tile.y.round();
			tile.z = tile.z.round();
		};

		/// Checks if the cube is steerable.
		bool steerable() const { return act == Idle || act == Roll || act == Hang; };

		/// Checks if the cube is moving.
		bool moving() const { return act >= Roll && act <= HangQB; };

		/// Checks if the cube does not exist.
		bool inactive() const { return act == Crush || act == Exit; };

		/// Attaches cube to a moving platform.
		/// @param plats Moving platform list.
		/// @param idx Moving platform index.
		void attach(const std::vector<Moving>& plats, size_t idx) {
			// fetch platform positions
			vec3x src = rel != -1ULL ? plats[rel].now : vec3x(0);
			vec3x dst = idx != -1ULL ? plats[idx].now : vec3x(0);

			// translate cube basis
			offset += dst - src;
			tile -= dst - src;
			rel = idx;
		};

		/// Syncs cube relative offset.
		/// @param plats Moving platform list.
		/// @param tick Moving platform tick timer pointer.
		void syncOffset(const std::vector<Moving>& plats, float* tick) {
			offset = rel != -1ULL ? plats[rel].now : vec3x(0);
			if (rel == -1ULL || tick == NULL) return;

			// update tick timer
			float vel = (float)(plats[rel].velocity().sqr() * 27fx);
			*tick = (vel == 0.0f) ? 0.9f : *tick + vel;
			if (floorf(*tick)) {
				*tick = fmod(*tick, 1.0f);
				Assets::sfx(Assets::Tick);
			};
		};

		/// Sets cube energy.
		/// @return Self-reference.
		Cube& setEnergy(fix energy) {
			this->energy = energy;
			return *this;
		};
	};
};