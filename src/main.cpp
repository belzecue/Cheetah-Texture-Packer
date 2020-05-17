#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#   include <QtWidgets/QApplication>
#else
#   include <QtGui/QApplication>
#endif

#include "mainwindow.h"
#include "support.h"
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QDebug>
#include <QPainter>
#include "stdio.h"
#include "stdlib.h"
#include "parsearguments.h"
#include <cassert>

#include <QImageWriter>

static
QStringList GetMimeTypes()
{
	QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = QImageWriter::supportedImageFormats();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();

	return mimeTypeFilters;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QCoreApplication::setApplicationName(QString("%1").arg(argv[0]).section('\\',-1));

	g_extensions = GetMimeTypes();

    //command-line version
    if(argc > 1)
    {
	/*	ImagePacker packer;
		Arguments args(argc, argv, packer);

		QString & outDir  = args.outDir;
        QString & outFile = args.outFile;

		qDebug() << "Saving to dir" << outDir << "and file" << outFile;

        Heuristic_t heuristic = TL;

        QString outFormat("PNG");

        if(packer.images.size() == 0)
        {
            fprintf(stderr, "No images found, exitting\n");
            exit(1);
        }

        packer.pack(heuristic, args.textureWidth, args.textureHeight);

        QList<QImage> textures;
        for(int i = 0; i < packer.bins.size(); i++)
        {
            QImage texture(packer.bins.at(i).width(), packer.bins.at(i).height(),
                           QImage::Format_ARGB32);
            texture.fill(Qt::transparent);
            textures << texture;
        }

        if(!WriteAtlas(0L, textures, packer, outDir, outFile, "txt", outFormat))
			return -1;

		packer.CreateOutputTextures(textures, false, true, 0L);

        qint64 area = CalculateTotalArea(textures);

        float percent = (((float)packer.area / (float)area) * 100.0f);
        //        float percent2 = (float)(((float)packer.neededArea / (float)area) * 100.0f );
        printf("Atlas generated. %f%% filled, %d images missed, %d merged, %d KB\n",
               percent, packer.missingImages, packer.mergedImages, (int)((area * 4) / 1024));

        //        const char * format = qPrintable(outFormat);
		ExportImages(0L, textures, outDir, outFile, outFormat);

        return 0;
		*/
    }

    QTranslator myTranslator;
    myTranslator.load("cheetah_" + QLocale::system().name(), "qm");
    a.installTranslator(&myTranslator);
    MainWindow w;
    w.show();


    return a.exec();
}
