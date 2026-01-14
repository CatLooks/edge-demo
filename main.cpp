#define _USE_MATH_DEFINES
#define lengthof(x) (sizeof(x) / sizeof(x[0]))

// include SFML & OpenGL
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <GL/glew.h>

// define 3d unsigned int vector
namespace sf { typedef Vector3<unsigned int> Vector3u; };

// include math libaries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

// include stdlib
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <math.h>

// include STL
#include <vector>
#include <deque>
#include <list>
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include <type_traits>

// include project
#include "types.hpp"
#include "files/file.hpp"
#include "logging.hpp"
#include "shaders.hpp"
#include "texture.hpp"
#include "assets.hpp"
#include "sound.hpp"
#include "music.hpp"
#include "timers.hpp"
#include "vertex.hpp"
#include "object.hpp"
#include "render.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "bytes/level.hpp"
#include "bytes/world.hpp"
#include "elements/level.hpp"
#include "ui.hpp"

/// Program entry.
/// @return Exit code.
int main(int argcount, const char** args) {
	// enable ANSI colors
	#ifdef _WIN32
	system("color");
	#endif

	// parse command line arguments
	for (int i = 1; i < argcount; i++) {
		if (strcmp(args[i], "-f") == 0)
			Win::fullscreen = true;
		if (strcmp(args[i], "-v") == 0)
			Win::vsync = true;
	};

	// open logging file
	FILE* logs = fopen("logs.txt", "w");
	Log::out.setStream(logs);
	Log::out.color = false;

	// attach signal handler
	signal(SIGINT  , Log::signal);
	signal(SIGILL  , Log::signal);
	signal(SIGFPE  , Log::signal);
	signal(SIGSEGV , Log::signal);
	signal(SIGTERM , Log::signal);
	signal(SIGABRT , Log::signal);

	// create window
	Win::create();

	// initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewStatus = glewInit();
	if (glewStatus != GLEW_OK) {
		Log::out.symF();
		Log::out.queue("GLEW initialization failed.\n");
		Log::out.cite((const char*)glewGetErrorString(glewStatus));
		Log::out.print();
		return -1;
	};

	// enable OpenGL logging
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(Log::errorGL, NULL);

	// log OpenGL version
	Log::out.separator("Info");
	Log::out.symI();
	Log::out.format("OpenGL version: %s\n", glGetString(GL_VERSION));
	Log::out.symI();
	Log::out.format("Shading version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	Log::out.symI();
	Log::out.format("Renderer: %s (%s)\n", glGetString(GL_RENDERER), glGetString(GL_VENDOR));
	Log::out.print();

	// set OpenGL settings
	Win::init();

	// load assets
	Assets::loadTex();
	Assets::loadSfx();
	Models::Moving::loadColors();

	// load shaders
	Shaders::load();

	// generate model instances
	Models::Instances::generate();

	// generate background object
	Models::Space::generate();

	// bind texture 0 for rendering
	Tex::useSlot(Shaders::tex, "tex", 0);

	// start loading files
	Log::out.separator("Loading data");
	Log::out.print();

	// load route data
	int status = 0;
	FileReader file = FileReader::open("levels/routes.dat", &status);
	if (status) {
		Log::out.symF();
		Log::out.format("Could not open <levels/routes.dat>.\n");
		Log::out.print();
		return 0xbad;
	};

	// parse route data
	Bytes::Routes routedata;
	try {
		routedata.read(file);
	} catch (EOFError err) {
		Log::out.symF();
		Log::out.format("Route data is corrupted.\n");
		Log::out.print();
		return 0xbad;
	};

	// UI system element
	UI::System* state = new UI::System(routedata);
	state->loadSigns();
	state->updateStats();

	// load world map
	state->loadWorld(state->world.load("world"));

	// prepare for main loop
	Log::out.separator();
	Log::out.print();
	Timers::clock.restart();

	// main loop
	while (app.isOpen()) {
		// update UI hover
		state->mouse(sf::Mouse::getPosition(app));

		// handle events
		sf::Event evt;
		while (app.pollEvent(evt)) {
			state->event(evt);
			Debug::event(evt);
			Win::event(evt);
		};

		// update state
		float delta = Win::update(state->stoptimers());
		state->update(delta);
		Music::update(delta);

		// draw game
		state->draw();

		// calculate FPS
		Debug::updateFPS();

		// swap window buffers
		app.display();
		Debug::updateFrame();
	};

	// unload GUI
	delete state;

	// unload background
	Models::Space::unload();

	// unload model instances
	Models::Instances::unload();

	// unload particle model
	Models::Particle::unload();

	// unload sprite instance
	UI::unload();

	// unload shaders
	Shaders::unload();

	// success
	return 0;
};