#include <QtGui>
#include <QtCore>

#include "mainwindow.h"


MainWindow::MainWindow() {

    createActions();
    createMenus();
    createBottomBar();

    QWidget *centralWidget = new QWidget;
    QVBoxLayout *backgroundLayout = new QVBoxLayout;
    centralWidget->setLayout(backgroundLayout);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Mid);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setFrameShape(QFrame::NoFrame);

    imageView = new ImageView;
    scrollArea->setWidget(imageView);

    backgroundLayout->addWidget(scrollArea);
    backgroundLayout->addWidget(bottomBar);

    setCentralWidget(centralWidget);

    showingOriginal = false;

    resize(800,800);
    setWindowTitle(tr("BobPNG"));
}


void MainWindow::createActions() {

    openAction = new QAction(tr("&Open..."), this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing image file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    saveAction = new QAction(tr("&Save as"), this);
    saveAction->setShortcut(tr("Ctrl+S"));
    saveAction->setStatusTip(tr("Save the image"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveAs()));
}


void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(exitAction);
}


void MainWindow::createBottomBar() {
    bottomBar = new QWidget;
    bottomBar->setVisible(false);

    QHBoxLayout *bottomBarLayout = new QHBoxLayout;
    bottomBar->setLayout(bottomBarLayout);

    sizeLabel = new QLabel("");
    bottomBarLayout->addWidget(sizeLabel);

    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    bottomBarLayout->addItem(spacerItem);

    numberOfColoursComboBox = new QComboBox;
    QObject::connect(numberOfColoursComboBox, SIGNAL(activated(int)), this, SLOT(updateImage()));

    numberOfColoursComboBox->addItem("256", QVariant(256));
    numberOfColoursComboBox->addItem("128", QVariant(128));
    numberOfColoursComboBox->addItem("64", QVariant(64));
    numberOfColoursComboBox->addItem("32", QVariant(32));
    numberOfColoursComboBox->addItem("16", QVariant(16));
    numberOfColoursComboBox->addItem("8", QVariant(8));
    numberOfColoursComboBox->addItem("4", QVariant(4));
    numberOfColoursComboBox->addItem("2", QVariant(2));

    bottomBarLayout->addWidget(numberOfColoursComboBox);


    ditherCheckBox = new QCheckBox(tr("Dither"));
    QObject::connect(ditherCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateImage()));

    bottomBarLayout->addWidget(ditherCheckBox);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    bottomBarLayout->addWidget(line);

    QPushButton *originalButton = new QPushButton(tr("Original"));
    connect(originalButton, SIGNAL(clicked()), this, SLOT(showOriginal()));

    bottomBarLayout->addWidget(originalButton);
}


void MainWindow::open() {
    QString selectionfilter = tr("PNG (*.png)");
    QString fileName = QFileDialog::getOpenFileName(
             this,
             "Select image to open",
             QDir().homePath(),
             tr("PNG (*.png)" ),
             &selectionfilter
    );

    if (!fileName.isEmpty()) {
        activeFilename = fileName;

        originalPixmap = QPixmap(fileName);

        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        originalPixmap.save(&buffer, "PNG");

        originalSize = bytes.size();

        executePngquant(256, false);

        bottomBar->setVisible(true);
    }
}

void MainWindow::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this);
    if (QFile::exists(fileName)) {
        int r = QMessageBox::warning(this, tr("Image Pro"),
                        tr("File %1 already exists.\n"
                           "Do you want to overwrite it?")
                        .arg(QDir::toNativeSeparators(fileName)),
                        QMessageBox::Yes | QMessageBox::No);
        if (r == QMessageBox::No)
            return;
    }
    if (!fileName.isEmpty()) {
        modifiedPixmap.save(fileName, "PNG");
    }
}

void MainWindow::showOriginal() {
    showingOriginal = !showingOriginal;

    if (showingOriginal) {
        imageView->setPixmap(originalPixmap);
    } else {
        imageView->setPixmap(modifiedPixmap);
    }

    numberOfColoursComboBox->setEnabled(!showingOriginal);
    ditherCheckBox->setEnabled(!showingOriginal);
}

void MainWindow::updateImage() {
    bool dither = ditherCheckBox->isChecked();

    int value = numberOfColoursComboBox->itemData(numberOfColoursComboBox->currentIndex()).toInt();

    executePngquant(value, dither);
}

void MainWindow::executePngquant(int value, bool dither) {

    QStringList args;
    args << QString::number(value);
    args << (dither ? "-fs" : "-nofs");
    args << "--speed";
    args << "3";

    QStringList catArgs;
    catArgs << activeFilename;

    QProcess pngquant;
    QProcess cat;

    cat.setStandardOutputProcess(&pngquant);

    cat.start("cat", catArgs);

    pngquant.start("pngquant", args);
    pngquant.waitForFinished(-1);

    QByteArray processOutput = pngquant.readAll();

    pngquant.close();
    cat.close();

    modifiedPixmap = QPixmap();
    modifiedPixmap.loadFromData(processOutput);

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    modifiedPixmap.save(&buffer, "PNG");

    setSizeLabel(bytes.size());

    imageView->setPixmap(modifiedPixmap);
}

void MainWindow::setSizeLabel(int size) {
    sizeLabel->setText(QString("Size: %1 kb (%2%)").arg(size/1024).arg((int)(((float)size/originalSize)*100)));
}
