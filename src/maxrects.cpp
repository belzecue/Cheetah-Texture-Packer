#include "maxrects.h"
#include <iostream>
#include <climits>
#include <cassert>

MaxRects::MaxRects(int w, int h, int border, QPoint align) :
	w(w),
	h(h),
	alignment(align)
{
    F.push_back(QRect(border, border, w-border*2, h-border*2));
}

QPoint MaxRects::insertNode(inputImage *input)
{
    int min = INT_MAX, mini = -1;
    QSize img = input->sizeCurrent;

    //    if(img.width() == w) img.setWidth(img.width() - border->l - border->r);
    //    if(img.height() == h) img.setHeight(img.height() - border->t - border->b);
    if(img.width() <= 1 || img.height() <= 1)
    {
        return QPoint(-1, -1);
    }

    bool leftNeighbor = false, rightNeighbor = false;
    bool _leftNeighbor = false, _rightNeighbor = false;
    bool bestIsRotated = false;

    for(int i = 0; i < F.size(); i++)
    {
        QRect itr = F[i];

//check each image with and without rotation
		bool rotated = false;

		for(;;)
		{
            //can we fit in this hole?
            if(itr.width() >= img.width() && itr.height() >= img.height())
            {
                const int m = calculateHeuristic(itr, img, _leftNeighbor, _rightNeighbor);

                if(m < min)
                {
                    min = m;
                    mini = i;
                    leftNeighbor = _leftNeighbor;
                    rightNeighbor = _rightNeighbor;
                    bestIsRotated = rotated;
                }
           }

            if(rotated || rotation == 0) break;
			rotated = true;
			img.transpose();
		}

		if(rotated)
		{
			img.transpose();
		}
    }

    if(mini < 0)
		return QPoint(-1, -1);

    if(bestIsRotated)
    {
        img.transpose();
        input->rotated = !input->rotated;
        input->sizeCurrent.transpose();
    }

    QRect buf(F[mini].topLeft(), img);

// this was in the original source code by github.com/scriptum
// i really have no idea what it does.
	if(heuristic == TL)
	{
		if(!leftNeighbor && F[mini].x() != 0 &&
				F[mini].width() + F[mini].x() == w)
		{
			buf = QRect(w - img.width(), F[mini].y(), img.width(), img.height());
		}
		if(!leftNeighbor && rightNeighbor)
		{
			buf = QRect(F[mini].x() + F[mini].width() - img.width(), F[mini].y(),
						img.width(), img.height());
		}
    }

    R.push_back(buf);

    CutIntersectingRects(F, buf, padding);

	return QPoint(buf.x(), buf.y());
}

void MaxRects::CutIntersectingRects(std::vector<QRect> &list, QRect buf, int padding)
{
 //add padding to buf (can't do this beforehand)
    if(padding)
        buf = QRect(buf.x()-padding, buf.y()-padding, buf.width()+padding*2, buf.height()+padding*2);

    for(int i = 0; i < list.size(); i++)
	{
        QRect itr = list[i];

		if(!itr.intersects(buf))
			continue;

        list.erase(list.begin()+i); --i;

		if(buf.x() + buf.width() < itr.x() + itr.width())
        {
            AddRect(list,
                QRect(buf.width() + buf.x(),
                      itr.y(),
                      itr.width() + itr.x() - buf.width() - buf.x(),
                      itr.height()));
		}
		if(buf.y() + buf.height() < itr.y() + itr.height())
        {
            AddRect(list,
               QRect(itr.x(),
                     buf.height() + buf.y(),
                     itr.width(),
                     itr.height() + itr.y() - buf.height() - buf.y()));
		}
		if(buf.x() > itr.x())
        {
            AddRect(list,
               QRect(itr.x(),
                     itr.y(),
                     buf.x() - itr.x(),
                     itr.height()));
		}
		if(buf.y() > itr.y())
		{
            AddRect(list,
              QRect(itr.x(),
                    itr.y(),
                    itr.width(),
                    buf.y() - itr.y()));
		}
    }
}


void MaxRects::AddRect(std::vector<QRect> & list, QRect rect)
{
//r = read, w = write
    int r,w;

    for(r=w=0; r < list.size(); ++r)
    {
        if(list[r].contains(rect))
        {
              assert(r == w);
              return;
        }

    // fundmentally, the issue is that we don't want to remove anything;
    // each removal is an O(N) operation in a vector.
    // so we want to shuffle everything along at once, then cut off the tail.
        if(!rect.contains(list[r]))
        {
            if(w != r) list[w] = list[r];
            ++w;
        }
	}

//shouldn't dealloc memory, just reduce the end ptr.
    list.resize(w);
    list.push_back(rect);
}


int64_t MaxRects::sumArea(std::vector<QRect> &list)
{
    int64_t area = 0;
    for(int i = 0; i < list.size(); ++i)
        area += list[i].width() * list[i].height();

    return area;
}
