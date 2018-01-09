#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingspanel.h"
#include "widgets/view.h"
#include "support.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QMimeData>
#include <QPushButton>
#include <QImageWriter>
#include <QShortcut>
#include <QKeySequence>
#include <QMenu>
#include <QPainter>
#include <iostream>
#include <cassert>


typedef void (QComboBox::*QComboBoxChanged)(int);
typedef void (QSpinBox::*QSpinBoxChanged)(int);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
	prefs(0L)
{
	m_delete = new QShortcut(QKeySequence(QKeySequence::Delete), this);
	m_status = new QLabel();
	m_scale  = new QLabel();

    exporting = false;
    ui->setupUi(this);

	prefs = new SettingsPanel(*this);

    m_view = new View(*this);
	ui->scrollArea->setWidget(m_view);
	ui->scrollArea->setAutoFillBackground(false);

	outDir = QDir::homePath();

    exporting = false;
	/*
    ui->widget->scaleBox = ui->d_scale;
    tabifyDockWidget(ui->dockPreferences, ui->dockExport);
    ui->dockPreferences->raise();
	*/

	ui->statusBar->addWidget(m_status);
	ui->statusBar->addPermanentWidget(m_scale);

    pattern = QPixmap(20, 20);
    QPainter painter(&pattern);

#define BRIGHT 190
#define SHADOW 150
    painter.fillRect(0, 0, 10, 10, QColor(SHADOW, SHADOW, SHADOW));
    painter.fillRect(10, 0, 10, 10, QColor(BRIGHT, BRIGHT, BRIGHT));
    painter.fillRect(10, 10, 10, 10, QColor(SHADOW, SHADOW, SHADOW));
    painter.fillRect(0, 10, 10, 10, QColor(BRIGHT, BRIGHT, BRIGHT));

    setAcceptDrops(true);

//	QComboBoxChanged qComboBoxChanged = &QComboBox::currentIndexChanged;
//connnect things state set to ensure auto update doesn't go off.
	/*
	connect(ui->d_heuristic,         qComboBoxChanged, this, &MainWindow::updateAuto);
	connect(ui->d_sortOrder,         qComboBoxChanged, this, &MainWindow::updateAuto);
	connect(ui->d_rotationStrategy,  qComboBoxChanged, this, &MainWindow::updateAuto);

	connect(ui->c_previewWithImages, &QAction::toggled,               this, &MainWindow::updateAuto);
	connect(ui->c_merge,             &QCheckBox::stateChanged,        this, &MainWindow::updateAuto);
	connect(ui->c_autosize,          &QCheckBox::stateChanged,        this, &MainWindow::updateAuto);
	connect(ui->c_square,            &QCheckBox::stateChanged,        this, &MainWindow::updateAuto);

	connect(ui->c_greenScreen,       &QCheckBox::stateChanged,        this, &MainWindow::updateAuto);
	connect(ui->c_greenScreenAlpha,  &QCheckBox::stateChanged,        this, &MainWindow::updateAuto);
	connect(ui->t_greenScreenColor,  &QLineEdit::editingFinished,     this, &MainWindow::updateAuto);
	*/

	//QSpinBoxChanged qSpinBoxChanged = &QSpinBox::valueChanged;
	/*
	connect(ui->s_borderBottom,      qSpinBoxChanged,         this, &MainWindow::updateAuto);
	connect(ui->s_borderRight,       qSpinBoxChanged,         this, &MainWindow::updateAuto);
	connect(ui->s_borderTop,         qSpinBoxChanged,         this, &MainWindow::updateAuto);
	connect(ui->s_borderLeft,        qSpinBoxChanged,         this, &MainWindow::updateAuto);
	connect(ui->s_textureW,          qSpinBoxChanged,         this, &MainWindow::updateAuto);
	connect(ui->s_textureH,          qSpinBoxChanged,         this, &MainWindow::updateAuto);
	connect(ui->s_extrude,           qSpinBoxChanged,         this, &MainWindow::updateAuto);
	connect(ui->s_minFillRate,       qSpinBoxChanged,         this, &MainWindow::updateAuto);
	connect(ui->s_alignX,            qSpinBoxChanged,         this, &MainWindow::updateAuto);
	connect(ui->s_alignY,            qSpinBoxChanged,         this, &MainWindow::updateAuto);
	*/


    connect(ui->m_close,   &QAction::triggered, this, &MainWindow::clearTiles);
    connect(ui->m_zoom25,  &QAction::triggered, [this]() {  m_view->changeZoom(.25); } );
    connect(ui->m_zoom50,  &QAction::triggered, [this]() {  m_view->changeZoom(.50); } );
    connect(ui->m_zoom100, &QAction::triggered, [this]() {  m_view->changeZoom(1.0); } );
    connect(ui->m_zoom200, &QAction::triggered, [this]() {  m_view->changeZoom(2.0); } );
    connect(ui->m_zoom400, &QAction::triggered, [this]() {  m_view->changeZoom(4.0); } );
    connect(ui->m_zoom800, &QAction::triggered, [this]() {  m_view->changeZoom(8.0); } );

    connect(ui->m_zoomIn , &QAction::triggered, [this]() { m_view->zoomIn(); } );
    connect(ui->m_zoomOut, &QAction::triggered, [this]() { m_view->zoomOut();  } );

	connect(ui->tilesList, &QListWidget::itemSelectionChanged, this, &MainWindow::packerRepaint);


	connect(m_delete, &QShortcut::activated, this, &MainWindow::deleteSelectedTiles);

	/*
	connect(ui->b_update, SIGNAL(clicked()), MainWindow, SLOT(packerUpdate()));
	connect(ui->b_getFolder, SIGNAL(clicked()), MainWindow, SLOT(getFolder()));
	connect(ui->b_Export, SIGNAL(clicked()), MainWindow, SLOT(exportImage()));
	connect(ui->d_scale, SIGNAL(currentIndexChanged(QString)), widget, SLOT(rescale(QString)));
	connect(ui->b_addTiles, SIGNAL(clicked()), MainWindow, SLOT(addTiles()));
	connect(ui->b_removeTiles, SIGNAL(clicked()), MainWindow, SLOT(deleteSelectedTiles()));
	connect(ui->b_swapSizes, SIGNAL(clicked()), MainWindow, SLOT(swapSizes()));
	connect(ui->b_clearTiles, SIGNAL(clicked()), MainWindow, SLOT(clearTiles()));
	connect(ui->s_alphaThreshold, &QSpinBox::valueChanged, MainWindow, SLOT(updateAplhaThreshold()));
*/
	connect(ui->m_new, &QAction::triggered, [this]()
	{
		MainWindow * window = new MainWindow();
		window->show();
	});

	connect(ui->m_open, &QAction::triggered, this, &MainWindow::addTiles);
	connect(ui->m_export, &QAction::triggered, this, &MainWindow::onSave);
	connect(ui->m_about, &QAction::triggered, [this]() {
		QMessageBox::information(this, QString("About %1").arg(windowTitle()),
		"Original by github.com/scriptum\nUpdated by github.com/pdjeeves\nUsed code by Hyllian - sergiogdb@gmail.com");
	});
	connect(ui->m_aboutQt, &QAction::triggered, [this]() { QMessageBox::aboutQt(this); });

//set packer settings
	updateAuto();
}

