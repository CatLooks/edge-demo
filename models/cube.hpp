#pragma once

/// Cube model generation namespace.
namespace Models {
	/// Cube modeling.
	namespace Cube {
		/// Cube rendering settings.
		struct Settings {
			fix size;     /// Cube size.
			float shadow; /// Shadow height.
			bool mini;    /// Whether the cube is shrinked.
		};

		/// Cube shadow height.
		const float shadowZ = -4.0f;

		/// Generate a cube model.
		/// @param object Cube model.
		/// @param shadow Shadow model.
		void generate(Object& object, Object& shadow) {
			Vertex buffer[24];

			// cube dimensions
			glm::vec3 min = glm::vec3(-0.5f);
			glm::vec3 max = glm::vec3( 0.5f);

			// generate cube vertices
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					buffer[f * 4 + i] = Vertex(vertID(min, max, faces[f][i]), Textures::Mask::white.coords[i]);
			};

			// update geometry
			object.set(buffer, lengthof(buffer));
			object.indexQuads();

			// generate shadow
			Shadow::generate(shadow, min, max, shadowZ, true);
		};

		/// Returns center of mass of cube model.
		/// @param object Cube model.
		glm::vec3 mass(const Object* object) {
			if (object == NULL) return glm::vec3(0.0f, 0.0f, 0.0f);
			return (object->vertices()[0].pos + object->vertices()[6].pos) * 0.5f + object->pos;
		};

