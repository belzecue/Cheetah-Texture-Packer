#include "scalesettings.h"
#include "imagescaler.h"
#include <QApplication>
#include <QImage>
#include <iostream>

#define PR(x) QApplication::translate("Preferences", x, Q_NULLPTR)
#define HT(x) QApplication::translate("HelpText", x, Q_NULLPTR)

QStringList g_scaleStrings()
{
    return QStringList()
        << PR("Nearest")
        << PR("Bilinear")
        << PR("SuperXbr");
}

QString     g_scaleToolTip()
{
    return QString();
}

void g_scaleImage(QImage & image, Scale_t scaleType, float scaleFactor)
{
    if(scaleType == s_Nearest)
    {
        image = image.scaled(image.size()*scaleFactor, Qt::KeepAspectRatio, Qt::FastTransformation);
        return;
    }
    else if(scaleType == s_Bilinear)
    {
        image = image.scaled(image.size()*scaleFactor, Qt::KeepAspectRatio, Qt::FastTransformation);
        return;
    }

 //double until we exceed the goal size
    QSize finalSize = scaleFactor * image.size();

    for(;scaleFactor > 2; scaleFactor /= 2)
    {
        image = double_image(image);
    }

//then scale down bilinearly.
    if(scaleFactor != 1)
    {
        image = image.scaled(finalSize, Qt::KeepAspectRatio, Qt::FastTransformation);
    }
}

bool g_cacheScaled(Scale_t scaleType)
{
    return scaleType != s_Nearest;
}
