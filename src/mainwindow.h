#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#   include <QtWidgets/QMainWindow>
#   include <QtWidgets/QListWidget>
#else
#   include <QMainWindow>
#   include <QListWidget>
#endif

#include <QDropEvent>
#include <QUrl>
#include <QDrag>
#include <QLabel>
#include "imagepacker.h"

namespace Ui
{
    class MainWindow;
}

class QShortcut;
class SettingsPanel;
class View;



class MainWindow : public QMainWindow
{
friend class SettingsPanel;
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

		QShortcut * m_delete;
		QString outDir;
		QString outFile;
		QString outFormat;
        QString imgFormat;

        void setZoomText(float);
        bool selectTiles(int texture, QRect rect, Qt::KeyboardModifiers	 keys);
        void packerRepaint();

    private:
		void onSave();
        void displayStatus(const QList<QImage> & textures);

		View   * m_view;
		QLabel * m_status;
		QLabel * m_scale;

        Ui::MainWindow *ui;
		SettingsPanel * prefs;

        void RecurseDirectory(const QString &dir);
        QString topImageDir;
        ImagePacker packer;
        bool exporting;
        int recursiveLoaderCounter;
        bool recursiveLoaderDone;
        QPixmap pattern;
        void addDir(QString dir);

    protected:
        void dropEvent(QDropEvent *event);
        void dragEnterEvent(QDragEnterEvent *event);

    public slots:
        void addTiles();
        void deleteSelectedTiles();
        void packerUpdate();
        void updateAuto();
        void updateAplhaThreshold();
        void exportImage();
        void clearTiles();
};

#endif // MAINWINDOW_H
