#include "rendering/brushes.h"
#include "mainwindow.h"
#include "view.h"
#include <QPaintEvent>
#include <QPainter>
#include <QComboBox>
#include <QDebug>
#include <QLabel>
#include <iostream>
#include <QVBoxLayout>

static constexpr float ZOOM_IN  = 9/8.0;
static constexpr float ZOOM_OUT = 1 / ZOOM_IN;

View::View(MainWindow & m, QWidget *parent) :
    super(parent),
    m_window(m),
	m_layout(0L)
{
    scale = 1;
}

void View::wheelEvent(QWheelEvent * event)
{
    if(event->modifiers() & Qt::ControlModifier
    && event->orientation() == Qt::Vertical)
    {
        float f = event->delta() > 0? ZOOM_IN-1 : 1-ZOOM_OUT;
        f = 1 + f * event->delta() / 120;
        changeZoom(scale *f);
    }

    super::wheelEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent * event)
{
    if(m_layout)
    {
        for(int i = 0; i < m_layout->count(); ++i)
        {
            QWidget * widget = m_layout->itemAt(i)->widget();
            if(widget && widget->underMouse())
            {
                if(m_window.selectTiles(i, QRect((event->pos() - widget->pos())/scale, QSize()), event->modifiers()))
                    m_window.packerRepaint();

                return;
            }
        }
    }

    super::mouseReleaseEvent(event);
}

void View::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);

	painter.setBrush(QColor(81, 112, 112, 255));
	painter.drawRect(event->rect());

//give it a matte finish!
    painter.setBrush(g_noiseBrush());
//	painter.setBrush(QBrush();
	painter.setOpacity(.1);
	painter.drawRect(event->rect());


	/*
    int minHeight = 0;
    for(int i = 0; i < textures.count(); i++)
    {
        minHeight += (textures[i].size().height() + 10) * scale;
    }
    this->setMinimumSize(size.width()*scale, minHeight);
    int pos = 0;
    for(int i = 0; i < textures.count(); i++)
    {
        painter.fillRect(0, pos, textures.at(i).width()*scale,
                         textures.at(i).height()*scale, Qt::magenta);
        painter.drawPixmap(0, pos, textures.at(i).width()*scale,
                           textures.at(i).height()*scale, textures.at(i));
        pos += (textures.at(i).height() + 10) * scale;
    }*/

//qlabels seem to snap back to original size after scroll area resizes? (kUbuntu)
	changeZoom(scale);
}


void View::clear()
{
	if(!m_layout) return;

	for(int i = m_layout->count()-1; i >= 0; --i)
	{
		delete m_layout->takeAt(i)->widget();
	}

	delete m_layout;
}

void View::updatePixmap(const QList<QImage> &images)
{
	clear();
	m_layout = new QVBoxLayout(this);

    for(int i = 0; i < images.count(); i++)
    {
		QLabel * label = new QLabel(this);
        label->setAlignment(Qt::AlignCenter);
		label->setStyleSheet("QLabel { background-color : magenta; }");
		label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		label->setPixmap(QPixmap::fromImage(images.at(i)));
		label->setScaledContents(true);
		m_layout->addWidget(label);
	}

	changeZoom(scale);
}

void View::zoomIn()
{
    changeZoom(scale*ZOOM_IN);
}

void View::zoomOut()
{
    changeZoom(scale*ZOOM_OUT);
}

void View::changeZoom(float value)
{
    scale = .25 > value? .25 : value < 8.0? value : 8.0;

	if(m_layout)
	{
		int N = m_layout->count();
		for(int i = 0; i < N; ++i)
		{
			QLabel * label = (QLabel*) (m_layout->itemAt(i)->widget());
			QSize size = label->pixmap()->size();
			label->resize(size*value);
		}
	}

    m_window.setZoomText(scale);
}


