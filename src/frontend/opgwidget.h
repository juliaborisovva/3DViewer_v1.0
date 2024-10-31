#ifndef OPGWIDGET_H
#define OPGWIDGET_H

extern "C" {
#include "../parser.h"
}
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QDragEnterEvent>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QSlider>
#include <QTimer>
#include <QtOpenGL>

class OPGWidget : public QOpenGLWidget {
  Q_OBJECT
 public:
  explicit OPGWidget(QWidget *parent = nullptr);
  ~OPGWidget();
  QString getfilename();
  main_struct obj;
  void setfilename(QString a);
  float getZoomSize();
  void setZoomSize(float newSize);
  float getxMove();
  void setxMove(float newX);
  float getyMove();
  void setyMove(float newY);
  float getzMove();
  void setzMove(float newZ);
  float getxRot();
  void setxRot(float newX);
  float getyRot();
  void setyRot(float newY);
  float getzRot();
  void setzRot(float newZ);
  void setDotSize(float newDotZise);
  int getDotSize();
  void setLineSize(float newDotZise);
  int getLineSize();
  void setSliderZoom(int count);
  int getBoxVertexInd();
  void setBoxVertexInd(int newBoxVertexInd);
  int getBoxFacetsInd();
  void setBoxFacetsInd(int newBoxFacetsInd);
  int getBoxProjectionInd();
  void setBoxProjectionInd(int newBoxFacetsInd);
  QString getFilePath();
  void setFilePath(QString newFilePath);
  QSlider *zoomPointer;
  QDoubleSpinBox *spinPointer[3];
  void setPointColor(float r, float g, float b, float a);
  void setLineColor(float r, float g, float b, float a);
  void setBgColor(float r, float g, float b, float a);
  void getPointColor(float *r, float *g, float *b, float *a);
  void getLineColor(float *r, float *g, float *b, float *a);
  void getBgColor(float *r, float *g, float *b, float *a);
  void setStartFlag(int flag);
  int getStartFlag();
  void setAutorotate(int status);
  int getAutorotate();

 private:
  QString filePath = "";
  QString file_name = "";
  QPoint mPos;
  int boxVertexInd = 2, boxFacetsInd = 0, boxProjectionInd = 0;
  float dotSize = 12, lineSize = 8;

  float rBg = 1.1, gBg = 1.0, bBg = 0.7, aBg = 1;
  float rPoints = 0, gPoints = 0, bPoints = 1, aPoints = 1;
  float rLine = 0, gLine = 1, bLine = 0, aLine = 1;
  int autorotate = 0;
  float xRot = 0, yRot = 0, zRot = 0;
  float xRotOld = 0.0, yRotOld = 0.0;
  float xMove = 0, yMove = 0, zMove = 0;

  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;

  void initializeGL() override;
  void paintGL() override;
  void wheelEvent(QWheelEvent *event) override;
  float zoom_size = 0.1;
  int flag_first_start = 0;

 signals:
  void wheelUpSig(int count);
  void wheelDownSig(int count);
  void nameChange(QString n);

 private slots:
  void wheelUp();
  void wheelDown();
};

#endif  // OPGWIDGET_H
