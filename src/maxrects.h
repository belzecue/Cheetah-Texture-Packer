#ifndef MAXRECTS_H
#define MAXRECTS_H
#include <QImage>
#include "imagepacker.h"

struct trbl
{
    QPoint t, r, b, l;
};

struct MaxRectsNode
{
    QRect r; //rect
    //    QSize *i; //image
    trbl b; //border
};
class MaxRects
{
	MaxRectsNode root;
	int w, h;
	QPoint alignment;

    public:
        MaxRects(int width, int height, QPoint align);
        QList<MaxRectsNode> F;
        QList<QRect> R;
        QList<MaxRectsNode *> FR;
        QPoint insertNode(inputImage *);
        int heuristic, rotation;
        bool leftToRight;
        border_t *border;

		inline int width()  const { return w / alignment.x(); }
		inline int height() const { return h / alignment.y(); }
};

#endif // MAXRECTS_H
