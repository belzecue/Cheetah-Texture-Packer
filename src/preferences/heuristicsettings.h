#ifndef HEURISTICSETTINGS_H
#define HEURISTICSETTINGS_H
#include <QStringList>
#include <QString>
#include <QRect>

typedef enum
{
	NONE,
	TL,
	BAF,
	BSSF,
	BLSF,
	MINW,
	MINH,
	HEURISTIC_NUM,
} Heuristic_t; //rotation

QStringList g_heuristicStrings();
QString     g_heuristicToolTip();

#endif // HEURISTICSETTINGS_H
