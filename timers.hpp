#pragma once

/// Rainbow timer class.
class Timer {
	private:
	uint64_t m_col; /// Total microseconds.
	uint64_t m_one; /// Microseconds to advance 1 unit.
	uint8_t  m_val; /// Timer value.

	public:
	/// Default constructor.
	/// @param tick Amount of microseconds for 1 unit change.
	/// @param offset Initial value offset.
	Timer (uint64_t tick, uint8_t offset): m_col(0), m_one(tick), m_val(offset) {};

	/// Sets timer tick time.
	/// @param tick Amount of microseconds for 1 unit change.
	/// @return Self-reference.
	Timer& set(uint64_t tick) {
		m_one = tick;
		return *this;
	};

	/// Updates timer state.
	/// @param us Elapsed microseconds.
	void update(uint64_t us) {
		// update collector
		m_col += us;

		// calculate advancements
		uint64_t a_col = m_col % m_one;
		uint64_t a_val = m_col / m_one;

		// update state
		m_col = a_col;
		m_val += a_val;
	};

	/// Returns timer value.
	/// @param offset Value offset.
	uint8_t value(uint8_t offset = 0) const { return m_val + offset; };
};

/// Timer namespace.
namespace Timers {
	/// Timer controller clock.
	sf::Clock clock;

	/// Slow text rainbow timer.
	Timer textSlow(11000, 0x10);

	/// Fast text rainbow timer.
	Timer textFast(6500, 0xC4);

	/// Slow speed rainbow timer.
	Timer slow(16000, 0x00);

	/// Medium speed rainbow timer.
	Timer mid(6000, 0x44);

	/// Fast speed rainbow timer.
	Timer fast(1000, 0xA0);

	/// Cube rainbow timer.
	Timer cube(10000, 0x00);

	/// Darkcube rainbow timer.
	Timer darkcube(10000, 0x80);

	/// Update all timers.
	/// @param uiOnly Whether to update only UI timers.
	float update(bool uiOnly) {
		// reset clock
		sf::Time time = clock.restart();
		uint64_t us = time.asMicroseconds();

		// update timers
		textSlow.update(us);
		textFast.update(us);
		if (!uiOnly) {
			slow.update(us);
			mid.update(us);
			fast.update(us);
			cube.update(us);
			darkcube.update(us);
		};

		// return elapsed time
		return time.asSeconds();
	};
};

/// Rainbow color processing namespace.
namespace Rainbow {
	/// Returns rainbow dust color.
	/// @param off Color offset.
	/// @param mode Color mode.
	glm::vec3 dust(uint8_t off, uint8_t mode) { return Assets::rainbow[mode][Timers::fast.value(off)]; };

	/// Returns rainbow GUI color.
	/// @param off Color offset.
	/// @param mode Color mode.
	glm::vec3 gui(uint8_t off, uint8_t mode) { return Assets::rainbow[mode][Timers::slow.value(off)]; };

	/// Returns rainbow text color.
	/// @param off Color offset.
	glm::vec3 text(uint8_t off) { return Assets::rainbow[1][Timers::textSlow.value(off)]; };
	/// Returns fast rainbow text color.
	/// @param off Color offset.
	glm::vec3 textFast(uint8_t off) { return Assets::rainbow[2][Timers::textFast.value(off)]; };

	/// Returns a resizer color.
	glm::vec3 resizerA() { return Assets::rainbow[1][Timers::mid.value(0x00)]; };
	/// Returns an alt resizer color.
	glm::vec3 resizerB() { return Assets::rainbow[1][Timers::mid.value(0xF8)]; };

	/// Returns a helper color.
	glm::vec3 helper() { return Assets::rainbow[2][Timers::slow.value(0x60)]; };

	/// Returns an exit color.
	glm::vec3 exitA() { return Assets::rainbow[2][Timers::slow.value(0x40)]; };
	/// Returns an alt exit color.
	glm::vec3 exitB() { return Assets::rainbow[2][Timers::slow.value(0xC0)]; };

	/// Returns moving platform highlight color.
	glm::vec3 moving() { return Assets::rainbow[0][Timers::fast.value(0x80)]; };

	/// Returns prism dark side face color.
	glm::vec3 prismX() { return Assets::rainbow[1][Timers::fast.value()]; };
	/// Returns prism top face color.
	glm::vec3 prismY() { return Assets::rainbow[2][Timers::fast.value()]; };
	/// Returns prism light side face color.
	glm::vec3 prismZ() { return Assets::rainbow[0][Timers::fast.value()]; };

	/// Returns current cube dark side face color.
	glm::vec3 cubeX(bool dynamic) { return Assets::rainbow[1][(dynamic ? Timers::cube : Timers::darkcube).value()]; };
	/// Returns current cube top face color.
	glm::vec3 cubeY(bool dynamic) { return Assets::rainbow[2][(dynamic ? Timers::cube : Timers::darkcube).value()]; };
	/// Returns current cube light side face color.
	glm::vec3 cubeZ(bool dynamic) { return Assets::rainbow[0][(dynamic ? Timers::cube : Timers::darkcube).value()]; };

	/// Returns gradient from dark side face to top face color.
	/// @param t Gradient progress.
	glm::vec3 gradXY(bool dynamic, float t) { return lerpv(cubeX(dynamic), cubeY(dynamic), t); };
	/// Returns gradient from top face to dark side face color.
	/// @param t Gradient progress.
	glm::vec3 gradYX(bool dynamic, float t) { return lerpv(cubeY(dynamic), cubeX(dynamic), t); };
	/// Returns gradient from light side face to top face color.
	/// @param t Gradient progress.
	glm::vec3 gradZY(bool dynamic, float t) { return lerpv(cubeZ(dynamic), cubeY(dynamic), t); };
	/// Returns gradient from top face to light side face color.
	/// @param t Gradient progress.
	glm::vec3 gradYZ(bool dynamic, float t) { return lerpv(cubeY(dynamic), cubeZ(dynamic), t); };

	/// Returns dark prism dark side face color.
	glm::vec3 darkPrismX() { return Assets::rainbow[4][Timers::fast.value()]; };
	/// Returns dark prism top face color.
	glm::vec3 darkPrismY() { return Assets::rainbow[5][Timers::fast.value()]; };
	/// Returns dark prism light side face color.
	glm::vec3 darkPrismZ() { return Assets::rainbow[3][Timers::fast.value()]; };

	/// Returns dark gradient from dark side face to top face color.
	/// @param t Gradient progress.
	glm::vec3 darkGradXY(float t) { return lerpv(darkPrismX(), darkPrismY(), t); };
	/// Returns dark gradient from top face to dark side face color.
	/// @param t Gradient progress.
	glm::vec3 darkGradYX(float t) { return lerpv(darkPrismY(), darkPrismX(), t); };
	/// Returns dark gradient from light side face to top face color.
	/// @param t Gradient progress.
	glm::vec3 darkGradZY(float t) { return lerpv(darkPrismZ(), darkPrismY(), t); };
	/// Returns dark gradient from top face to light side face color.
	/// @param t Gradient progress.
	glm::vec3 darkGradYZ(float t) { return lerpv(darkPrismY(), darkPrismZ(), t); };

	/// Returns color glint.
	/// @param off Rainbow offset.
	float glint(uint8_t off) {
		float t = 10.0f / 256.0f * Timers::textSlow.value(off) - 3.0f;
		return 1.0f + 0.5f * expf(-t * t);
	};
};