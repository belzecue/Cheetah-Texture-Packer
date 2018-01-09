#if 1
typedef int nothing;
#else
#include "inputimage.h"
#include <QImageReader>

static uint32_t g_currentId = 0;

InputImage::InputImage(const QImagePtr & image) :
    InputImage(image, image.getFile().fileName(), QRect(QPoint(0, 0), QImageReader(image.getFile().path()).size()), false)
{
}

InputImage::InputImage(const QImagePtr & image, const QString & name, const QRect & section, bool rotate) :
    m_name(name),
    m_image(image),
    m_hasAlpha(false),
    m_section(section),
    m_rotated(rotate),
    m_hash(0L),
    m_id(++g_currentId),
    m_duplicateId(m_id),
    m_textureId(-1),
    m_pos(-1, -1),
    m_rotate(false),
    m_cropped(0L)
{
    AddRef();
    QImage & image = getImage();


//try to reduce function calls, (compiler doesn't understand this is our only thread)
//this will take some time!
	int Nx = m_section.bottom();
	int Ny = m_section.right();
//reduce pointer transitions 'cause possible.
	uint8_t buf  = ~0;
    for(int y = section.top(); y <= Ny; ++y)
    {
        for(int x = section.left(); x <= Nx; ++x)
        {
			uint8_t px = qAlpha(image.pixel(x, y));
			if(px < buf && (buf = px) == 0) goto break_loop;
        }
    }

break_loop:
	m_minAlpha = buf;
    Release();
}

#define CMPF1(x,y,a) (qAlpha(img.pixel(x,y)) <= a)
#define CMPF3(x,y,a) (img.pixel(x,y) == a)

bool InputImage::updateCrop(int cropThreshold, bool useGreenScreen, QColor greenScreen)
{
//needs to be reset regardless of our result.
	m_duplicateId = m_id;

	if(hasAlpha())
	{
		if(cropThreshold < m_minAlpha
		|| cropThreshold == qAlpha(m_cropped))
		{
			m_cropped = qRgba(0, 0, 0, cropThreshold);
			return false;
		}

		m_cropped = qRgba(0, 0, 0, cropThreshold);
	}
	else
	{
		if(!useGreenScreen)
		{
			greenScreen.setAlpha(0L);
			if(m_crop != QRect(0, 0, m_section.width(), m_section.height()))
			{
				m_crop = QRect(0, 0, m_section.width(), m_section.height());
				return true;
			}
			return false;
		}
		else
		{
			if(m_cropped == greenScreen.rgb()) return false;
			m_cropped = greenScreen.rgb();
		}
	}

    QImage & img = getImage();
	int j, w, h, x, y;
    //QImage im;
    bool t;

	if(useGreenScreen && !hasAlpha())
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

    if(w < x) w = x;
    if(h < y) h = y;

	m_crop = QRect(x, y, w-x, h-x);
	return true;
}

uint32_t rc_crc32(uint32_t crc, const uint8_t *buf, uint64_t len);

void InputImage::updateHash()
{
	QImage & img = getImage();
	uint32_t * table = rc_crc32_table();
    uint32_t octet;
    const uint8_t *p, *q;
	uint32_t crc = 0;

	//the issue here is that cropping and greenscreening can both change the result.
	//meaning it is never possible to just input the raw bytes, even if it doesn't apply to us
	//it may apply to somethign that we match with!

//must be a green screen effect
	if(qAlpha(m_cropped) == UCHAR_MAX)
	{
		for(int y = m_crop.top(); y < m_crop.bottom(); ++y)
		{
			for(int x = m_crop.top(); x < m_crop.bottom(); ++x)
			{
				QRgb px = img.pixel(x, y);
				if(px == m_cropped)
				{
					crc = (crc >> 8) ^ table[(crc & 0xff)];
					crc = (crc >> 8) ^ table[(crc & 0xff)];
					crc = (crc >> 8) ^ table[(crc & 0xff)];
					crc = (crc >> 8) ^ table[(crc & 0xff)];
				}
				else
				{
					crc = (crc >> 8) ^ table[(crc & 0xff) ^ qRed(crc)];
					crc = (crc >> 8) ^ table[(crc & 0xff) ^ qGreen(crc)];
					crc = (crc >> 8) ^ table[(crc & 0xff) ^ qBlue(crc)];
					crc = (crc >> 8) ^ table[(crc & 0xff) ^ qAlpha(crc)];
				}
			}
		}
	}
//it's an alpha effect
	else
	{
		for(int y = m_crop.top(); y < m_crop.bottom(); ++y)
		{
			for(int x = m_crop.top(); x < m_crop.bottom(); ++x)
			{
				QRgb px = img.pixel(x, y);
				if(px == m_cropped)
				{
					crc = (crc >> 8) ^ table[(crc & 0xff)];
					crc = (crc >> 8) ^ table[(crc & 0xff)];
					crc = (crc >> 8) ^ table[(crc & 0xff)];
					crc = (crc >> 8) ^ table[(crc & 0xff)];
				}
				else
				{
					crc = (crc >> 8) ^ table[(crc & 0xff) ^ qRed(crc)];
					crc = (crc >> 8) ^ table[(crc & 0xff) ^ qGreen(crc)];
					crc = (crc >> 8) ^ table[(crc & 0xff) ^ qBlue(crc)];
					crc = (crc >> 8) ^ table[(crc & 0xff) ^ qAlpha(crc)];
				}
			}
		}
	}


	m_hash = ~crc;
}

#endif

