#pragma once

/// 3D volume object
struct vol3x {
	vec3x pos; // Volume position.
	vec3x dim; // Volume dimensions.

	/// Empty constructor.
	/// @note Defaults to 0x0x0 at (0, 0, 0).
	vol3x () {};
	/// Dimension constructor.
	/// @param size Volume size.
	/// @note Position defaults to (0, 0, 0).
	vol3x (vec3x size): dim(size) {};
	/// Full constructor.
	/// @param pos Volume position.
	/// @param size Volume size.
	vol3x (vec3x pos, vec3x size): pos(pos), dim(size) {};

	/// Returns a volume created from a single point.
	/// @param point Volume point.
	static vol3x point(vec3x point) { return vol3x(point, vec3x(0)); };
	/// Creates a volume from 2 points.
	/// @param min Minimum coordinates point.
	/// @param max Maximum coordinates point.
	static vol3x fromTo(vec3x min, vec3x max) { return vol3x(min, max - min); };
	/// Creates a volume from a point and a radius.
	/// @param center Volume center.
	/// @param radius Volume half-size.
	static vol3x at(vec3x center, vec3x radius) { return vol3x(center - radius, radius * 2fx); };

	/// Returns the middle point of volume.
	inline vec3x mid() const { return pos + dim / 2; };
	/// Returns the outer point of volume.
	inline vec3x out() const { return pos + dim; };

	/// Offsets a volume by a displacement vector.
	/// @param off Offset vector.
	/// @return New volume object.
	vol3x operator+(const vec3x& off) const { return vol3x(pos + off, dim); };
	/// Offsets a volume by a displacement vector.
	/// @param off Offset vector.
	/// @return New volume object.
	vol3x operator-(const vec3x& off) const { return vol3x(pos - off, dim); };
	/// Returns a volume that contains original volume and a point.
	/// @param point Contained point.
	/// @return New volume object.
	vol3x operator|(const vec3x& point) const { return fromTo(vec3x::min(pos, point), vec3x::max(pos + dim, point)); };

	/// Offsets a volume by an offset vector.
	/// @param off Offset vector.
	/// @return Self-reference.
	vol3x& operator+=(const vec3x& off) { return *this = *this + off; };
	/// Offsets a volume by an offset vector.
	/// @param off Offset vector.
	/// @return Self-reference.
	vol3x& operator-=(const vec3x& off) { return *this = *this - off; };
	/// Joins a volume with a point.
	/// @param off Joined point.
	/// @return Self-reference.
	vol3x& operator|=(const vec3x& point) { return *this = *this | point; };

	/// Generates volume intersection.
	/// @param oth Other volume.
	vol3x operator&(const vol3x& oth) const {
		vec3x vmin = {
			pos.x.max(oth.pos.x),
			pos.y.max(oth.pos.y),
			pos.z.max(oth.pos.z)
		};
		vec3x vmax = {
			out().x.min(oth.out().x),
			out().y.min(oth.out().y),
			out().z.min(oth.out().z)
		};
		if (vmin.x >= vmax.x || vmin.y >= vmax.y || vmin.z >= vmax.z)
			return vol3x(vmin, vec3x(0));
		return vol3x::fromTo(vmin, vmax);
	};

	/// Checks if a point is inside the volume.
	/// @param point Tested point.
	bool contains(const vec3x& point) const {
		return (pos.x < point.x && point.x < out().x)
			&& (pos.y < point.y && point.y < out().y)
			&& (pos.z < point.z && point.z < out().z);
	};

	/// Checks if 2 volumes intersect.
	/// @param oth Other volume.
	bool intersects(const vol3x& oth) const {
		vec3x dim = (*this & oth).dim;
		return dim.x >= (1fx / 64fx)
			&& dim.y >= (1fx / 64fx)
			&& dim.z >= (1fx / 64fx);
	};
};