#include "qimageview.h"
#include "rendering/brushes.h"
#include "imagepacker.h"
#include <QPainter>
#include <QPaintEvent>
#include <QListWidgetItem>

#define DISPLAY_STENCIL

QImageView::QImageView(ImagePacker &packer) :
        m_packer(packer)
{

}

QImageView::~QImageView()
{
}

void QImageView::mousePressEvent(QMouseEvent * event)
{


}

void QImageView::paintEvent(QPaintEvent * event)
{
    QPainter p(this);

//this->packer->images->array, 3 pointer transitions!!!
    ImagePacker::List_t & list = m_packer.images;
    bool merge  = m_packer.merge;
    int  id = m_imageId;

#ifdef DISPLAY_STENCIL
    p.setBrush(Qt::magenta);
    p.drawRect(event->rect());

    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);

    for(auto i = list.begin(); i != list.end(); ++i)
    {
        if((i->duplicateId != NULL && merge)
        ||  i->textureId   != id) continue;

        QRect crop = i->getCrop(m_packer.cropThreshold);
        ExtrudeData extrude = m_packer.getExtrude(crop);

        p.drawRect(0, 0, crop.width() + extrude.width(),
                         crop.height() + extrude.height());
    }

//draw checkerboard thing
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    p.setBrush(g_transparencyBrush());
    p.drawRect(event->rect());
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
#else
    p.setBrush(g_transparencyBrush());
    p.drawRect(event->rect());
#endif

 //draw our current pixmap to the screen.
    p.drawPixmap(event->rect(), m_pixmap);

//draw selection outlines
    p.setPen  (Qt::yellow);
    p.setBrush(Qt::NoBrush);
    for(auto i = list.begin(); i != list.end(); ++i)
    {
        if((i->duplicateId != NULL && merge)
        ||  i->textureId   != id) continue;

        QListWidgetItem * listItem = i->id->listItem;
        if(!listItem || !listItem->isSelected()) continue;

        QRect crop = i->getOutline(m_packer.cropThreshold);
        ExtrudeData extrude = m_packer.getExtrude(crop);
        p.drawRect(extrude.l+crop.x(), extrude.t+crop.y(), crop.width(), crop.height());
    }
}
