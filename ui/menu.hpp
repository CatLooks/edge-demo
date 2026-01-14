#pragma once

/// User interface namespace.
namespace UI {
	/// Button menu element.
	class Menu : public Element {
		protected:
		/// Previously selected button index.
		size_t selectPrev = 0;
		/// Button list.
		std::vector<Button*> buttons;

		public:
		/// Selected button index.
		size_t selected = 0;

		/// Button menu constructor.
		Menu () {
			box = { 8px, 0px, 1ps - 16px, 1ps };
			updateCall([&](Element&, float) {
				// check for no selection changes
				if (selected == selectPrev)
					return;

				// unselect previous button
				buttons[selectPrev]->selected = false;

				// select new button
				buttons[selected]->selected = true;

				// update selection state
				selectPrev = selected;
			});
			eventCall(KeyDown, [&](Element&, const EventData& data) {
				// select button below
				if (data.key == sf::Keyboard::Down) {
					if (++selected >= buttons.size()) selected = 0;
					Assets::sfx(Assets::Select);
					return true;
				};

				// select button below
				if (data.key == sf::Keyboard::Up) {
					if (--selected >= buttons.size()) selected = buttons.size() - 1;
					Assets::sfx(Assets::Select);
					return true;
				};

				// press button
				if (data.key == sf::Keyboard::Enter) {
					buttons[selected]->forceEvent(MouseDown, EventData());
					return true;
				};

				// unknown event
				return false;
			});
		};

		/// Adds a button to menu.
		/// @param label Button label.
		/// @return Button pointer.
		Button* addButton(const char* label) {
			// create a new button
			Button* button = new Button(label, 4, Left);
			button->border = RGB(0x3B3B3B);
			button->thickness = 1;

			// set button position
			if ((button->selected = children.empty()))
				button->box = { 0px, 0px, 1ps, 64px };
			else
				button->box = { 0px, 64px + children.back()->box.pos.y, 1ps, 64px };

			// bind selection event
			size_t idx = children.size();
			button->eventCall(MouseDown, [=](Element&, const EventData&) -> bool {
				selected = idx;
				return false;
			});

			// add button to children list
			buttons.push_back(button);
			add(button);
			return button;
		};
	};
};