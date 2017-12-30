#include "imagepacker.h"
#include <QDebug>
#include "maxrects.h"
#include <cassert>

ImagePacker::ImagePacker()
{
    prevSortOrder = -1;
    extrude = 1;
	alignment = QPoint(4, 4);
    cropThreshold = 10;
    minTextureSizeX = 32;
    minTextureSizeY = 32;
	greenScreen = qRgba(0, 0, 0, 255);
	greenScreenToAlpha = false;
	useGreenScreen = false;
}

void ImagePacker::pack(int heur, int w, int h)
{
// alignment below 1 px limit doesn't make any sense!
	assert(alignment.x() > 0 && alignment.y() > 0);

    SortImages(w, h);

    missingImages = 1;
    mergedImages = 0;
    area = 0;
    bins.clear();

    unsigned areaBuf = AddImgesToBins(heur, w, h);

    if(areaBuf && !missingImages)
    {
        CropLastImage(heur, w, h, false);
    }

    if(merge)
        for(int i = 0; i < images.size(); i++)
		{
			inputImage & img = images[i];
            if(img.duplicateId != NULL)
            {
				const inputImage * value = find(img.duplicateId);
                img.pos       = value->pos;
                img.textureId = value->textureId;
                mergedImages++;
            }
		}
}

quint32 rc_crc32(quint32 crc, const uchar *buf, size_t len)
{
    static quint32 table[256];
    static int have_table = 0;
    quint32 rem, octet;
    const uchar *p, *q;

    /* This check is not thread safe; there is no mutex. */
    if(have_table == 0)
    {
        /* Calculate CRC table. */
        for(int i = 0; i < 256; i++)
        {
            rem = i;  /* remainder from polynomial division */
            for(int j = 0; j < 8; j++)
            {
                if(rem & 1)
                {
                    rem >>= 1;
                    rem ^= 0xedb88320;
                }
                else
                {
                    rem >>= 1;
                }
            }
            table[i] = rem;
        }
        have_table = 1;
    }

    crc = ~crc;
    q = buf + len;
    for(p = buf; p < q; p++)
    {
        octet = *p;  /* Cast to unsigned octet. */
        crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
    }
    return ~crc;
}

void ImagePacker::UpdateCrop()
{
    for(int i = 0; i < images.size(); ++i)
    {
        images[i].crop = crop(QImage(images[i].path));
    }
}

void ImagePacker::addItem(const QImage &img, void *data, QString path)
{
    inputImage i;
    if(img.width() == 0 || img.height() == 0)
    {
        return;
    }
    i.hash = rc_crc32(0, img.bits(), img.byteCount());
    i.crop = crop(img);
    i.size = img.size();
    i.id = data;
    i.path = path;
    images << i;
}

void ImagePacker::addItem(QString path, void *data)
{
    addItem(QImage(path), data, path);
}

void ImagePacker::clear()
{
    images.clear();
}

void ImagePacker::realculateDuplicates()
{
    for(int i = 0; i < images.count(); i++)
    {
        images[i].duplicateId = NULL;
    }
    for(int i = 0; i < images.count(); i++)
    {
//call stack operation & dual pointer lookup can be avoided
		inputImage & i0 = images[i];

        for(int k = i + 1; k < images.count(); k++)
        {
			inputImage & k0 = images[k];

            if(k0.duplicateId == NULL &&
                    i0.hash == k0.hash &&
                    i0.size == k0.size &&
                    i0.crop == k0.crop)
            {
                k0.duplicateId = i0.id;
            }
        }
    }
}

void ImagePacker::removeId(void *data)
{
    for(int k = 0; k < images.count(); k++)
    {
        if(images.at(k).id == data)
        {
            images.removeAt(k);
            break;
        }
    }
}
const inputImage *ImagePacker::find(void *data)
{
    for(int i = 0; i < images.count(); i++)
    {
		inputImage * i0 = &images[i];
        if(data == i0->id)
        {
            return i0;
        }
    }
    return NULL;
}

void ImagePacker::SortImages(int w, int h)
{
    realculateDuplicates();
    neededArea = 0;
    QSize size;
    for(int i = 0; i < images.size(); i++)
    {
		inputImage & img = images[i];

        img.pos = QPoint(999999, 999999);
        if(cropThreshold)
        {
            size = img.crop.size();
        }
        else
        {
            size = img.size;
        }
/*
		if(alignment.x() && size.width() % alignment.x())
		{
			size.setWidth(size.width() + (alignment.x() - (size.width() % alignment.x())));
		}
		if(alignment.y() && size.height() % alignment.y())
		{
			size.setHeight(size.height() + (alignment.y() - (size.height() % alignment.y())));
		}*/

        if(size.width() != w)
        {
            size.setWidth(size.width() + border.l + border.r + 2 * extrude);
        }
        if(size.height() != h)
        {
            size.setHeight(size.height() + border.t + border.b + 2 * extrude);
        }

        img.rotated = false;
        if((rotate == WIDTH_GREATHER_HEIGHT && size.width() > size.height()) ||
                (rotate == WIDTH_GREATHER_2HEIGHT && size.width() > 2 * size.height()) ||
                (rotate == HEIGHT_GREATHER_WIDTH && size.height() > size.width()) ||
                (rotate == H2_WIDTH_H && size.height() > size.width() &&
                 size.width() * 2 > size.height()) ||
                (rotate == W2_HEIGHT_W && size.width() > size.height() &&
                 size.height() * 2 > size.width()) ||
                (rotate == HEIGHT_GREATHER_2WIDTH && size.height() > 2 * size.width()))
        {
            size.transpose();
            img.rotated = true;
        }
        img.sizeCurrent = size;
        if(img.duplicateId == NULL || !merge)
        {
            neededArea += size.width() * size.height();
        }
    }
    sort();
}

