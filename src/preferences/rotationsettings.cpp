#include "rotationsettings.h"
#include <QApplication>
#include <QSize>

#define HT(x) QApplication::translate("HelpText", x, Q_NULLPTR)

QStringList g_rotationStrings()
{
	return QStringList()
		     << QString("Never")
		     << QString("Automatic")
		     << QString("H/2 < W < H")
		     << QString("W > H")
		     << QString("W > 2H")
		     << QString("W/2 < H < W")
		     << QString("H > W")
		     << QString("H > 2W");
}

QString g_rotationToolTip()
{
	return QString("<html><head/><body><p>%1</p><p>%2</p></body></html>")
		.arg(HT("Weight function tries to find best place for each image. Top left will be best in most cases."))
		.arg(HT("Some heuristics depend on sorting method."));

}

bool shouldRotate(Rotation_t rot, const QSize & size)
{
	switch(rot)
	{
	case H2_WIDTH_H:
		return  size.height() > size.width() && size.width() * 2 > size.height();
	case WIDTH_GREATHER_HEIGHT:
		return size.width() > size.height();
	case WIDTH_GREATHER_2HEIGHT:
		return size.width() > 2 * size.height();
	case W2_HEIGHT_W:
		return size.width() > size.height() && size.height() * 2 > size.width();
	case HEIGHT_GREATHER_WIDTH:
		return size.height() > size.width();
	case HEIGHT_GREATHER_2WIDTH:
		return size.height() > 2 * size.width();
	default:
		break;
	}

	return false;
}
