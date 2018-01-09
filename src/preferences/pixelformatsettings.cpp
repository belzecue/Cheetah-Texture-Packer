#include "pixelformatsettings.h"
#include <QApplication>
#include <iostream>


#define PR(x) QApplication::translate("Preferences", x, Q_NULLPTR)
#define HT(x) QApplication::translate("HelpText", x, Q_NULLPTR)

QStringList g_pixelFormatStrings()
{
	return QStringList()
	<< PR("Mono")
	<< PR("MonoLSB")
	<< PR("Indexed8")
	<< PR("RGB32")
	<< PR("ARGB32")
	<< PR("RGB16")
	<< PR("RGB666")
	<< PR("RGB555")
	<< PR("RGB888")
	<< PR("RGB444")
	<< PR("RGBX8888")
	<< PR("RGBA8888")
	<< PR("BGR30")
	<< PR("RGB30")
	<< PR("Alpha8")
	<< PR("Grayscale8");
}

QString     g_pixelFormatToolTip()
{
    return QString();
}


QImage::Format g_pixelFormatFromIndex(int i)
{
	switch(i)
	{
	case 0: QImage::Format_Mono;
	case 1: QImage::Format_MonoLSB;
	case 2: QImage::Format_Indexed8;
	case 3: QImage::Format_RGB32;
	case 4: QImage::Format_ARGB32;
	case 5: QImage::Format_RGB16;
	case 6: QImage::Format_RGB666;
	case 7: QImage::Format_RGB555;
	case 8: QImage::Format_RGB888;
	case 9: QImage::Format_RGB444;
	case 10: QImage::Format_RGBX8888;
	case 11: QImage::Format_RGBA8888;
	case 12: QImage::Format_BGR30;
	case 13: QImage::Format_RGB30;
	case 14: QImage::Format_Alpha8;
	case 15: QImage::Format_Grayscale8;
	default: break;
	}

	return QImage::Format_RGBA8888;
}
