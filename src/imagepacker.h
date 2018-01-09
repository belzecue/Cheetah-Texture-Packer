#ifndef IMAGEPACKER_H
#define IMAGEPACKER_H
#include "preferences/heuristicsettings.h"
#include "preferences/rotationsettings.h"
#include "preferences/sortsettings.h"
#include <QObject>
#include <QImage>
#include <QRgb>

class MaxRects;
class QTextStream;
class QListWidgetItem;

struct packerData
{
	packerData() : listItem(0L) {}
	QListWidgetItem *listItem;
	QString path;
	QString file;
};

struct packedImage
{
    QImage img;
//is this image itself part of an atlas?
    QRect originalSection;
    QRect crop;
    bool border, rotate;
    int textureId;
    int id;
};

struct ExtrudeData
{
    uint8_t t, b, l, r;
    QPoint pos() const { return QPoint(t, l); }
    int width()  const { return l+r; }
    int height() const { return t+b; }
};

struct inputImage
{
    quint32 hash;
    int textureId;
    packerData *id;
    inputImage *duplicateId;
    QPoint pos;
	QSize originalSize;
    QSize size, sizeCurrent;
    QRect crop;
	QRect section;
    QString path;

    QSize getSize(bool doCrop) const
    {
        return doCrop? crop.size() : size;

    }

    QRect getCrop(bool doCrop) const
    {
       return doCrop? crop : QRect(0, 0, size.width(), size.height());
    //   return rotated? QRect(size.height() - r.y() - r.height(),  r.x(), r.height(), r.width()) : r;
    }

    QRect getOutline(bool doCrop)
    {
       QRect r = QRect(QPoint(0, 0), getSize(doCrop));
       r = rotated? QRect(r.y() - r.height(),  r.x(), r.height(), r.width()) : r;
	   return QRect(pos + r.topLeft(), r.size());
    }

    QImage getImage()
    {
        return id? QImage(id->path) : QImage();
    }

    bool cropped, rotated;
};

class ImagePacker : public QObject
{
    private:
        void internalPack(int heur, int w, int h);

        void SortImages(int w, int h);

    public:
typedef QList<inputImage> List_t;
        List_t images;
        QList<QSize> bins;
        ImagePacker();

		void drawImage(QPainter & p, QImage & img, const QRect & crop);
		void WriteAtlasFile(int j, QTextStream & out, const QString & imgFile) const;
        void CreateOutputTextures(QList<QImage> & textures, bool preview, bool exporting, QPixmap * pattern);
		void DrawOutlines(QList<QImage> & textures);

        bool compareImages(QImage *img1, QImage *img2, int *i, int *j);
        void pack(Heuristic_t heur, int w, int h);

        unsigned AddImgesToBins(Heuristic_t heur, int w, int h);

        void CropLastImage(Heuristic_t heur, int w, int h, bool wh);
        void DivideLastImage(Heuristic_t heur, int w, int h, bool wh);

        void UpdateCrop();
		void applyGreenScreen(QImage & image);

        float GetFillRate();

        void ClearBin(int binIndex);

        int FillBin(Heuristic_t heur, int w, int h, int binIndex);

        QRect crop(const QImage &img);
        ExtrudeData getExtrude(const QRect & crop);

        void addItem(const QImage &img, packerData *data, QString path);
        void addItem(QString path, packerData * data);
        const inputImage *find(void *data);
        void removeId(void *);
        void realculateDuplicates();
        void clear();
        int compare;
        quint64 area, neededArea;
        int missingImages;
        int mergedImages;
        bool ltr, merge, square, autosize, mergeBF;
        int cropThreshold;
        int border;
        int padding;
        int extrude;
		QPoint alignment;
//considered to be transparent if fomrat has no alpha channel.
		QRgb   greenScreen;
		bool   greenScreenToAlpha;
		bool   useGreenScreen;
        Rotation_t rotate;
        Sort_t     sortOrder;
        int minFillRate;
        int minTextureSizeX;
        int minTextureSizeY;
};




#endif // IMAGEPACKER_H