int ImagePacker::FillBin(int heur, int w, int h, int binIndex)
{
    int areaBuf = 0;
    MaxRects rects(w, h, alignment);
    rects.heuristic = heur;
    rects.leftToRight = ltr;
    rects.rotation = rotate;
    rects.border = &border;
    for(int i = 0; i < images.size(); i++)
    {
		inputImage & img = images[i];

        if(QPoint(999999, 999999) != img.pos)
        {
            continue;
        }
        if( img.duplicateId == NULL || !merge)
        {
            img.pos = rects.insertNode(&img);
            img.textureId = binIndex;
            if(QPoint(999999, 999999) == img.pos)
            {
                missingImages++;
            }
            else
            {
//compiler can't optimize this (doesn't know that these values aren't changed in another thread)
				float eax = img.sizeCurrent.width() * img.sizeCurrent.height();
                areaBuf += eax;
                area    += eax;
            }
        }
    }
    return areaBuf;
}

void ImagePacker::ClearBin(int binIndex)
{
    for(int i = 0; i < images.size(); i++)
    {
		inputImage & img = images[i];

        if( img.textureId == binIndex)
        {
            area -= img.sizeCurrent.width() * img.sizeCurrent.height();
            img.pos = QPoint(999999, 999999);
        }
    }
}

unsigned ImagePacker::AddImgesToBins(int heur, int w, int h)
{
    int binIndex = bins.count() - 1;
    unsigned areaBuf = 0;
    unsigned lastAreaBuf = 0;
    do
    {
        missingImages = 0;
        bins << QSize(w, h);
        lastAreaBuf = FillBin(heur, w , h , ++binIndex);
        if(!lastAreaBuf)
        {
            bins.removeLast();
        }
        areaBuf += lastAreaBuf;
    }
    while(missingImages && lastAreaBuf);
    return areaBuf;
}

void ImagePacker::CropLastImage(int heur, int w, int h, bool wh)
{
    missingImages = 0;
    QList<inputImage> last_images = images;
    QList<QSize> last_bins = bins;
    quint64 last_area = area;

    bins.removeLast();
    ClearBin(bins.count());

    if(square)
    {
        w /= 2;
        h /= 2;
    }
    else
    {
        if(wh)
        {
            w /= 2;
        }
        else
        {
            h /= 2;
        }
        wh = !wh;
    }

    int binIndex = bins.count();
    missingImages = 0;
    bins << QSize(w, h);
    FillBin(heur, w , h , binIndex);
    if(missingImages)
    {
        images = last_images;
        bins = last_bins;
        area = last_area;
        missingImages = 0;
        if(square)
        {
            w *= 2;
            h *= 2;
        }
        else
        {
            if(!wh)
            {
                w *= 2;
            }
            else
            {
                h *= 2;
            }
            wh = !wh;
        }
        if(autosize)
        {
            float rate = GetFillRate();
            if((rate < (static_cast<float>(minFillRate) / 100.f)) &&
                    ((w > minTextureSizeX) && (h > minTextureSizeY)))
            {
                DivideLastImage(heur, w, h, wh);
                if(GetFillRate() <= rate)
                {
                    images = last_images;
                    bins = last_bins;
                    area = last_area;
                }
            }
        }
    }
    else
    {
        CropLastImage(heur, w, h, wh);
    }
}

void ImagePacker::DivideLastImage(int heur, int w, int h, bool wh)
{
    missingImages = 0;
    QList<inputImage> last_images = images;
    QList<QSize> last_bins = bins;
    quint64 last_area = area;

    bins.removeLast();
    ClearBin(bins.count());

    if(square)
    {
        w /= 2;
        h /= 2;
    }
    else
    {
        if(wh)
        {
            w /= 2;
        }
        else
        {
            h /= 2;
        }
        wh = !wh;
    }
    AddImgesToBins(heur, w, h);
    if(missingImages)
    {
        images = last_images;
        bins = last_bins;
        area = last_area;
        missingImages = 0;
    }
    else
    {
        CropLastImage(heur, w, h, wh);
    }
}

float ImagePacker::GetFillRate()
{
    quint64 binArea = 0;
    for(int i = 0; i < bins.count(); i++)
    {
        binArea += bins.at(i).width() * bins.at(i).height();
    }
    return (float)((double)area / (double)binArea);
}
