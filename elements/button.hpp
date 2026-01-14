#pragma once

/// Level related namespace.
namespace Level {
	/// Button object.
	struct Button {
		/// Button data.
		const Bytes::Button* root;

		/// Button state.
		bool state;
		/// Whether the button is under the cube.
		bool pressed;

		/// Structure constructor.
		/// @param data Button data.
		Button (const Bytes::Button* data): root(data), state(data->state), pressed(false) {};

		/// Draws the button.
		/// @param plats Moving platform object list.
		/// @return Drawn vertex count.
		size_t draw(const std::vector<Moving>& plats) const {
			if (!root->visible) return 0;
			Assets::plat.bind();
			return Models::drawButton(volume(plats).pos.gl(), state);
		};

		/// Updates the button state.
		void update() {
			if (!pressed && root->stayup)
				state = false;
		};

		/// Returns button volume.
		/// @param plats Moving platform object list.
		vol3x volume(const std::vector<Moving>& plats) const {
			return vol3x(
				root->attached
					? plats[root->moving].now + vec3x(0, 0, 1)
					: root->pos,
			vec3x(1, 1, 0.25fx));
		};

		/// Executes button commands.
		/// @param movingPlats Moving platform list.
		/// @param buttons Button list.
		/// @param bumpers Bumper list.
		void execute(std::vector<Moving>& movingPlats, std::vector<Button>& buttons, std::vector<Bumper>& bumpers) {
			for (const Bytes::MovingCommand cmd : root->movings)
				movingPlats[cmd.id].move(cmd.way);
			for (const Bytes::ButtonCommand cmd : root->buttons)
				buttons[cmd.id].state = cmd.state;
			for (const Bytes::BumperCommand cmd : root->bumpers)
				bumpers[cmd.id].trigger(cmd.act);
		};

		/// Executes and immediately procs button commands.
		/// @param movingPlats Moving platform list.
		/// @param buttons Button list.
		/// @param bumpers Bumper list.
		void proc(std::vector<Moving>& movingPlats, std::vector<Button>& buttons, std::vector<Bumper>& bumpers) {
			for (const Bytes::MovingCommand cmd : root->movings)
				movingPlats[cmd.id].proc(cmd.way);
			for (const Bytes::ButtonCommand cmd : root->buttons)
				buttons[cmd.id].state = cmd.state;
			for (const Bytes::BumperCommand cmd : root->bumpers)
				bumpers[cmd.id].trigger(cmd.act);
		};
	};
};