MainWindow::~MainWindow()
{
    delete ui;
	delete prefs;
}

void MainWindow::setZoomText(float zoom)
{
	m_scale->setText(QString("%1%").arg((int) (zoom*100)));
}

void MainWindow::RecurseDirectory(const QString &dir)
{
    QDir dirEnt(dir);
    QFileInfoList list = dirEnt.entryInfoList();
    for(int i = 0; i < list.count() && !recursiveLoaderDone; i++)
    {
        recursiveLoaderCounter++;
        QFileInfo info = list[i];

        QString filePath = info.filePath();
        QString fileExt = info.suffix().toLower();
        QString name = dir + QDir::separator();
        if(info.isDir())
        {
            // recursive
            if(info.fileName() != ".." && info.fileName() != ".")
            {
                RecurseDirectory(filePath);
            }
        }
        else
            if(g_extensions.contains(fileExt))
            {
                if(!QFile::exists(name + info.completeBaseName() + QString(".atlas")))
                {
                    ui->tilesList->addItem(filePath.replace(topImageDir, ""));
                    packerData *data = new packerData();
                    data->listItem = ui->tilesList->item(ui->tilesList->count() - 1);
					data->file = info.fileName();
                    data->path = info.absoluteFilePath();
                    packer.addItem(data->path, data);
                }
            }
        if(recursiveLoaderCounter == 500)
        {
            if(QMessageBox::No ==
                    QMessageBox::question(
                        this,
                        tr("Directory is too big"),
                        tr("It seems that directory <b>") + topImageDir +
                        tr("</b> is too big. "
                           "Loading may take HUGE amount of time and memory. "
                           "Please, check directory again. <br>"
                           "Do you want to continue?"),
                        QMessageBox::Yes,
                        QMessageBox::No))
            {
                recursiveLoaderDone = true;
                recursiveLoaderCounter++;
                continue;
            }
            ui->c_previewWithImages->setChecked(false);
        }
    }
}

