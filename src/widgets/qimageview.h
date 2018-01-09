#ifndef QIMAGEVIEW_H
#define QIMAGEVIEW_H
#include <QPixmap>
#include <QWidget>

class ImagePacker;

/** this class was made to better deal with scaling and
 * format changes, but both were left unimplemented, so
 * this is unused.
 *
 * It also tries to make the image handling more uniform,
 * with the transparency background rendered here rather
 * than in the image itself. */

class QImageView : public QWidget
{
    Q_OBJECT

    ImagePacker & m_packer;
    QPixmap m_pixmap;

    float   m_scale;
    int     m_imageId;

public:
    QImageView(ImagePacker & packer);
    virtual ~QImageView();

private:
    void mousePressEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *event);
};

#endif // QIMAGEVIEW_H
