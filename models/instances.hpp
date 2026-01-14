#pragma once

/// Model generation namespace.
namespace Models {
	/// Model instances.
	namespace Instances {
		Object* hint      ; /// Hint shadow.
		Object* falling[4]; /// Falling platforms.
		Object* button [2]; /// Button states.
		Object* resizer[2]; /// Resizer (in & out).
		Object* helper    ; /// Helper object.
		Object* prism  [4]; /// Prism and its shadow.
		Object* moving [4]; /// Moving platform active highlight (full & half).
		Object* exit      ; /// Exit highlight.

		/// Generates all instance models.
		void generate() {
			// generate hint
			Hint::generate(*(hint = new Object));

			// generate falling platforms
			for (size_t i = 0; i < 4; i++)
				Falling::generate(*(falling[i] = new Object), static_cast<Theme>(i));

			// generate buttons
			for (size_t i = 0; i < 2; i++)
				Button::generate(*(button[i] = new Object), i != 0);

			// generate resizers
			for (size_t i = 0; i < 2; i++)
				Resizer::generate(*(resizer[i] = new Object));

			// generate helper
			Helper::generate(*(helper = new Object));

			// generate prisms
			Prism::generate(*(prism[0] = new Object), *(prism[1] = new Object));
			Prism::generate(*(prism[2] = new Object), *(prism[3] = new Object));

			// generate moving platform highlight
			for (size_t i = 0; i < 4; i++)
				Moving::highlight(*(moving[i] = new Object), (i & 1) != 0);
			for (size_t i = 2; i < 4; i++) {
				moving[i]->color = glm::vec3(0.0f);
				moving[i]->alpha = 1 / 32.0f;
			};

			// generate exit
			Models::Exit::generate(*(exit = new Object));
		};

		/// Deallocates all instance models.
		void unload() {
			delete hint;
			delete falling[0];
			delete falling[1];
			delete falling[2];
			delete falling[3];
			delete button[0];
			delete button[1];
			delete resizer[0];
			delete resizer[1];
			delete helper;
			delete prism[0];
			delete prism[1];
			delete prism[2];
			delete prism[3];
			delete moving[0];
			delete moving[1];
			delete moving[2];
			delete moving[3];
			delete exit;
		};

		/// Animates all dynamic instances.
		/// @param Elapsed time since last update.
		void animate(float delta) {
			// animate resizers
			Resizer::animateOut(*resizer[0]);
			Resizer::animateIn (*resizer[1]);

			// update helper color
			helper->color = Rainbow::helper();

			// rotate prisms
			prism[0]->rotZ += delta * 0.5f;
			prism[1]->rotZ += delta * 0.5f;
			prism[2]->rotZ += delta * 0.5f;
			prism[3]->rotZ += delta * 0.5f;

			// update rainbow prism color
			Prism::colorX(*prism[0], Rainbow::prismX());
			Prism::colorY(*prism[0], Rainbow::prismY());
			Prism::colorZ(*prism[0], Rainbow::prismZ());
			prism[0]->update();

			// update dark prism color
			Prism::colorX(*prism[2], Rainbow::darkPrismX());
			Prism::colorY(*prism[2], Rainbow::darkPrismY());
			Prism::colorZ(*prism[2], Rainbow::darkPrismZ());
			prism[2]->update();

			// update moving platform highlight
			moving[0]->color = Rainbow::moving();
			moving[1]->color = Rainbow::moving();

			// update exit colors
			Models::Exit::update(*exit);
		};
	};

	/// Draws a hint shadow.
	/// @param pos Hint position.
	/// @return Drawn vertex count.
	size_t drawHint(glm::vec3 pos) {
		Instances::hint->pos = pos;
		return Instances::hint->draw(Shaders::tex);
	};

	/// Draws a falling platform.
	/// @param pos Platform position.
	/// @param theme Platform model theme.
	/// @return Drawn vertex count.
	size_t drawFalling(glm::vec3 pos, Theme theme) {
		Instances::falling[theme]->pos = pos;
		return Instances::falling[theme]->draw(Shaders::tex);
	};

	/// Draws a button.
	/// @param pos Button position.
	/// @param pressed Whether the button is pressed.
	/// @return Drawn vertex count.
	size_t drawButton(glm::vec3 pos, bool pressed) {
		Instances::button[pressed]->pos = pos;
		return Instances::button[pressed]->draw(Shaders::tex);
	};

	/// Draws a resizer.
	/// @param pos Resizer position.
	/// @param shrink Whether the resizer shrinks the cube.
	/// @return Drawn vertex count.
	size_t drawResizer(glm::vec3 pos, bool shrink) {
		Instances::resizer[shrink]->pos = pos;
		return Instances::resizer[shrink]->draw(Shaders::tex);
	};

	/// Draws a helper hologram.
	/// @param pos Helper position.
	/// @return Drawn vertex count.
	size_t drawHelper(glm::vec3 pos) {
		Instances::helper->pos = pos;
		return Instances::helper->draw(Shaders::tex);
	};

	/// Draws a prism.
	/// @param pos Prism position.
	/// @param dark Whether the prism is dark.
	/// @return Drawn vertex count.
	size_t drawPrism(glm::vec3 pos, bool dark) {
		size_t idx = dark ? 2 : 0;
		Instances::prism[idx]->pos = pos;
		return Instances::prism[idx]->draw(Shaders::tex);
	};

	/// Draws a prism's shadow.
	/// @param pos Prism position.
	/// @param dark Whether the prism is dark.
	/// @return Drawn vertex count.
	size_t drawPrismShadow(glm::vec3 pos, bool dark) {
		size_t idx = dark ? 3 : 1;
		Instances::prism[idx]->pos = pos;
		return Instances::prism[idx]->draw(Shaders::tex);
	};

	/// Draws an active moving platform highlight.
	/// @param pos Highlight position.
	/// @param half Highlight height.
	/// @return Drawn vertex count.
	size_t drawActive(glm::vec3 pos, bool half) {
		Instances::moving[half]->pos = pos;
		return Instances::moving[half]->draw(Shaders::tex);
	};

	/// Draws an inactive moving platform highlight.
	/// @param pos Highlight position.
	/// @param half Highlight height.
	/// @return Drawn vertex count.
	size_t drawInactive(glm::vec3 pos, bool half) {
		Instances::moving[2 + half]->pos = pos;
		return Instances::moving[2 + half]->draw(Shaders::tex);
	};

	/// Draws an exit.
	/// @param pos Exit position.
	/// @return Drawn vertex count.
	size_t drawExit(glm::vec3 pos) {
		Instances::exit->pos = pos;
		return Instances::exit->draw(Shaders::tex);
	};
};