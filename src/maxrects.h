#ifndef MAXRECTS_H
#define MAXRECTS_H
#include <QImage>
#include "imagepacker.h"

class MaxRects
{
	int w, h;
	QPoint alignment;


    public:
        MaxRects(int width, int height, int border, QPoint align);
        std::vector<QRect> F;
        std::vector<QRect> R;
        QPoint insertNode(inputImage *);
        Heuristic_t heuristic;
		Rotation_t rotation;
        int  border;
		int  padding;

		inline int width()  const { return w / alignment.x(); }
		inline int height() const { return h / alignment.y(); }
		int calculateHeuristic(QRect itr, QSize img, bool & leftNeighbor, bool & rightNeighbor) const;

private:
//max rects works by dividing each rect that the placed object intersects with, into the largest resulting rectangles.
//we then try to find a placement within those rectangles.


#ifdef NAIVE_IMPLEMENTATION
        for each new rect r
            add r to list

        for i in list 0...N
            for j in list (i+1)...N
                if(i.contains(j)) list.remove(j);
                if(j.contains(i)) list.remove(i);

       Assuming that we have a list of size N, and we are adding M new rectangels for it.
       This appears to be O((N+M)^2); but it isn't because removing from a dynamic array is O(N),
       So this implementation is O((N+M)^3)
#endif

    //this is much more optomized n*(m+n)
    //can be optimized much further, but that resulted in spaghetti code!
     static void CutIntersectingRects(std::vector<QRect> &list, QRect buf, int padding);
     static void AddRect(std::vector<QRect> &list, QRect rect);
     static int64_t sumArea(std::vector<QRect> &list);
};

#endif // MAXRECTS_H
