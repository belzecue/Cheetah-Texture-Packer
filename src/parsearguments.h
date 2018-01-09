#ifndef PARSEARGUMENTS_H
#define PARSEARGUMENTS_H
#include "imagepacker.h"
#include <QString>

class MainWindow;

typedef enum TokId_t
{
	arg_help,
	arg_size,
	arg_outFile,
	arg_disableMerge,
	arg_disableCrop,
	arg_disableBorder,
	arg_disableRecusion,
	arg_cropThreshold,
	arg_borderSize,
	arg_extrudeSize,
	arg_enableRotate,
	arg_square,
	arg_autosizeThreshold,
	arg_minTextureSize,
	arg_sortOrder,
	arg_badToken = -1
} TokId_t;

struct TokenType
{
	TokId_t token;
	unsigned int length;
	const char * string;
};

struct Arguments
{
	Arguments();
	Arguments(int argc, char * argv[], ImagePacker & packer);

	int textureWidth;
	int textureHeight;
	QString outDir;
	QString outFile;
};

TokId_t getToken(const char * string);

#endif // PARSEARGUMENTS_H
