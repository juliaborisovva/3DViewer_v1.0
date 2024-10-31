#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <QSettings>
#include <QTimer>

#include "opgwidget.h"
#include "qgifimage.h"

extern "C" {
#include "../parser.h"
}

QT_BEGIN_NAMESPACE namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private slots:
  void on_fileDialog_clicked();

  void on_spinMoveX_valueChanged(double arg1);

  void on_spinMoveY_valueChanged(double arg1);

  void on_spinMoveZ_valueChanged(double arg1);

  void on_spinRotX_valueChanged(double arg1);

  void on_spinRotY_valueChanged(double arg1);

  void on_spinRotZ_valueChanged(double arg1);

  void on_facetColor_clicked();

  void on_vertexColor_clicked();

  void on_sliderZoom_valueChanged(int value);

  void on_spinLineSize_valueChanged(double arg1);

  void on_boxVertex_currentIndexChanged(int index);

  void on_boxFacets_currentIndexChanged(int index);

  void on_boxProjection_currentIndexChanged(int index);

  void on_spinDotSize_valueChanged(double arg1);

  void on_bgColor_clicked();

  void on_autorotate_stateChanged(int arg1);

  void on_saveJPEG_clicked();

  void on_saveBMP_clicked();

  void createGif();

  void drow(QString file_name);

  void on_saveGIF_clicked();

  void saveSettings();

  void loadSettings();

  void mousePressEvent(QMouseEvent *);

  void mouseReleaseEvent(QMouseEvent *);

 private:
  Ui::MainWindow *ui;
  QSettings *settings;
  QGifImage *pgif;
  QTimer *gif_timer;

 public slots:
  void getNameChange(QString newName);
};

#endif  // MAINWINDOW_H