void MainWindow::onSave()
{
    exportImage();
}

void MainWindow::addDir(QString dir)
{
    //FIXME
    //this is messy hack due to difference between QFileDialog and QFileInfo dir separator in Windows
    if(QDir::separator() == '\\')
    {
        topImageDir = dir.replace("\\", "/") + "/";
    }
    else
    {
        topImageDir = dir + "/";
    }


	outDir = dir;

    recursiveLoaderCounter = 0;
    recursiveLoaderDone = false;
    //packer.clear();
    RecurseDirectory(dir);
    QFileInfo info(dir);
	outFile = info.baseName();
}

void MainWindow::addTiles()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                  tr("Select tile directory"), topImageDir);
    if(dir.length() > 0)
    {
        addDir(dir);
        packerUpdate();
    }
}

bool MainWindow::selectTiles(int texture, QRect rect, Qt::KeyboardModifiers keys)
{
    bool selectionChanged = false;

    if(!(keys & Qt::ShiftModifier
    ||   keys & Qt::ControlModifier))
    {
        ui->tilesList->clearSelection();
        selectionChanged =  true;
    }

    for(int i = 0; i < packer.images.size(); ++i)
    {
        if((packer.images[i].duplicateId != NULL && packer.merge)
        ||  packer.images[i].textureId   != texture)
            continue;


        if(packer.images[i].getOutline(packer.cropThreshold).intersects(rect))
        {
            QListWidgetItem * item = packer.images[i].id->listItem;
            if(keys & Qt::ControlModifier)
            {
                selectionChanged =  true;
                item->setSelected(!item->isSelected());
            }
            else if(!item->isSelected())
            {
                selectionChanged =  true;
                item->setSelected(true);
            }
         }
    }

    return selectionChanged;
}


void MainWindow::deleteSelectedTiles()
{
    for(int j = 0; j < packer.images.size(); ++j)
    {
        if(packer.images.at(j).id->listItem
        && packer.images.at(j).id->listItem->isSelected())
        {
            delete packer.images.at(j).id;
            packer.images.removeAt(j);
            --j;
        }
    }

    qDeleteAll(ui->tilesList->selectedItems());
	updateAuto();
}

void MainWindow::displayStatus(const QList<QImage> & textures)
{
	quint64 area = CalculateTotalArea(textures);

	QString message;
	if(area)
	{
		float percent = (((float)packer.area / (float)area) * 100.0f);
		float percent2 = (float)(((float)packer.neededArea / (float)area) * 100.0f);

		QString missed;
		QString merged;

		if(packer.missingImages)
		{
			missed = tr("%1 images missed").arg(QString::number(packer.missingImages));
			missed = QString(", <font color=red><b>%1</b></font>").arg(missed);
		}

		if(packer.mergedImages)
		{
			merged  = tr(", %1 images merged").arg(packer.mergedImages);
		}

		message = tr("%1% filled").arg(percent)
				+ missed
				+ merged
				+ tr(", needed area: %1%").arg(percent2)
				+ tr(", KBytes %3").arg(area * 4 / 1024);
	}

	m_status->setText(message);

}

