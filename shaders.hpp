/// Shader object.
class Shader {
	public:
	GLuint ID;

	/// Loads shader for further operations.
	void use() const {
		static GLuint last = -1;
		if (ID != last);
			glUseProgram(last = ID);
	};

	/// Unloads shader.
	void unload() { glDeleteProgram(ID); ID = 0; };

	/// Compiles shader code.
	/// @param source Shader source code.
	/// @param type Shader type.
	/// @return Shader ID.
	static GLuint compile(const char* source, char type) {
		// data pointer
		const char* ptr = source;

		// create shader unit
		GLuint shader = glCreateShader(type == 'v' ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
		glShaderSource(shader, 1, &ptr, NULL);

		// compile code
		glCompileShader(shader); report(shader, type);
		return shader;
	};

	/// Creates a shader program.
	/// @param vert Vertex shader ID.
	/// @param frag Fragment shader ID.
	/// @return Shader object.
	static Shader create(GLuint vert, GLuint frag) {
		// create shader program
		GLuint prog = glCreateProgram();
		if (vert) glAttachShader(prog, vert);
		if (frag) glAttachShader(prog, frag);
		glLinkProgram(prog);
		report(prog, 'p');

		// return shader program object
		return Shader { prog };
	};

	private:
	/// Reports shader status.
	/// @param shader Shader ID.
	/// @param type Shader type.
	/// @note Types:
	/// - `'v'` - vertex shader.
	/// - `'f'` - fragment shader.
	/// - `'p'` - program shader.
	static void report(GLuint shader, char type) {
		const size_t len = 0x10000;
		char log[len];
		GLint status;
		GLsizei outlen;

		// perform check
		switch (type) {
			case 'v':
				glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
				glGetShaderInfoLog(shader, len, &outlen, log);
				if (outlen == 0) break;
				if (status == GL_FALSE) {
					Log::out.symE();
					Log::out.queue("Failed vertex shader compilation:\n");
				} else {
					Log::out.symW();
					Log::out.queue("Vertex shader compilation info:\n");
				};
				Log::out.cite(log);
				Log::out.print();
			break;
			case 'f':
				glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
				glGetShaderInfoLog(shader, len, &outlen, log);
				if (outlen == 0) break;
				if (status == GL_FALSE) {
					Log::out.symE();
					Log::out.queue("Failed fragment shader compilation:\n");
				} else {
					Log::out.symW();
					Log::out.queue("Fragment shader compilation info:\n");
				};
				Log::out.cite(log);
				Log::out.print();
			break;
			case 'p':
				glGetProgramiv(shader, GL_LINK_STATUS, &status);
				glGetProgramInfoLog(shader, len, &outlen, log);
				if (outlen == 0) break;
				if (status == GL_FALSE) {
					Log::out.symE();
					Log::out.queue("Failed shader linking:\n");
				} else {
					Log::out.symW();
					Log::out.queue("Shader linking info:\n");
				};
				Log::out.cite(log);
				Log::out.print();
			break;
			default: break;
		};
	};
};

/// Shader program collection.
namespace Shaders {
	/// Texture shader.
	Shader tex;
	/// GUI shader.
	Shader gui;
	/// Background shader.
	Shader bgs;
	/// Shadow mask shader.
	Shader sdw;

	/// Reads contents of file.
	/// @param path File path.
	/// @return File data.
	std::vector<char> readfile(const char* path) {
		// open stream
		FILE* file = fopen(path, "r");
		if (file == NULL) {
			const char* error = strerror(errno);
			Log::out.symE();
			Log::out.format("Failed to open file <%s>: %s\n", path, error);
			Log::out.print();
			return {};
		};

		// read entire file
		std::vector<char> result;
		while (true) {
			// read character
			char c = fgetc(file);
			if (feof(file)) break;

			// add character
			result.push_back(c);
		};
		result.push_back(0);

		// return file data
		fclose(file);
		return result;
	};

	/// Loads all shaders.
	void load() {
		Log::out.separator("Shaders");

		// shader file data
		struct filedata_t {
			const char* path;
			char type;
		};
		static const filedata_t shaders[] {
			{ "shade/std.vert", 'v' }, // 0
			{ "shade/sdw.frag", 'f' }, // 1
			{ "shade/tex.frag", 'f' }, // 2
			{ "shade/gui.vert", 'v' }, // 3
			{ "shade/bgs.vert", 'v' }, // 4
			{ "shade/bgs.frag", 'f' }, // 5
		};
		const size_t N = lengthof(shaders);

		// shader program data
		struct progdata_t {
			const char* name;
			Shader* target;
			size_t vert;
			size_t frag;
		};
		static const progdata_t progs[] {
			{ "texture shader"   , &tex, 0, 2 },
			{ "GUI shader"       , &gui, 3, 2 },
			{ "background shader", &bgs, 4, 5 },
			{ "shadow shader"    , &sdw, 4, 1 }
		};

		// load code
		std::vector<char> sources[N];
		for (size_t i = 0; i < N; i++) {
			Log::out.symI();
			Log::out.format("Reading <%s>.\n", shaders[i].path);
			Log::out.print();
			sources[i] = readfile(shaders[i].path);
		};

		// compile shader code
		GLuint IDs[N];
		for (size_t i = 0; i < N; i++) {
			Log::out.symI();
			Log::out.format("Compiling <%s>.\n", shaders[i].path);
			Log::out.print();
			IDs[i] = Shader::compile(sources[i].data(), shaders[i].type);
		};

		// create shader programs
		for (size_t i = 0; i < lengthof(progs); i++) {
			Log::out.symI();
			Log::out.format("Linking %s.\n", progs[i].name);
			Log::out.print();
			*progs[i].target = Shader::create(IDs[progs[i].vert], progs[i].frag == -1ULL ? 0 : IDs[progs[i].frag]);
		};

		// free shader code
		for (size_t i = 0; i < N; i++)
			glDeleteShader(IDs[i]);
	};

	/// Unloads all shaders.
	void unload() {
		tex.unload();
		gui.unload();
		bgs.unload();
		sdw.unload();
	};
};