#pragma once

/// Level related namespace.
namespace Level {
	/// Cube input recorder object.
	struct Recorder {
		/// Output stream.
		FILE* output;
		/// Event count.
		uint32_t count;
		/// Time elapsed since last event.
		float elapsed;
		/// Last cube input state.
		CubeInput last;
		/// Recorded cube pointer.
		Cube* cube;
		/// Whether to sync next offset.
		bool synced;

		/// Recorder constructor.
		Recorder (FILE* output, Cube* cube): output(output), count(0), elapsed(0.0f), cube(cube) {};

		/// Creates time offset for new event.
		float time() {
			if (synced) {
				synced = false;
				elapsed = 0.0f;
			};
			float t = elapsed;
			elapsed = 0.0f;
			return (uint16_t)roundf(t * 240.0f);
		};

		/// Reset recorder.
		void reset() {
			count = 0;
			synced = true;
			last = CubeInput();
		};

		/// Starts recording.
		void start() {
			count = 0;
			elapsed = 0.0f;
			synced = false;
			last = CubeInput();
		};

		/// Update recorder.
		/// @param delta Time elapsed since last frame.
		void update(float delta) {
			// ignore if no cube is bound
			if (cube == NULL) return;

			// get new input
			CubeInput input = cube->keys.state();

			// update time
			elapsed += delta;

			// check for events
			if (input.east != last.east) {
				uint16_t offset = time();
				fprintf(output, "<event type=\"east%s\" time=\"%d\"/>\n", input.east ? "down" : "up", offset);
				count++;
			};
			if (input.west != last.west) {
				uint16_t offset = time();
				fprintf(output, "<event type=\"west%s\" time=\"%d\"/>\n", input.west ? "down" : "up", offset);
				count++;
			};
			if (input.north != last.north) {
				uint16_t offset = time();
				fprintf(output, "<event type=\"north%s\" time=\"%d\"/>\n", input.north ? "down" : "up", offset);
				count++;
			};
			if (input.south != last.south) {
				uint16_t offset = time();
				fprintf(output, "<event type=\"south%s\" time=\"%d\"/>\n", input.south ? "down" : "up", offset);
				count++;
			};

			// update last input
			last = input;
		};

		/// Logs a sync event.
		void sync() {
			synced = true;
			count++;
			fprintf(output, "<event type=\"sync\"/>\n");
		};

		/// Ends current recording.
		void end() {
			fprintf(output, "// end recording (key count = %u) //\n", count);
			reset();
		};
	};
};