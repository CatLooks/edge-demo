#pragma once

/// User interface namespace.
namespace UI {
	/// import texture map
	using Textures::TexMap;

	/// Amount of characters across the X-axis.
	const float countX = 1.0f / 16;
	/// Amount of characters across the Y-axis.
	const float countY = 1.0f / 16;

	/// Text renderer namespace.
	namespace TextDraw {
		/// Glyph width table.
		const unsigned int glyphWidth[128] {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 6, 4, 5, 0,
			3, 3, 3, 2, 5, 9, 7, 2, 3, 3, 3, 5, 2, 5, 1, 4,
			4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 2, 4, 5, 4, 4,
			5, 6, 5, 5, 5, 4, 4, 5, 5, 1, 4, 5, 4, 7, 5, 5,
			5, 5, 5, 5, 5, 5, 6, 7, 5, 5, 4, 0, 8, 0, 5, 0,
			7, 5, 4, 4, 4, 4, 3, 4, 4, 1, 2, 4, 1, 7, 4, 4,
			4, 4, 3, 4, 3, 4, 5, 7, 4, 4, 4, 2, 3, 2, 6, 0,
		};

		/// Returns texture map for an ASCII character.
		/// @param c ASCII character.
		TexMap ascii(unsigned char c) {
			glm::vec2 orig = { (c & 15) * countX, (c >> 4) * countY };
			glm::vec2 dest = orig + glm::vec2(countX, countY);
			return TexMap(orig, dest);
		};

		/// Sets glyph sprite color.
		/// @param color Text color.
		void setColor(glm::vec3 color) { sprite().color = color; };

		/// Sets glyph sprite opacity.
		/// @param color Text opacity.
		void setAlpha(float alpha) { sprite().alpha = alpha; };

		/// Sets glyph sprite size.
		/// @param size Text size.
		void setSize(unsigned int size) { sprite().size = sf::Vector2i(16 * size, 16 * size); };

		/// Sets glyph sprite style.
		/// @param color Text color.
		/// @param color Text opacity.
		/// @param size Text size.
		void setStyle(glm::vec3 color, float alpha, unsigned int size) {
			setColor(color);
			setAlpha(alpha);
			setSize(size);
		};

		/// Draws a glyph.
		/// @param c Drawn character.
		/// @param pos Character position.
		/// @param bold Whether the character is outlined.
		void drawGlyph(char c, sf::Vector2i pos, bool bold) {
			sprite().pos = pos;
			sprite().set(ascii(c + (bold ? 0x80 : 0x00)));
			sprite().draw();
		};
	};

	/// Text label element.
	class Text : public Element {
		public:
		/// Text color.
		glm::vec3 color = glm::vec3(1.0f);
		/// Text alpha.
		float alpha = 1.0f;
		/// Text font size.
		unsigned int font = 1;
		/// Text alignment.
		Align align = Align::Left;

		/// Sets label text.
		/// @param str Text string.
		/// @note This function modifies element size.
		void setText(const char* str) {
			text.clear();
			lines.clear();

			// check for empty string
			if (str == NULL || str[0] == '\0') {
				box = DimBox();
				return;
			};
			text = std::string(str);

			// calculate text bounds
			box.size = Dim2(0px, Dim(16 * font, 0.0f, 0.0f));
			int& width = box.size.x.off;
			int& height = box.size.y.off;

			// text style tracker
			int cursor = 0;
			for (size_t i = 0; true; i++) {
				// check for newline
				if (str[i] == '\n' || str[i] == '\x1f' || str[i] == '\0') {
					// add bold padding
					cursor += font;

					// store line width
					lines.push_back(cursor);

					// update bounds
					width = cursor >= width ? cursor : width;
					switch (str[i]) {
						case '\n'  : height += 13 * font; break;
						case '\x1f': height += 9  * font; break;
						case '\0'  : return;
					};

					// reset cursor position
					cursor = 0;
					continue;
				};

				// advance cursor position
				unsigned int adv = str[i] >= 0x80 ? 0 : TextDraw::glyphWidth[(uint8_t)str[i]];
				if (adv == 0) continue;
				cursor += (adv + 1) * font;
			};
		};

