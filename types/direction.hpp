#pragma once

/// 2D direction type.
enum Direction: uint8_t {
	East, West, North, South
};

/// Returns a character corresponding to a direction.
/// @param dir Direction type.
char directionChar(Direction dir) {
	static const char table[4] { 'E', 'W', 'N', 'S' };
	return (dir >= East && dir <= South) ? table[dir] : '?';
};

/// Returns a string representation of a direction.
/// @param dir Direction type.
/// @param lower Whether the output should be lowercase.
const char* directionName(Direction dir, bool lower = false) {
	static const char* const table[2][4] {
		{ "East", "West", "North", "South" },
		{ "east", "west", "north", "South" },
	};
	return (dir >= East && dir <= South) ? table[lower][dir] : "Unknown";
};

/// Converts a direction into a 2D vector.
/// @param dir Direction type.
/// @return Unit vector.
glm::vec2 directionVec(Direction dir) {
	switch (dir) {
		case East : return { -1, 0 };
		case West : return {  1, 0 };
		case North: return { 0, -1 };
		case South: return { 0,  1 };
	};
	return { 0, 0 };
};

/// Converts steering values into a direction.
/// @param dx X axis steering.
/// @param dy Y axis steering.
/// @note X axis will take priority if both directions are held.
/// @return If no steering is present, result is undefined.
Direction directionFromSteer(float dx, float dy) {
	if (dx < 0.0f) return East;
	if (dx > 0.0f) return West;
	if (dy < 0.0f) return North;
	if (dy > 0.0f) return South;
	return East;
};

/// Checks if a direction lies on a positive value of axis.
/// @param dir Direction type.
/// @return `0` if sign is negative, `1` if sign is positive.
bool directionSign(Direction dir) {
	return dir == West || dir == South;
};