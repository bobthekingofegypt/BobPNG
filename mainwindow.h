#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "imageview.h"

class QAction;
class QMenu;
class QScrollArea;
class QComboBox;
class QCheckBox;

class MainWindow : public QMainWindow {
    Q_OBJECT

private slots:
    void open();
    void saveAs();
    void showOriginal();
    void updateImage();

public:
    MainWindow();

private:
    void createActions();
    void createMenus();
    void createBottomBar();
    void setSizeLabel(int size);
    void executePngquant(int value, bool dither);

    ImageView *imageView;
    QScrollArea *scrollArea;
    QComboBox *numberOfColoursComboBox;
    QCheckBox *ditherCheckBox;
    QWidget *bottomBar;

    QLabel *sizeLabel;

    QMenu *fileMenu;

    QAction *openAction;
    QAction *saveAction;
    QAction *exitAction;

    bool showingOriginal;
    QString activeFilename;
    QPixmap originalPixmap;
    QPixmap modifiedPixmap;

    int originalSize;
};

#endif // MAINWINDOW_H