		/// Sets label text.
		/// @param str Label text format string.
		/// @param args Format arguments.
		template <typename... A> void setFmt(const char* str, A... args) {
			char* result = formatf(str, args...);
			setText(result);
			delete result;
		};

		protected:
		/// Label text data.
		std::string text;
		/// Line length list.
		std::vector<size_t> lines;

		/// Draws the text.
		/// @param self Bounding box.
		void drawSelf(sf::IntRect self) const override {
			// text renderer state
			sf::Vector2i cursor;
			bool bold = false;
			bool rainbow = false;
			bool dynamic = false;
			bool fast = false;
			bool red = false;
			bool dim = false;
			bool newline = true;
			uint8_t rainoff = 0;
			glm::vec3 last = glm::vec3(1.0f);

			// set default style
			sf::Vector2i origin = { self.left, self.top };
			TextDraw::setStyle(last, alpha, font);

			// process each character
			Assets::font.bind();
			size_t lineno = 0;
			for (char c : text) {
				// prepare a newline
				if (newline) {
					switch (align) {
						case Left:
							cursor.x = 0;
							break;
						case Right:
							cursor.x = self.width - lines[lineno];
							break;
						case Center:
							cursor.x = (self.width - lines[lineno]) / 2;
							break;
					};
					newline = false;
				};

				// check for a control character
				switch (c) {
					// create new line
					case '\n':
						newline = true;
						lineno++;
						cursor.y += 13 * font;
						continue;

					// create short new line
					case '\x1f':
						newline = true;
						lineno++;
						cursor.y += 9 * font;
						continue;

					// reset text style
					case '\1':
						bold = false;
						rainbow = false;
						dim = false;
						TextDraw::setColor(last = glm::vec3(1.0f));
						TextDraw::setAlpha(alpha);
						continue;

					// enable text outline
					case '\2': bold = true; continue;
					// disable text outline
					case '\3': bold = false; continue;
					// dim text
					case '\4': TextDraw::setColor(last * 0.5f); continue;
					// enable static rainbow color
					case '\5': rainbow = true; dynamic = false; continue;
					// enable dynamic rainbow color
					case '\6': rainbow = true; dynamic = true; continue;
					// disable gradient color
					case '\7': rainbow = false; TextDraw::setColor(last = glm::vec3(1.0f)); continue;
					// restore previous color
					case '\r': TextDraw::setColor(last); continue;
					// normal rainbow mode
					case '\x16': red = false; continue;
					// red rainbow mode
					case '\x17': red = true; continue;
					// slow rainbow mode
					case '\x18': fast = false; continue;
					// fast rainbow mode
					case '\x19': fast = true; continue;

					// set text color
					case '\x10': TextDraw::setColor(last = RGB(0x7D7D7D)); continue;
					case '\x11': TextDraw::setColor(last = RGB(0x000000)); continue;
					case '\x12': TextDraw::setColor(last = RGB(0xFFFFFF)); continue;
					case '\x13': TextDraw::setColor(last = RGB(0x424242)); continue;
					case '\x14': TextDraw::setColor(last = RGB(0xC0C0C0)); continue;
				};

				// update gradient color
				if (rainbow) {
					if (red) {
						TextDraw::setColor(last = RGB(0xFF2864) * Rainbow::glint(rainoff));
						rainoff += 13;
					} else {
						TextDraw::setColor(last = fast ? Rainbow::textFast(rainoff) : Rainbow::text(rainoff));
						if (dynamic) rainoff += fast ? 5 : 21;
					};
				};

				// dim character
				if (dim) TextDraw::setColor(last * 0.5f);

				// draw printable character
				TextDraw::drawGlyph(c, origin + cursor, bold);
				cursor.x += (TextDraw::glyphWidth[(uint8_t)c] + 1) * font;

				// restore color
				if (dim) { TextDraw::setColor(last); dim = false; };
			};
		};
	};
};