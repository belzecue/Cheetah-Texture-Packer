#include "sortsettings.h"
#include "imagepacker.h"
#include <QApplication>
#include <QStringList>


#define PR(x) QApplication::translate("Preferences", x, Q_NULLPTR)
#define HT(x) QApplication::translate("HelpText", x, Q_NULLPTR)

QStringList g_sortStrings()
{
	return QStringList()
		     << PR("Don't sort")
		     << PR("Width")
		     << PR("Height")
		     << PR("Area")
		     << PR("Max");
}

QString     g_sortToolTip()
{
	return QString();
}

static bool ImageCompareByIndex(const inputImage &i1, const inputImage &i2);
static bool ImageCompareByHeight(const inputImage &i1, const inputImage &i2);
static bool ImageCompareByWidth(const inputImage &i1, const inputImage &i2);
static bool ImageCompareByArea(const inputImage &i1, const inputImage &i2);
static bool ImageCompareByMax(const inputImage &i1, const inputImage &i2);

SortFunction_t getSortFunction(Sort_t sort)
{
	switch(sort)
	{
	case WIDTH:    return ImageCompareByWidth;
	case HEIGHT:   return ImageCompareByHeight;
	case SORT_AREA: return ImageCompareByArea;
	case SORT_MAX:  return ImageCompareByMax;
	default: break;
	}

	return 0L;
}

#include <QListWidget>
#include <QListWidgetItem>

static bool ImageCompareByIndex(const inputImage &i1, const inputImage &i2)
{
	if(i1.id->listItem && i2.id->listItem)
	{
		QListWidget * list = i1.id->listItem->listWidget();
		int first  = list->row(i1.id->listItem);
		int second = list->row(i2.id->listItem);

		return first < second;
	}

	return 0;
}

//all of these need to compare such that images with same w/h follow each other in array.

static bool ImageCompareByHeight(const inputImage &i1, const inputImage &i2)
{
    int first =  (i1.sizeCurrent.height() << 16) + i1.sizeCurrent.width();
    int second = (i2.sizeCurrent.height() << 16) + i2.sizeCurrent.width();

	if(first == second)
	{
		return ImageCompareByIndex(i1, i2);
	}
	else
		return first > second;
}

static bool ImageCompareByWidth(const inputImage &i1, const inputImage &i2)
{
    int first = (i1.sizeCurrent.width() << 16) + i1.sizeCurrent.height();
    int second = (i2.sizeCurrent.width() << 16) + i2.sizeCurrent.height();

	if(first == second)
		return ImageCompareByIndex(i1, i2);
	else
		return first > second;
}

static bool ImageCompareByArea(const inputImage &i1, const inputImage &i2)
{
    int first = i1.sizeCurrent.height() * i1.sizeCurrent.width();
    int second = i2.sizeCurrent.height() * i2.sizeCurrent.width();

	if(first == second)
        return ImageCompareByIndex(i1, i2);
	else
		return first > second;
}

static bool ImageCompareByMax(const inputImage &i1, const inputImage &i2)
{
    int first = i1.sizeCurrent.height() > i1.sizeCurrent.width() ?
                (i1.sizeCurrent.height() << 16) + i1.sizeCurrent.width() :
				(i1.sizeCurrent.width()  << 16) + i1.sizeCurrent.height();
    int second = i2.sizeCurrent.height() > i2.sizeCurrent.width() ?
				(i2.sizeCurrent.height() << 16) + i2.sizeCurrent.width() :
				(i2.sizeCurrent.width()  << 16) + i2.sizeCurrent.height();

    if(first == second)
        return ImageCompareByIndex(i1, i2);
    else
        return first > second;
}
