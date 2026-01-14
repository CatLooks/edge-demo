#pragma once

/// Logging functionality namespace.
namespace Log {
	/// String buffer
	class Buffer {
		public:
		/// Buffer element size.
		static const size_t length = 256;

		/// Returns string representation of a month.
		/// @param month Month number.
		static const char* monthRepr(int month) {
			const char* table[12] {
				"Jan", "Feb", "Mar", "Apr", "May", "Jun",
				"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
			};
			return (month >= 0 && month < 12) ? table[month] : "???";
		};

		private:
		/// String buffer element type.
		struct elem_t {
			char arr[length];
		};

		/// String buffer container.
		std::deque<elem_t> data;
		/// Output stream pointer.
		FILE* stream;
		/// Whether to free stream after destruction.
		bool frees;
		/// Whether to ignore next separator.
		bool sepign;

		public:
		/// Whether to color the output.
		bool color;
		/// Default constructor.
		/// @param stream Output stream.
		/// @param color Whether to apply color to the output text.
		Buffer (FILE* stream, bool color = true): stream(stream), frees(false), sepign(false), color(color) {};

		/// Destructor.
		~Buffer () { if (frees) fclose(stream); };

		/// Sets logging output stream.
		/// @param stream File stream.
		void setStream(FILE* stream) {
			this->stream = stream;
			frees = !(stream == stdout || stream == stdin || stream == stderr || stream == NULL);
		};

		/// Reserves a string buffer.
		/// @return Reserved string pointer.
		char* reserve() {
			data.push_back({});
			data.back().arr[length - 1] = 0;
			return &data.back().arr[0];
		};

		/// Adds a string to buffer queue.
		/// @param str String pointer.
		void queue(const char* str) {
			while (true) {
				// copy string data
				char* buffer = reserve();
				strncpy(buffer, str, length - 1);

				// advance reader
				if (strlen(str) < length - 1)
					break;
				str += length - 1;
			};
		};

		/// Adds a formatted string to buffer queue.
		/// @param fmt Format string.
		/// @param ... Format arguments.
		template <typename... A> void format(const char* fmt, A... args) {
			char* result = formatf(fmt, args...);
			queue(result);
			delete result;
		};

		/// Sets the style for text.
		void style(uint8_t id) { if (color) format("\033[38;5;%dm", id); };

		/// Resets text color.
		void reset() { if (color) queue("\033[0m"); };

		/// Prints an error symbol.
		void symE() { sepign = false; timestamp(); style(9); queue("[E] "); reset(); };
		/// Prints a fatal error symbol.
		void symF() { sepign = false; timestamp(); style(13); queue("[F] "); reset(); };
		/// Prints a warning symbol.
		void symW() { sepign = false; timestamp(); style(10); queue("[W] "); reset(); };
		/// Prints an info symbol.
		void symI() { sepign = false; timestamp(); style(12); queue("[I] "); reset(); };

		/// Prints a separator title.
		/// @param title Middle separator string.
		void separator(const char* title = NULL) {
			if (title == NULL) {
				if (sepign) return;
				timestamp();
				style(11);
				queue("========================================\n");
				reset();
				return;
			};
			timestamp();
			style(11); queue("================ "); reset();
			queue(title);
			style(11); queue(" ================\n"); reset();
			sepign = true;
		};

		/// Adds a timestamp to buffer queue.
		void timestamp() {
			// get local time
			time_t secs = time(NULL);
			tm* now = localtime(&secs);
			if (now == NULL) return;

			// create timestamp format
			style(14);
			format("[%04d %s %02d %02d:%02d:%02d] ",
					now->tm_year + 1900, monthRepr(now->tm_mon), now->tm_mday,
					now->tm_hour, now->tm_min, now->tm_sec);
			reset();
		};

