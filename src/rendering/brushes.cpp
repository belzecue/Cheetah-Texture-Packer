#include "brushes.h"
#include <QPainter>
#include <QPixmap>
#include <QImage>



static QImage g_noiseTexture();
static QPixmap g_transparencyPattern();

QBrush & g_transparencyBrush()
{
    static QBrush brush(g_transparencyPattern());
    return brush;
}

QBrush & g_noiseBrush()
{
    static QBrush brush(g_noiseTexture());
    return brush;
}

enum { BRIGHT = 190, SHADOW = 150, };
static QPixmap g_transparencyPattern()
{
    QPixmap texture(20, 20);
    QPainter painter(&texture);
    painter.fillRect(texture.rect(), QColor(BRIGHT, BRIGHT, BRIGHT));
    painter.fillRect( 0,  0, 10, 10, QColor(SHADOW, SHADOW, SHADOW));
    painter.fillRect(10, 10, 10, 10, QColor(SHADOW, SHADOW, SHADOW));

    return texture;
}

static QImage g_noiseTexture()
{
#if 0
    return QImage("Noise.png");
#else
    QImage image(256, 256, QImage::Format_Grayscale8);
    image.fill(0);

    for(int y = 0; y < image.height(); ++y)
    {
        for(int x = 0; x < image.width(); ++x)
        {
            int eax = (rand() & 0xFF) - (rand() & 0xFF);

            if(eax > 0)
            {
                image.setPixel(y, x, qRgba(eax, eax, eax, 0xFF));
            }
        }
    }

    return image;
#endif
}
