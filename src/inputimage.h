/**
 * This class was made to increase seperation of objects
 * as well as enable some things like repacking, and
 * fixing the hashes (they fail to detect when two images are identical as a result of cropping)
 * in the end i decided this change wasn't worthwhile
 */

#if 0
#ifndef INPUTIMAGE_H
#define INPUTIMAGE_H
#include <QString>
#include <QFileInfo>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QColor>
#include <cstdint>

#include "qimageptr.hpp"



class InputImage
{
    const QString   m_name;
    QImagePtr m_image;

// misc data about the image
    uint8_t    m_minAlpha;

//used if input image was already part of an atlas
    const QRect  m_section;
    const bool   m_rotated;

//used to detect duplicates before a proper compare
    uint32_t        m_hash;
    const uint32_t  m_id;
    uint32_t        m_duplicateId;

//packed image data
    int    m_textureId;
    QPoint m_pos;
    QRect  m_crop;
    bool   m_rotate;

//value last used for cropping
    QRgb   m_cropped;

	void rehash();

public:
    InputImage(const QImagePtr & image);
    InputImage(const QImagePtr & image, const QString & name, const QRect & section, bool rotate);

    bool operator==(const InputImage & it) const { return hash() == it.hash() && m_crop.size() == it.m_crop.size(); }
    bool operator!=(const InputImage & it) const { return !(*this == it); }

    bool isDuplicate() const { return m_duplicateId != m_id; }
    void setDuplicate(InputImage & it) { m_duplicateId = it.m_duplicateId; }
    void clearDuplicate() { m_duplicateId = m_id; }

//image interface
    void AddRef() { m_image.AddRef(); }
    void Release() { m_image.Release(); }
    const QFileInfo & getFile() { return m_image.getFile(); }
    const QImage & getImage() { return m_image.getImage(); }

//dynamic getters
    bool hasAlpha() const { return m_minAlpha < UCHAR_MAX; }
	uint32_t hash() const { return m_hash; }
//true if the crop section changed
    bool updateCrop(int alphaThreshold, bool useGreenScreen, QColor screenColor);
	bool updateHash();

//packer getters
    int textureId() { return m_textureId; }
    QPoint pos() { return m_pos; }
    bool isRotated() { return m_rotate != m_rotated; }
    QSize size() const { return m_section.size(); }

    QRect readingCrop() const { return QRect(m_section.topLeft() + m_crop.topLeft(), m_crop.size()); }
};

#endif // INPUTIMAGE_H
#endif
