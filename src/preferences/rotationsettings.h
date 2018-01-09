#ifndef ROTATIONSETTINGS_H
#define ROTATIONSETTINGS_H
#include <QStringList>
#include <QString>

class QSize;

typedef enum
{
	NEVER,
	ONLY_WHEN_NEEDED,
	H2_WIDTH_H,
	WIDTH_GREATHER_HEIGHT,
	WIDTH_GREATHER_2HEIGHT,
	W2_HEIGHT_W,
	HEIGHT_GREATHER_WIDTH,
	HEIGHT_GREATHER_2WIDTH,
	ROTATION_NUM
} Rotation_t; //rotation

bool shouldRotate(Rotation_t rot, const QSize & size);
QStringList g_rotationStrings();
QString     g_rotationToolTip();

#endif // ROTATIONSETTINGS_H
