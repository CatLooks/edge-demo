#pragma once

/// User interface namespace.
namespace UI {
	/// Level completion element.
	class Complete : public Panel {
		public:
		Element* m_base;
		RankImage* m_img;

		/// Request enumeration.
		enum Request {
			None = 0,
			Next,
			Retry,
			Exit
		};
		Request req = None;

		/// Menu constructor.
		/// @param data Display data.
		/// @param name Level name.
		Complete (const Data::LevelData& data, const char* name) {
			// setup base panel
			box = { -1ws, 0px, 400px, 1ps };
			color = RGB(0x3B3B3B);
			active = false;

			// create menu panel
			m_base = new Element;
			m_base->box = { 0px, -1ps, 1ps, 1ps };
			add(m_base);
			{
				// create level label
				UI::Text* info = new Text;
				info->box.pos = { 16px, 8px };
				info->font = 4;
				info->setFmt("\x19\6%d | %s", data.no, name);
				m_base->add(info);

				// create time labels
				UI::Panel* panel;

				// panel generator function
				Dim y = info->box.size.y + info->box.pos.y * 2;
				auto setup = [&](Panel*& panel) -> Text* {
					// setup panel
					panel = new Panel;
					panel->box = { 8px, y, 1ps - 16px, 56px };
					panel->border = RGB(0x3B3B3B);
					panel->thickness = 1;
					m_base->add(panel);
					y = y + panel->box.size.y;

					// setup panel text
					UI::Text* text = new Text;
					text->font = 3;
					text->box.pos = { 8px, 0.5ps - 0.5ws + Dim::pix(text->font) };
					panel->add(text);
					return text;
				};

				// create game time label
				char* timerepr = Data::Time::repr(data.time.game);
				setup(panel)->setFmt("\x13" "Game Time: %s", timerepr);
				delete timerepr;

				// create edge time label
				timerepr = Data::Time::repr(data.time.edge);
				setup(panel)->setFmt("\x13" "Edge Time: \x18\6%s", timerepr);
				delete timerepr;

				// create total time label
				timerepr = Data::Time::repr(data.time.total());
				setup(panel)->setFmt("\2Total Time: %s", timerepr);
				delete timerepr;
				panel->rainbow = true;

				// create ranking panel
				panel = new Panel;
				panel->box = { 8px, y, 1ps - 16px, 192px };
				panel->border = RGB(0x1C1D1F);
				panel->color = RGB(0xD2D2D2);
				panel->thickness = 6;
				m_base->add(panel);
				{
					// add rank label
					UI::Panel* rankText = new Panel;
					rankText->border = RGB(0x1C1D1F);
					rankText->color = RGB(0x1C1D1F);
					rankText->thickness = 6;
					panel->add(rankText);
					{
						UI::Text* text = new Text;
						text->box.pos = { 16px, 4px };
						text->font = 3;
						text->setText("Rank");
						rankText->add(text);
						rankText->box.size = text->box.size + text->box.pos * 2 - Dim2(0px, Dim::pix(text->font));
					};

					// add rank image
					UI::Panel* rankPanel = new Panel;
					rankPanel->box = { 1ps - 1ws - 32px, 0.5ps - 0.5ws, 160px, 160px };
					rankPanel->thickness = 2;
					panel->add(rankPanel);
					rankPanel->add(m_img = new RankImage(data.rank, 128));
				};
				y = y + panel->box.size.y;

				// create death count label
				setup(panel)->setFmt("\x13" "Deaths: \1\2%llu", data.deaths);

				// create prism count label
				setup(panel)->setFmt("\x13" "Prisms: \1\2\6%llu\7/\6%llu\7", data.pcol, data.pttl);

				// create menu separator
				panel = new Panel;
				panel->box = { 0px, y + 12px, 1ps, 6px };
				panel->color = RGB(0x1C1D1F);
				m_base->add(panel);
				y = y + (panel->box.pos.y - y) * 2 + panel->box.size.y;

				// create level complete menu
				UI::Menu* menu = new Menu;
				menu->box = { 8px, y, 1ps - 16px, 1ps - y };
				{
					UI::Button* button;

					// add next level button
					button = menu->addButton("\x10" "Next");
					button->eventCall(MouseDown, [=](Element&, const EventData&) -> bool {
						req = Next;

						// play sound effect
						Assets::sfx(Assets::Start);
						return true;
					});

					// add retry level button
					button = menu->addButton("\x10" "Retry");
					button->eventCall(MouseDown, [=](Element&, const EventData&) -> bool {
						req = Retry;

						// play sound effect
						Assets::sfx(Assets::Start);
						return true;
					});

					// add main menu button
					button = menu->addButton("\x10" "Main menu");
					button->eventCall(MouseDown, [=](Element&, const EventData&) -> bool {
						req = Exit;

						// play sound effect
						Assets::sfx(Assets::Ok);
						return true;
					});
				};
				m_base->add(menu);
			};
		};

		/// Returns a camera offset for level complete menu.
		/// @param camera Target camera.
		glm::vec3 cameraOffset(const Camera& camera) {
			return UI::cameraOffset(camera, box.pos.x.par + (float)box.size.x.off / Win::size.x);
		};
	};
};