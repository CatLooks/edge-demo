#pragma once

/// Model generation namespace.
namespace Models {
	/// Particle object.
	namespace Particle {
		static Object* object = NULL;

		/// Unloads particle model.
		void unload() {
			if (object)
				delete object;
		};

		/// Generates particle model.
		Object& model() {
			if (object == NULL) {
				// particle vertices
				std::vector<Vertex> verts = {
					Vertex({-0.5f,  0.0f, 0.0f}, Textures::Mask::white.coords[0]),
					Vertex({ 0.0f, -0.5f, 0.0f}, Textures::Mask::white.coords[1]),
					Vertex({ 0.0f, +0.5f, 0.0f}, Textures::Mask::white.coords[2]),
					Vertex({+0.5f,  0.0f, 0.0f}, Textures::Mask::white.coords[3])
				};

				// generate particle object
				object = new Object;
				object->set(verts);
				object->indexQuads();
				object->update();
				object->rotZ = glm::radians(45.0f);
			};
			return *object;
		};

		/// Acceleration controlled value.
		template <typename T> struct Accel {
			T s; // Position.
			T v; // Velocity.
			T a; // Acceleration.

			/// Empty constructor.
			Accel () {};
			/// Full constructor.
			/// @param pos Initial position.
			/// @param vel Initial velocity.
			/// @param acc Initial acceleration.
			Accel (T pos, T vel, T acc): s(pos), v(vel), a(acc) {};

			/// Sets displacement.
			Accel& setDis(T pos) { s = pos; return *this; };
			/// Sets velocity.
			Accel& setVel(T vel) { v = vel; return *this; };
			/// Sets acceleration.
			Accel& setAcc(T acc) { a = acc; return *this; };

			/// Updates value's state.
			/// @param delta Time elapsed since last frame.
			void update(float delta) {
				v += a * delta;
				s += v * delta;
			};
		};

		/// Dust particle object.
		struct Dust {
			Accel<glm::vec3> pos; /// Dust position.
			Accel<float>     dim; /// Dust size.
			uint8_t          off; /// Dust rainbow offset.
			uint8_t         mode; /// Dust rainbow mode.

			/// Updates dust's state.
			/// @param delta Time elapsed since last frame.
			void update(float delta) {
				pos.update(delta);
				dim.update(delta);
			};
		};

		/// Collection of dust particles.
		class System {
			protected:
			/// Dust particle list.
			std::vector<Dust> dustlist;

			public:
			/// Updates all dust parameters.
			/// @param delta Time elapsed since last frame.
			void update(float delta) {
				/// Update all particles.
				for (Dust& dust : dustlist) {
					if (dust.dim.s > 0)
						dust.update(delta);
				};
			};

			/// Adds a dust particle into the system.
			void add(Dust dust) { dustlist.push_back(dust); };

			/// Draws dust particles.
			/// @return Drawn vertex count.
			size_t draw() const {
				Assets::mask.bind();
				size_t vertcount = 0;
				for (const Dust& dust : dustlist) {
					// check if particle is active
					if (dust.dim.s <= 0) continue;

					// set model parameters
					model().pos = dust.pos.s;
					model().scale = glm::vec3(dust.dim.s);
					model().color = Rainbow::dust(dust.off, dust.mode);

					// draw model
					vertcount += model().draw(Shaders::tex);
				};
				return vertcount;
			};

			/// Clears all particles.
			void clear() { dustlist.clear(); };
		};
	};
};