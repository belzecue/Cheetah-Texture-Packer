#ifndef SORTSETTINGS_H
#define SORTSETTINGS_H

struct inputImage;
class QStringList;
class QString;

typedef bool (*SortFunction_t)(const inputImage &i1, const inputImage &i2);

typedef enum {
	SORT_NONE,
	WIDTH,
	HEIGHT,
	SORT_AREA,
	SORT_MAX,
	SORT_NUM
} Sort_t; //sort

SortFunction_t getSortFunction(Sort_t sort);
QStringList g_sortStrings();
QString     g_sortToolTip();
QStringList g_sortStrings();
QString     g_sortToolTip();


#endif // SORTSETTINGS_H