void MainWindow::packerUpdate()
{
    packer.sortOrder = prefs->sortOrder();
    packer.border = prefs->border();
    packer.extrude = prefs->extrude();
    packer.padding = prefs->padding();
    packer.merge = prefs->doMerge();
    packer.square = prefs->keepSquare();
    packer.autosize = prefs->autosize();
    packer.minFillRate = prefs->minFillRate();
    packer.mergeBF = false;
    packer.rotate = prefs->rotation();

	packer.alignment.setX( prefs->alignX());
	packer.alignment.setY( prefs->alignY());

	packer.cropThreshold      = prefs->alphaThreshold();
	packer.greenScreen        = prefs->greenScreenColor();
    packer.greenScreenToAlpha = prefs->greenScreenToAlpha();
    packer.useGreenScreen     = prefs->useGreenScreen();

	int textureWidth = prefs->maxWidth();
	int textureHeight = prefs->maxHeight();
    Heuristic_t heuristic = prefs->heuristic();

	if(packer.images.size())
	{
		packer.pack(heuristic, textureWidth, textureHeight);
		packerRepaint();
	}
}


void MainWindow::packerRepaint()
{
    bool previewWithImages = ui->c_previewWithImages->isChecked();

    QList<QImage> textures;
    for(int i = 0; i < packer.bins.size(); i++)
    {
        QImage texture(packer.bins.at(i).width(), packer.bins.at(i).height(),
                       QImage::Format_ARGB32);
        texture.fill(Qt::transparent);
        textures << texture;
    }
    if(exporting)
    {
        if(!WriteAtlas(0L, textures, packer, outDir, outFile, "txt"))
			return;
    }

    packer.CreateOutputTextures(textures, previewWithImages, exporting, &pattern);
    packer.DrawOutlines(textures);

	displayStatus(textures);

    if(exporting)
    {
        if(ExportImages(this, textures, outDir, outFile, imgFormat))
		{
			QMessageBox::information(0, tr("Done"), tr("Your atlas successfully saved in ") + outDir);
		}

        exporting = false;
    }
    else
    {
		m_view->updatePixmap(textures);
    }
}

void MainWindow::updateAplhaThreshold()
{
    packer.cropThreshold = prefs->alphaThreshold();
    packer.UpdateCrop();
    updateAuto();
}
/*
void MainWindow::getFolder()
{
    ui->t_outDir->setText(QFileDialog::getExistingDirectory(this,
                        tr("Open Directory"),
                         ui->t_outDir->text(),
                        QFileDialog::ShowDirsOnly));
}*/

void MainWindow::exportImage()
{
    exporting = true;
    outDir = prefs->outDir();
    outFile = prefs->outFile();
    imgFormat = prefs->imgFormat();
    packerUpdate();
}

void MainWindow::clearTiles()
{
    packer.images.clear();
    ui->tilesList->clear();
}

void MainWindow::updateAuto()
{
    if(ui->c_autoUpdate->isChecked())
    {
        packerUpdate();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> droppedUrls = event->mimeData()->urls();
    int droppedUrlCnt = droppedUrls.size();
    for(int i = 0; i < droppedUrlCnt; i++)
    {
        QString localPath = droppedUrls[i].toLocalFile();
        QFileInfo fileInfo(localPath);
        if(fileInfo.isFile())
        {
            ui->tilesList->addItem(fileInfo.fileName());
            packerData *data = new packerData;
            data->listItem = ui->tilesList->item(ui->tilesList->count() - 1);
            data->file = fileInfo.fileName();
			data->path = fileInfo.absoluteFilePath();
            packer.addItem(data->path, data);
            //QMessageBox::information(this, tr("Dropped file"), "Dropping files is not supported yet. Drag and drop directory here.");
        }
        else
            if(fileInfo.isDir())
            {
                addDir(fileInfo.absoluteFilePath());
            }
    }
    packerUpdate();

    event->acceptProposedAction();
}