		/// Cube roll animation.
		/// @param object Cube model.
		/// @param shadow Shadow model.
		/// @param dir Animation direction.
		/// @param t Animation progress.
		/// @param settings Animation settings.
		/// @note The roll displaces cube from (0, 0, 0) to unit direction vector R * (x, 0, z).
		/// @return Animation collision volume.
		vol3x roll(Object& object, Object& shadow, Direction dir, fix t, const Settings& settings) {
			// calculate cosine & sine values
			float cosv = cosf((float)t * M_PI_2);
			float sinv = sinf((float)t * M_PI_2);

			/// 0 = X-Y-
			/// 1 = X-Y+
			/// 2 = X+Y-
			/// 3 = X+Y+

			// cube size
			float R = settings.mini ? small : 1.0f;

			// fixed coordinate
			const float nf = -0.5f * R;
			const float pf =  0.5f * R;

			// negative rotation points
			glm::vec2 n0 = glm::vec2(-R / 2, -0.5f);
			glm::vec2 n1 = n0 + glm::vec2( -sinv, cosv ) * R;
			glm::vec2 n2 = n0 + glm::vec2(  cosv, sinv ) * R;
			glm::vec2 n3 = n1 + n2 - n0;

			// positive rotation points
			glm::vec2 p2 = glm::vec2(R / 2, -0.5f);
			glm::vec2 p3 = p2 + glm::vec2(  sinv, cosv ) * R;
			glm::vec2 p0 = p2 + glm::vec2( -cosv, sinv ) * R;
			glm::vec2 p1 = p0 + p3 - p2;

			// calculate vertex positions
			glm::vec3 verts[8];
			switch (dir) {
				case East:
					verts[0] = glm::vec3(n0.x, n0.y, nf);
					verts[1] = glm::vec3(n0.x, n0.y, pf);
					verts[2] = glm::vec3(n1.x, n1.y, nf);
					verts[3] = glm::vec3(n1.x, n1.y, pf);
					verts[4] = glm::vec3(n2.x, n2.y, nf);
					verts[5] = glm::vec3(n2.x, n2.y, pf);
					verts[6] = glm::vec3(n3.x, n3.y, nf);
					verts[7] = glm::vec3(n3.x, n3.y, pf);
					Shadow::update(shadow, {n1.x, nf}, {n2.x, pf}, settings.shadow, -0.5f);
					break;
				case West:
					verts[0] = glm::vec3(p0.x, p0.y, nf);
					verts[1] = glm::vec3(p0.x, p0.y, pf);
					verts[2] = glm::vec3(p1.x, p1.y, nf);
					verts[3] = glm::vec3(p1.x, p1.y, pf);
					verts[4] = glm::vec3(p2.x, p2.y, nf);
					verts[5] = glm::vec3(p2.x, p2.y, pf);
					verts[6] = glm::vec3(p3.x, p3.y, nf);
					verts[7] = glm::vec3(p3.x, p3.y, pf);
					Shadow::update(shadow, {p0.x, nf}, {p3.x, pf}, settings.shadow, -0.5f);
					break;
				case North:
					verts[0] = glm::vec3(nf, n0.y, n0.x);
					verts[1] = glm::vec3(nf, n2.y, n2.x);
					verts[2] = glm::vec3(nf, n1.y, n1.x);
					verts[3] = glm::vec3(nf, n3.y, n3.x);
					verts[4] = glm::vec3(pf, n0.y, n0.x);
					verts[5] = glm::vec3(pf, n2.y, n2.x);
					verts[6] = glm::vec3(pf, n1.y, n1.x);
					verts[7] = glm::vec3(pf, n3.y, n3.x);
					Shadow::update(shadow, {nf, n1.x}, {pf, n2.x}, settings.shadow, -0.5f);
					break;
				case South:
					verts[0] = glm::vec3(nf, p0.y, p0.x);
					verts[1] = glm::vec3(nf, p2.y, p2.x);
					verts[2] = glm::vec3(nf, p1.y, p1.x);
					verts[3] = glm::vec3(nf, p3.y, p3.x);
					verts[4] = glm::vec3(pf, p0.y, p0.x);
					verts[5] = glm::vec3(pf, p2.y, p2.x);
					verts[6] = glm::vec3(pf, p1.y, p1.x);
					verts[7] = glm::vec3(pf, p3.y, p3.x);
					Shadow::update(shadow, {nf, p0.x}, {pf, p3.x}, settings.shadow, -0.5f);
					break;
				default:
					break;
			};

			// update vertices
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].setPos(verts[faces[f][i]]);
			};

			// calculate collision volume
			vol3x volume = vol3x::point(vec3x::fromGL(verts[0]));
			for (size_t i = 1; i < 8; i++)
				volume |= vec3x::fromGL(verts[i]);
			return volume + vec3x(0.5fx);
		};

		/// Cube hanging animation.
		/// @param object Cube model object.
		/// @param shadow Shadow model.
		/// @param dir Animation direction.
		/// @param t Animation progress.
		/// @param settings Animation settings.
		/// @note The roll displaces cube from (0, 0, 0) to R * (0, 1, 0).
		/// @return Animation collision volume.
		vol3x hang(Object& object, Object& shadow, Direction dir, fix t, const Settings& settings) {
			/// 0 = X-Y-
			/// 1 = X-Y+
			/// 2 = X+Y-
			/// 3 = X+Y+

			// cube size
			float R = settings.mini ? small : 1.0f;
			float T = settings.mini ? (-R / 2) : (R / 2);

			// calculate cosine & sine values
			float cosv = cosf((float)t * M_PI_2) * R;
			float sinv = sinf((float)t * M_PI_2) * R;

			// fixed coordinate
			const float nf = -0.5f * R;
			const float pf =  0.5f * R;

			// negative rotation points
			glm::vec2 n1 = glm::vec2(-R / 2, T);
			glm::vec2 n0 = n1 + glm::vec2( sinv, -cosv );
			glm::vec2 n3 = n1 + glm::vec2( cosv,  sinv );
			glm::vec2 n2 = n0 + n3 - n1;

			// positive rotation points
			glm::vec2 p3 = glm::vec2(R / 2, T);
			glm::vec2 p2 = p3 + glm::vec2( -sinv, -cosv );
			glm::vec2 p1 = p3 + glm::vec2( -cosv,  sinv );
			glm::vec2 p0 = p1 + p2 - p3;

			// calculate vertex positions
			glm::vec3 verts[8];
			switch (dir) {
				case East:
					verts[0] = glm::vec3(n0.x, n0.y, nf);
					verts[1] = glm::vec3(n0.x, n0.y, pf);
					verts[2] = glm::vec3(n1.x, n1.y, nf);
					verts[3] = glm::vec3(n1.x, n1.y, pf);
					verts[4] = glm::vec3(n2.x, n2.y, nf);
					verts[5] = glm::vec3(n2.x, n2.y, pf);
					verts[6] = glm::vec3(n3.x, n3.y, nf);
					verts[7] = glm::vec3(n3.x, n3.y, pf);
					Shadow::wall(shadow, {n1.x, nf}, {n2.x, pf}, settings.shadow, -0.5f, R, sinv, dir);
					break;
				case West:
					verts[0] = glm::vec3(p0.x, p0.y, nf);
					verts[1] = glm::vec3(p0.x, p0.y, pf);
					verts[2] = glm::vec3(p1.x, p1.y, nf);
					verts[3] = glm::vec3(p1.x, p1.y, pf);
					verts[4] = glm::vec3(p2.x, p2.y, nf);
					verts[5] = glm::vec3(p2.x, p2.y, pf);
					verts[6] = glm::vec3(p3.x, p3.y, nf);
					verts[7] = glm::vec3(p3.x, p3.y, pf);
					Shadow::wall(shadow, {p3.x, nf}, {p0.x, pf}, settings.shadow, -0.5f, R, sinv, dir);
					break;
				case North:
					verts[0] = glm::vec3(nf, n0.y, n0.x);
					verts[1] = glm::vec3(nf, n2.y, n2.x);
					verts[2] = glm::vec3(nf, n1.y, n1.x);
					verts[3] = glm::vec3(nf, n3.y, n3.x);
					verts[4] = glm::vec3(pf, n0.y, n0.x);
					verts[5] = glm::vec3(pf, n2.y, n2.x);
					verts[6] = glm::vec3(pf, n1.y, n1.x);
					verts[7] = glm::vec3(pf, n3.y, n3.x);
					Shadow::wall(shadow, {nf, n1.x}, {pf, n2.x}, settings.shadow, -0.5f, R, sinv, dir);
					break;
				case South:
					verts[0] = glm::vec3(nf, p0.y, p0.x);
					verts[1] = glm::vec3(nf, p2.y, p2.x);
					verts[2] = glm::vec3(nf, p1.y, p1.x);
					verts[3] = glm::vec3(nf, p3.y, p3.x);
					verts[4] = glm::vec3(pf, p0.y, p0.x);
					verts[5] = glm::vec3(pf, p2.y, p2.x);
					verts[6] = glm::vec3(pf, p1.y, p1.x);
					verts[7] = glm::vec3(pf, p3.y, p3.x);
					Shadow::wall(shadow, {nf, p3.x}, {pf, p0.x}, settings.shadow, -0.5f, R, sinv, dir);
					break;
				default:
					break;
			};

			// update vertices
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].setPos(verts[faces[f][i]]);
			};

			// calculate collision volume
			vol3x volume = vol3x::point(vec3x::fromGL(verts[0]));
			for (size_t i = 1; i < 8; i++)
				volume |= vec3x::fromGL(verts[i]);
			return volume + vec3x(0.5fx);
		};

		/// Cube resize animation.
		/// @param object Cube model object.
		/// @param shadow Shadow model.
		/// @param t Animation progress (0 = normal, 1 = small).
		/// @param shadowZ Shadow height output.
		/// @param size Cube size output.
		/// @return Animation collision volume.
		vol3x resize(Object& object, Object& shadow, fix t, float& shadowZ, fix& size) {
			// calculate animation interpolator
			float i = sinf((float)t * M_PI_2);

			// calculate cube size
			float s = lerpf(1.0f, small, i);
			size = (fix)s;

			/// Half of cube size.
			const float h = s * 0.5f;

			// calculate new cube dimensions
			glm::vec3 min = glm::vec3(-h,   - 0.5f, -h);
			glm::vec3 max = glm::vec3( h, s - 0.5f,  h);

			// update cube vertices
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].setPos(vertID(min, max, faces[f][i]));
			};

			// update shadow
			shadowZ = lerpf(Cube::shadowZ, Prism::shadowZ, i);
			Shadow::update(shadow, glm::vec2(min.x, min.z), glm::vec2(max.x, max.z), shadowZ, -0.5f);

			// return collision volume
			return vol3x::fromTo(vec3x::fromGL(min), vec3x::fromGL(max)) + vec3x(0.5fx);
		};
	};
};