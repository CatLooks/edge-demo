#pragma once

/// User interface namespace.
namespace UI {
	/// Button object.
	class Button : public Panel {
		public:
		/// Whether the button is selected.
		bool selected = false;
		/// Button color when not selected.
		glm::vec3 normal = glm::vec3(1.0f);

		/// Default constructor.
		/// @param label Button label text.
		/// @param font Button label font size.
		/// @param align Button label alignment.
		Button (const char* label, int font, Align align) {
			// add button label
			UI::Text* text = new UI::Text;
			switch (align) {
				case Left  : text->box.pos = { Dim(+16, 0.0f,  0.0f), Dim::pix(font) + 0.5ps - 0.5ws }; break;
				case Center: text->box.pos = { Dim(  0, 0.5f, -0.5f), Dim::pix(font) + 0.5ps - 0.5ws }; break;
				case Right : text->box.pos = { Dim(-16, 1.0f, -1.0f), Dim::pix(font) + 0.5ps - 0.5ws }; break;
				default    : break;
			};
			text->align = UI::Center;
			text->font = font;
			text->setText(label);
			add(text);

			// add selected color shader
			updateCall([&](Element&, float) {
				color = selected ? Rainbow::textFast(0) : normal;
			});
		};
	};
};