#ifndef VIEW_H
#define VIEW_H
#include <QWidget>
#include <QPixmap>
#include <QImage>

class QLabel;
class QVBoxLayout;
class MainWindow;



class View : public QWidget
{
 typedef QWidget super;
        Q_OBJECT

        MainWindow & m_window;
		QVBoxLayout         * m_layout;
		std::vector<QImage>   m_textures;

    public:
        View(MainWindow & m, QWidget *parent = 0);

        void zoomIn();
        void zoomOut();
		float zoom() const { return scale; }
        void changeZoom(float);
		void updatePixmap(const QList<QImage> & image);

    protected:
        void mouseReleaseEvent(QMouseEvent * event);
        void paintEvent(QPaintEvent *event);
        void wheelEvent(QWheelEvent * event);

    private:
	//dump images
		void clear();

        float scale;
        QSize size;

};

#endif // VIEW_H
