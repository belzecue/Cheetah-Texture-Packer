#ifndef PIXELFORMATSETTINGS_H
#define PIXELFORMATSETTINGS_H
#include <QStringList>
#include <QString>
#include <QImage>

//didn't finish this....
//flipping this switch will set up the GUI, but it won't do anything
#define PIXEL_FORMAT_ENABLED 0

QStringList g_pixelFormatStrings();
QString     g_pixelFormatToolTip();
#define     g_pixelFormatDefault() 8

QImage::Format g_pixelFormatFromIndex(int);

#endif // PIXELFORMATSETTINGS_H
