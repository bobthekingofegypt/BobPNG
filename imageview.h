#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWidget>

class QLabel;
class QHBoxLayout;
class QPainter;
class QColor;
class QPixmap;

class ImageView : public QWidget {
    Q_OBJECT

protected:
    void paintEvent(QPaintEvent *);

public:
    ImageView(QWidget *parent = 0);
    void setPixmap(QPixmap pixmap);

private:
    QLabel *imageLabel;
    QHBoxLayout *mainLayout;
    QPixmap pixmap;
    QPixmap checkerPixmap;

    QPixmap createCheckerPixmap();
};

#endif // IMAGEVIEW_H
