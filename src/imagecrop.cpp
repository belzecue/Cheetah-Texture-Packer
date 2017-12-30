#include "imagepacker.h"
#include <QHash>

#define CMPF1(x,y,a) (qAlpha(img.pixel(x,y)) <= a)
#define CMP(x,y,a) if(CMPF1(x,y,a)) {t = false; break;} if(!t) break;
#define CMPF3(x,y,a) (img.pixel(x,y) == a)

/*
#define CMPF1(x,y,a) (qAlpha(img.pixel(x,y)) > a)

*/

static
bool doesImageHaveAlpha(const QImage &img)
{
	if(!img.hasAlphaChannel()) return false;

//it may have been added during conversion/loading.
	for(int y = 0; y < img.height(); y++)
	{
		for(int x = 0; x < img.width(); x++)
		{
			if(qAlpha(img.pixel(x,y)) != 255)
				return true;
		}
	}

	return false;
}

//auto-cropping algorithm
QRect ImagePacker::crop(const QImage &img)
{
    int j, w, h, x, y;
    //QImage im;
    bool t;

	if(useGreenScreen && !doesImageHaveAlpha(img))
	{
		//crop only alpha
		t = true;
		for(y = 0; y < img.height(); y++)
		{
			for(j = 0; j < img.width(); j++)
			{
				t &= CMPF3(j, y, greenScreen);
				if(!t) break;
			}
			if(!t) break;
		}

		t = true;
		for(x = 0; x < img.width(); x++)
		{
			for(j = y; j < img.height(); j++)
			{
				t &= CMPF3(x, j, greenScreen);
				if(!t) break;
			}

			if(!t) break;
		}
		t = true;
		for(w = img.width(); t && w > 0; w--)
		{
			for(j = y; j < img.height(); j++)
			{
				t &= CMPF3(w - 1, j, greenScreen);
				if(!t) break;
			}

			if(!t) break;
		}
		t = true;
		for(h = img.height(); t && h > 0; h--)
		{
			for(j = x; j < w; j++)
			{
				t &= CMPF3(j, h - 1, greenScreen);
				if(!t) break;
			}

			if(!t) break;
		}
	}
	else
	{
		//crop only alpha
		t = true;
		for(y = 0; y < img.height(); y++)
		{
			for(j = 0; j < img.width(); j++)
			{
				t &= CMPF1(j, y, cropThreshold);
				if(!t) break;
			}

			if(!t) break;
		}

		t = true;
		for(x = 0; x < img.width(); x++)
		{
			for(j = y; j < img.height(); j++)
			{
				t &= CMPF1(x, j, cropThreshold);
				if(!t) break;
			}

			if(!t) break;
		}
		t = true;
		for(w = img.width(); t && w > 0; w--)
		{
			for(j = y; j < img.height(); j++)
			{
				t &= CMPF1(w - 1, j, cropThreshold);
				if(!t) break;
			}

			if(!t) break;
		}
		t = true;
		for(h = img.height(); t && h > 0; h--)
		{
			for(j = x; j < w; j++)
			{
				t &= CMPF1(j, h - 1, cropThreshold);
				if(!t) break;
			}

			if(!t) break;
		}
	}

    w = w - x;
    h = h - y;
    if(w < 0)
    {
        w = 0;
    }
    if(h < 0)
    {
        h = 0;
    }
    return QRect(x, y, w, h);
}

void ImagePacker::applyGreenScreen(QImage & img)
{
	if(!useGreenScreen || !greenScreenToAlpha || doesImageHaveAlpha(img))
		return;

	if(!img.hasAlphaChannel())
	{
//only one known to be lossless...
		img = img.convertToFormat(QImage::Format_ARGB32);
	}

	for(int y = 0; y < img.height(); y++)
	{
		for(int x = 0; x < img.width(); x++)
		{
			if(img.pixel(x,y) == greenScreen)
				img.setPixel(x, y, qRgba(0, 0, 0, 0));
		}
	}



}
