#ifndef QT_TO_GL_H
#define QT_TO_GL_H
#include <QImage>

namespace Qt_to_Gl
{

QImage::Format GetTargetFormat(QImage::Format in);

uint32_t bytesPerPixel(uint32_t type);

}


#endif // QT_TO_GL_H
