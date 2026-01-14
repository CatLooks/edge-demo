#pragma once

/// Assets namespace.
namespace Assets {
	/// Intermediate asset namespace.
	namespace Mid {
		/// Terrain sprites.
		sf::Image terrain;

		/// Platform sprites.
		sf::Image plat;

		/// Rainbow color sprite.
		sf::Image rainbow;

		/// Rainbow highlight sprite.
		sf::Image mask;

		/// Font sprite.
		sf::Image font;

		/// Ranks sprite.
		sf::Image rank;

		/// White sprite.
		sf::Image white;
	};

	/// Cube faces texture.
	Tex terrain;

	/// Platforms texture.
	Tex plat;

	/// Rainbow highlight mask texture.
	Tex mask;

	/// Font textures.
	Tex font;

	/// Rank textures.
	Tex rank;

	/// White texture.
	Tex white;

	/// Rainbow color array.
	glm::vec3 rainbow[6][256];

	/// Returns a pixel from the image.
	/// @param img Image object.
	/// @param pos Pixel position.
	/// @param size Assumed image size.
	/// @return Pixel color value.
	glm::vec3 pixel(const sf::Image& img, sf::Vector2u pos, sf::Vector2u size) {
		// get unit coordinates
		unsigned int x = pos.x * img.getSize().x / size.x;
		unsigned int y = pos.y * img.getSize().y / size.y;

		// return pixel value
		sf::Color color = img.getPixel(x, y);
		return glm::vec3(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f);
	};

	/// Loads all texture assets.
	void loadTex() {
		Log::out.separator("Textures");

		// image assets loading data
		struct imgdata_t {
			const char* path;
			sf::Image* img;
		};
		static const imgdata_t images[] {
			{ "textures/rainbow.png" , &Mid::rainbow },
			{ "textures/terrain.png" , &Mid::terrain },
			{ "textures/platform.png", &Mid::plat    },
			{ "textures/mask.png"    , &Mid::mask    },
			{ "textures/font.png"    , &Mid::font    },
			{ "textures/ranks.png"   , &Mid::rank    }
		};

		// generate white sprite
		Mid::white.create(1, 1, sf::Color::White);

		// load images
		for (const imgdata_t& data : images) {
			Log::out.symI();
			Log::out.format("Loading <%s>.\n", data.path);
			Log::out.print();
			if (!data.img->loadFromFile(data.path)) {
				Log::out.symE();
				Log::out.queue("Failed to load image.\n");
				Log::out.print();
			};
		};

		// load rainbow
		if (Mid::rainbow.getSize() == sf::Vector2u(256, 6)) {
			for (GLuint t = 0; t < 6; t++) {
				for (GLuint x = 0; x < 256; x++) {
					sf::Color color = Mid::rainbow.getPixel(x, t);
					rainbow[t][x] = glcolor(color.r, color.g, color.b);
				};
			};
		} else {
			Log::out.symE();
			Log::out.format("Rainbow sprite must be 256x6 pixels (read %dx%d).\n",
				Mid::rainbow.getSize().x, Mid::rainbow.getSize().y);
			Log::out.print();
		};

		// load textures
		terrain.set(Mid::terrain, GL_TEXTURE0, GL_LINEAR );
		plat   .set(Mid::plat   , GL_TEXTURE0, GL_LINEAR );
		mask   .set(Mid::mask   , GL_TEXTURE0, GL_LINEAR );
		font   .set(Mid::font   , GL_TEXTURE0, GL_NEAREST);
		rank   .set(Mid::rank   , GL_TEXTURE0, GL_NEAREST);
		white  .set(Mid::white  , GL_TEXTURE0, GL_NEAREST);
	};
};