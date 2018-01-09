#ifndef SCALESETTINGS_H
#define SCALESETTINGS_H
#include <QStringList>
#include <QString>

//didn't finish this....
//flipping this switch will set up the GUI, but it won't do anything
#define SCALING_ENABLED 0

class QImage;

typedef enum
{
    s_Nearest,
    s_Bilinear,
    s_SuperXbr
} Scale_t;

QStringList g_scaleStrings();
QString     g_scaleToolTip();
#define     g_scaleDefault() s_Bilinear

void g_scaleImage(QImage & image, Scale_t scaleType, float scaleFactor);
bool g_cacheScaled(Scale_t scaleType);

#endif // SCALESETTINGS_H
