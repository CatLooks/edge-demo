#pragma once

/// User interface namespace.
namespace UI {
	/// Generates a prism list.
	/// @param str Output string (max 24 chars).
	/// @param max Total prism count.
	/// @param left Uncollected prism count.
	/// @param midsplit Whether to split the prisms equally.
	void genPrismList(char* str, size_t max, size_t left, bool midsplit) {
		size_t split = midsplit ? (max + 1) / 2 : 10;
		if (midsplit && max <= 10) split = 10;
		for (size_t i = 0; i < max; i++) {
			if (i == max - left) {
				*str++ = '\7';
				*str++ = '\x10';
			};
			if (i == split) {
				*str++ = '\x1f';
			};
			*str++ = '\\';
		};
		*str = '\0';
	};

	/// Returns a camera offset for menu display.
	/// @param camera Target camera.
	/// @param view Shift amount.
	glm::vec3 cameraOffset(const Camera& camera, float view) {
		// get block shift magnitude
		float mag = glm::length(camera.off) * tanf(glm::radians(camera.fov) * 0.5f);

		// get offset coordinates
		return glm::vec3(-1.0f, 0.0f, 1.0f) * mag * view;
	};

	/// Pause menu element.
	class Pause : public Panel {
		public:
		Element* m_base;
		Text* m_info;
		Menu* m_menu;

		/// Level object pointer.
		Level::Level* level;
		/// Level number.
		uint16_t no;

		/// Pause menu toggle request.
		bool toggle = false;
		/// World map exit request.
		bool world = false;

		/// Pause menu constructor.
		/// @param level Level reference.
		/// @param id Level ID.
		Pause (Level::Level* level, uint16_t id): level(level), no(id) {
			// setup base panel
			box = { -1ws, 0px, 0px, 1ps };
			color = RGB(0x3B3B3B);
			active = false;

			// create menu panel
			m_base = new UI::Element;
			m_base->box = { 0px, -1ws, 1ps, 1ps };
			add(m_base);
			{
				// add level info panel
				m_info = new UI::Text;
				m_info->box.pos = { 16px, 8px };
				m_info->font = 4;
				m_info->setText("\\\\\\\\\\\\\\\\\\\\");
				m_info->updateCall([=](UI::Element& elem, float delta) {
					// check if no level is loaded
					if (level == NULL) { m_info->setText(""); return; };

					// generate prism array
					char prisms[32];
					genPrismList(prisms, level->root.prisms.size(), level->state.prisms.size(), false);

					// update info text
					m_info->setFmt("\x19\6%d | %s\n%s", id, level->root.name.c_str(), prisms);
				});
				m_base->add(m_info);

				// update base panel width
				box.size.x = Dim::pix(m_info->box.size.x.off + m_info->box.pos.x.off * 2);
			};

			// create button container
			m_menu = new Menu;
			m_menu->attachBelow(m_info, 0px);
			{
				UI::Button* button;

				// add resume button
				button = m_menu->addButton("\x10" "Resume");
				button->eventCall(Element::MouseDown, [=](UI::Element&, const EventData&) -> bool {
					// play press sound
					Assets::sfx(Assets::Ok);
					toggle = true;
					return true;
				});

				// add last checkpoint button
				button = m_menu->addButton("\x10" "Checkpoint");
				button->eventCall(Element::MouseDown, [=](UI::Element&, const EventData&) -> bool {
					// trigger checkpoint
					if (level) level->restore(false);
					toggle = true;

					// play press sound
					Assets::sfx(level ? Assets::Ok : Assets::Cancel);
					return true;
				});

				// add retry button
				button = m_menu->addButton("\x10" "Retry");
				button->eventCall(Element::MouseDown, [=](UI::Element&, const EventData&) -> bool {
					// trigger retry
					if (level) level->retry();
					toggle = true;

					// play press sound
					Assets::sfx(level ? Assets::Start : Assets::Cancel);
					return true;
				});

				// add exit button
				button = m_menu->addButton("\x10" "Exit");
				button->eventCall(Element::MouseDown, [=](UI::Element&, const EventData&) -> bool {
					// exit to world map
					toggle = true;
					world = true;

					// play press sound
					Assets::sfx(Assets::Ok);
					return true;
				});
			};
			m_base->add(m_menu);
		};

		/// Returns a camera offset for menu display.
		/// @param camera Target camera.
		glm::vec3 cameraOffset(const Camera& camera) {
			return UI::cameraOffset(camera, box.pos.x.par + (float)box.size.x.off / Win::size.x);
		};
	};
};