		/// Adds a citation of a string to buffer queue.
		/// @param str Cited string.
		/// @param tab Citation indentation string.
		void cite(const char* str, const char* tab = "    ") {
			// add tabs in string
			std::vector<char> conv;
			bool newline = true;

			// process each character
			for (size_t i = 0; str[i]; i++) {
				// add tab after newline
				if (newline && str[i] != '\n') {
					for (size_t j = 0; tab[j]; j++)
						conv.push_back(tab[j]);
				};

				// add character
				conv.push_back(str[i]);
				newline = str[i] == '\n';
			};
			conv.push_back(0);

			// queue processed string
			queue(conv.data());
		};

		/// Prints out contents of buffer to stream.
		void print() {
			if (stream != NULL) {
				for (const elem_t& elem : data)
					fprintf(stream, elem.arr);
				fflush(stream);
			};
			data.clear();
		};
	};

	/// Error output buffer.
	Buffer out = Buffer(NULL);

	/// Signal report callback.
	void signal(int sig) {
		out.symF();
		out.queue("Received Signal ");
		out.style(0xC);
		out.format("%d", sig);
		out.reset();
		out.queue(": ");
		out.style(0x9);
		switch (sig) {
			case SIGINT  : out.queue("Interrupt");              break;
			case SIGILL  : out.queue("Illegal Instruction");    break;
			case SIGFPE  : out.queue("FPU Exception");          break;
			case SIGSEGV : out.queue("Segmentation Violation"); break;
			case SIGTERM : out.queue("Termination");            break;
			case SIGABRT : out.queue("Abort");                  break;
			default: break;
		};
		out.queue("\n");
		out.reset();
		out.print();
		exit(1);
	};

	/// OpenGL error report callback.
	void GLAPIENTRY errorGL(GLenum source, GLenum type, GLuint ID, GLenum severity, GLsizei length, const char* message, const void* params) {
		/// ignore notifications
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			return;
		switch (type) {
			case GL_DEBUG_TYPE_ERROR              : out.symE(); out.style(0x9); out.queue("Error");               out.reset(); break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: out.symW(); out.style(0x8); out.queue("Deprecated Behavior"); out.reset(); break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR : out.symW(); out.style(0xD); out.queue("Undefined Behavior");  out.reset(); break;
			case GL_DEBUG_TYPE_PORTABILITY        : out.symW(); out.style(0xA); out.queue("Portability");         out.reset(); break;
			case GL_DEBUG_TYPE_PERFORMANCE        : out.symW(); out.style(0xB); out.queue("Performance");         out.reset(); break;
			default                               : out.symW(); out.style(0xE); out.queue("Other");               out.reset(); break;
		};
		out.queue(" Message (");
		switch (source) {
			case GL_DEBUG_SOURCE_API            : out.style(0xB); out.queue("API");             out.reset(); break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM  : out.style(0xC); out.queue("Window");          out.reset(); break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: out.style(0xA); out.queue("Shader Compiler"); out.reset(); break;
			case GL_DEBUG_SOURCE_THIRD_PARTY    : out.style(0xD); out.queue("Third Party");     out.reset(); break;
			default                             : out.style(0x8); out.queue("Other");           out.reset(); break;
		};
		out.queue(" | ");
		switch (severity) {
			case GL_DEBUG_SEVERITY_LOW         : out.style(0xA); out.queue("Low");     out.reset(); break;
			case GL_DEBUG_SEVERITY_MEDIUM      : out.style(0xB); out.queue("Medium");  out.reset(); break;
			case GL_DEBUG_SEVERITY_HIGH        : out.style(0x9); out.queue("High");    out.reset(); break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: out.style(0xC); out.queue("Notif");   out.reset(); break;
			default                            : out.style(0x8); out.queue("Unknown"); out.reset(); break;
		};
		out.queue(" Severity) [ID = ");
		out.style(0xB); out.format("%d", ID); out.reset(); out.queue("]\n");
		out.style(0x8); out.cite(message);    out.reset(); out.queue("\n");
		out.print();
	};
};