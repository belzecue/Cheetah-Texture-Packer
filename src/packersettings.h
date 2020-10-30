#ifndef PACKERSETTINGS_H
#define PACKERSETTINGS_H
#include "preferences.h"
#include <glm/gtc/type_precision.hpp>
#include <string>

struct PackerSettings
{
	Preferences::Rotation  rotate{Preferences::Rotation::Automatic};
	Preferences::Heuristic heuristic{Preferences::Heuristic::TopLeft};
	Preferences::Sort      sortOrder{Preferences::Sort::None};
	uint8_t                padding{0};

	uint8_t  cropThreshold{0};
	uint8_t  extrude{0};
	uint8_t  border{0};
	uint8_t  minFillRate{0};

	glm::u16vec2 minSize{512, 512};
	glm::u16vec2 maxSize{2048, 2048};

	uint8_t merge : 1;
	uint8_t mergeBF : 1;
	uint8_t square : 1;
	uint8_t autosize : 1;

	uint8_t greenScreenToAlpha : 1;
	uint8_t useGreenScreen : 1;

	glm::u8vec4  greenScreen;
	glm::u8vec2  alignment;

	std::string outDir;
	std::string outFile;
	std::string imageFormat;
};


#endif // PACKERSETTINGS_
