#pragma once

/// User interface namespace.
namespace UI {
	/// Screen element.
	class Screen : public Element {
		public:
		/// Default constructor.
		Screen () {
			box = {0px, 0px, 1ps, 1ps};
		};

		/// Updates hover field for all elements.
		/// @param pos Mouse position.
		void mouse(sf::Vector2i pos) {
			Element::mouse(rect(), pos);
		};

		/// Processes an event on the screen.
		/// @param evt Event object.
		/// @return Whether the event was processed.
		bool event(const sf::Event& evt) {
			// check for mouse press
			if (evt.type == sf::Event::MouseButtonPressed && evt.mouseButton.button == sf::Mouse::Left)
				return Element::event(rect(), UI::Element::MouseDown, { {evt.mouseButton.x, evt.mouseButton.y}, sf::Keyboard::Unknown });

			// check for mouse release
			if (evt.type == sf::Event::MouseButtonReleased && evt.mouseButton.button == sf::Mouse::Left)
				return Element::event(rect(), UI::Element::MouseUp, { {evt.mouseButton.x, evt.mouseButton.y}, sf::Keyboard::Unknown });

			// check for key press
			if (evt.type == sf::Event::KeyPressed)
				return Element::event(rect(), UI::Element::KeyDown, { sf::Vector2i(), evt.key.code });

			// check for key release
			if (evt.type == sf::Event::KeyReleased)
				return Element::event(rect(), UI::Element::KeyUp, { sf::Vector2i(), evt.key.code });

			// unrelated event
			return false;
		};

		/// Draws the screen UI.
		void draw() const {
			drawSelf(rect());
			drawChildren(rect());
		};

		/// Returns screen bounding box.
		sf::IntRect rect() const { return sf::IntRect(0, 0, Win::size.x, Win::size.y); };
	};
};