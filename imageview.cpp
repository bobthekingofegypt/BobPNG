#include <QtGui>

#include "imageview.h"

ImageView::ImageView(QWidget *parent) : QWidget(parent) {

    checkerPixmap = createCheckerPixmap();

    imageLabel = new QLabel;
    imageLabel->setScaledContents(true);

    mainLayout = new QHBoxLayout;
    mainLayout->addWidget(imageLabel);
    this->setLayout(mainLayout);

    this->resize(0,0);

}


void ImageView::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.drawTiledPixmap(rect(), checkerPixmap);
}


QPixmap ImageView::createCheckerPixmap() {

    QPixmap checkerPixmap = QPixmap(32, 32);

    QPainter checkerPainter(&checkerPixmap);
    checkerPainter.fillRect(checkerPixmap.rect(), QColor(128,128,128));

    QColor alternateColour = QColor(192,192,192);
    checkerPainter.fillRect(0, 0, 16, 16, alternateColour);
    checkerPainter.fillRect(16, 16, 16, 16, alternateColour);

    checkerPainter.end();

    return checkerPixmap;
}


void ImageView::setPixmap(QPixmap pixmap) {
    imageLabel->setPixmap(pixmap);
    this->resize(pixmap.size());
}
