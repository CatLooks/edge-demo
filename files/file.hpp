#pragma once

/// Returns an allocated file name created from path.
/// @param path File path.
char* getfilename(const char* path) {
	// get string length & last '/' or '\'
	size_t last = -1ULL;
	size_t length = 0;
	while (path[length] != '\0') {
		if (path[length] == '/' || path[length] == '\\')
			last = length;
		length++;
	};

	// create file name copy
	char* name = (char*)malloc(length - last);
	memcpy(name, &path[last + 1], length - last);
	return name;
};

// import units
#include "read.hpp"
#include "write.hpp"
#include "time.hpp"
#include "save.hpp"