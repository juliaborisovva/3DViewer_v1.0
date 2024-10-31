#include "opgwidget.h"

#include <QDir>

OPGWidget::OPGWidget(QWidget *parent) : QOpenGLWidget(parent) {
  QObject::connect(this, SIGNAL(wheelUpSig(int)), this, SLOT(wheelUp()));
  QObject::connect(this, SIGNAL(wheelDownSig(int)), this, SLOT(wheelDown()));

  setAcceptDrops(true);
  this->obj.count_of_facets = 0;
  this->obj.count_of_vertex = 0;
}

OPGWidget::~OPGWidget() {
  if (this->flag_first_start != 0) {
    remove_matrix(&(this->obj.matrix_3d));
    remove_massive_of_polygons(&(this->obj.polygons));
  }
}

void OPGWidget::initializeGL() {
  glClearColor(rBg, gBg, bBg, aBg);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
}

void OPGWidget::paintGL() {
  glClearColor(rBg, gBg, bBg, aBg);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

  float zoom_size = this->getZoomSize();
  glScalef(zoom_size, zoom_size, zoom_size);

  glTranslatef(xMove, yMove, zMove);

  // вращение по осям
  if (autorotate) {
    yRot -= 0.1;
    xRot -= 0.1;
    glRotatef(20, xRot, yRot, zRot);
    this->spinPointer[0]->setValue(xRot);
    this->spinPointer[1]->setValue(yRot);
    this->spinPointer[2]->setValue(zRot);
  }

  //тут кручение мышОй
  glRotatef(xRot, 1, 0, 0);
  glRotatef(yRot, 0, 1, 0);

  if (this->flag_first_start != 0) {
    glVertexPointer(3, GL_FLOAT, 0, this->obj.matrix_3d.matrix);
  }
  glEnableClientState(GL_VERTEX_ARRAY);

  // сюда прикрутить проекции
  // - управление видом проекции
  if (this->boxProjectionInd == 0)
    glFrustum(-2, 2, -2, 2, -5, 100);
  else if (this->boxProjectionInd == 1)
    glOrtho(-2, 2, -2, 2, 5, 1);
  //    glFrustum(-2, 2, -2, 2, -100, 100);

  // points
  glPointSize(this->dotSize);
  if (this->boxVertexInd == 2) {
    glEnable(GL_POINT_SMOOTH);
  } else if (this->boxVertexInd == 1) {
    glDisable(GL_POINT_SMOOTH);
  }

  if (this->boxVertexInd != 0) {
    glColor4f(rPoints, gPoints, bPoints, aPoints);
    if (flag_first_start != 0) {
      glDrawArrays(GL_POINTS, 1, this->obj.count_of_vertex);
    }
  }

  // lines
  glLineWidth(this->lineSize);

  if (this->boxFacetsInd == 0) {
    glDisable(GL_LINE_STIPPLE);
  } else if (this->boxFacetsInd == 1) {
    glLineStipple(this->lineSize, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
  }

  glColor4f(rLine, gLine, bLine, aLine);
  if (this->flag_first_start != 0) {
    for (unsigned int i = 1; i < this->obj.count_of_facets + 1; i++) {
      glDrawElements(GL_LINES, this->obj.polygons[i].num_of_vertex_in_facets,
                     GL_UNSIGNED_INT, this->obj.polygons[i].vertex);
    }
  }
}

void OPGWidget::dragEnterEvent(QDragEnterEvent *event) { event->accept(); }
void OPGWidget::dragLeaveEvent(QDragLeaveEvent *event) { event->accept(); }
void OPGWidget::dragMoveEvent(QDragMoveEvent *event) { event->accept(); }
void OPGWidget::dropEvent(QDropEvent *event) {
  {
    QStringList accepted_types;
    accepted_types << "obj";
    foreach (const QUrl &url, event->mimeData()->urls()) {
      QString fname = url.toLocalFile();
      QFileInfo info(fname);
      if (info.exists()) {
        if (accepted_types.contains(info.suffix().trimmed(),
                                    Qt::CaseInsensitive))
          emit nameChange(fname);
      }
    }
  }
}

void OPGWidget::mousePressEvent(QMouseEvent *mo) {
  mPos = mo->pos();
  xRotOld = xRot;
  yRotOld = yRot;
}

void OPGWidget::mouseMoveEvent(QMouseEvent *mo) {
  xRot = (-1 / M_PI * (mo->pos().y() - mPos.y()));
  yRot = (-1 / M_PI * (mo->pos().x() - mPos.x()));

  xRot += xRotOld;
  yRot += yRotOld;

  this->spinPointer[0]->setValue(xRot);
  this->spinPointer[1]->setValue(yRot);
  update();
}

QString OPGWidget::getfilename() { return this->file_name; }

void OPGWidget::setfilename(QString a) { this->file_name = a; }

void OPGWidget::wheelEvent(QWheelEvent *event) {
  QPoint numDegrees = event->angleDelta() / 8;
  if (numDegrees.y() > 0) {
    emit wheelUpSig(numDegrees.y());
  } else {
    emit wheelDownSig(numDegrees.y());
  }
}

void OPGWidget::wheelUp() {
  this->setZoomSize(this->getZoomSize() + 0.02);
  this->zoomPointer->setSliderPosition(this->getZoomSize() * 100);
  update();
}

void OPGWidget::wheelDown() {
  this->setZoomSize(this->getZoomSize() - 0.02);
  this->zoomPointer->setSliderPosition(this->getZoomSize() * 100);
  update();
}

float OPGWidget::getZoomSize() { return this->zoom_size; }

void OPGWidget::setZoomSize(float newSize) {
  if (newSize <= 1 && newSize >= 0) {
    this->zoom_size = newSize;
  }
}

void OPGWidget::setPointColor(float r, float g, float b, float a) {
  this->rPoints = r;
  this->gPoints = g;
  this->bPoints = b;
  this->aPoints = a;
  update();
}

void OPGWidget::setLineColor(float r, float g, float b, float a) {
  this->rLine = r;
  this->gLine = g;
  this->bLine = b;
  this->aLine = a;
  update();
}

void OPGWidget::setBgColor(float r, float g, float b, float a) {
  this->rBg = r;
  this->gBg = g;
  this->bBg = b;
  this->aBg = a;
  update();
}

float OPGWidget::getxMove() { return this->xMove; }

void OPGWidget::setxMove(float newX) { this->xMove = newX; }

float OPGWidget::getyMove() { return this->yMove; }

void OPGWidget::setyMove(float newY) { this->yMove = newY; }

float OPGWidget::getzMove() { return this->zMove; }

void OPGWidget::setzMove(float newZ) { this->zMove = newZ; }

float OPGWidget::getxRot() { return this->xRot; }

void OPGWidget::setxRot(float newX) { this->xRot = newX; }

float OPGWidget::getyRot() { return this->yRot; }

void OPGWidget::setyRot(float newY) { this->yRot = newY; }

float OPGWidget::getzRot() { return this->zRot; }

void OPGWidget::setzRot(float newZ) { this->zRot = newZ; }

void OPGWidget::setLineSize(float newLineZise) { this->lineSize = newLineZise; }

int OPGWidget::getLineSize() { return this->lineSize; }

void OPGWidget::setDotSize(float newDotZise) { this->dotSize = newDotZise; }

int OPGWidget::getDotSize() { return this->dotSize; }

void OPGWidget::setSliderZoom(int count) {
  this->setZoomSize(this->getZoomSize() + count);
}

int OPGWidget::getBoxVertexInd() { return this->boxVertexInd; }

void OPGWidget::setBoxVertexInd(int newBoxVertexInd) {
  this->boxVertexInd = newBoxVertexInd;
}

int OPGWidget::getBoxFacetsInd() { return this->boxFacetsInd; }

void OPGWidget::setBoxFacetsInd(int newBoxFacetsInd) {
  this->boxFacetsInd = newBoxFacetsInd;
}

int OPGWidget::getBoxProjectionInd() { return this->boxProjectionInd; }

void OPGWidget::setBoxProjectionInd(int newBoxProjectionInd) {
  this->boxProjectionInd = newBoxProjectionInd;
}

void OPGWidget::getPointColor(float *r, float *g, float *b, float *a) {
  *r = this->rPoints;
  *g = this->gPoints;
  *b = this->bPoints;
  *a = this->aPoints;
}
void OPGWidget::getLineColor(float *r, float *g, float *b, float *a) {
  *r = this->rLine;
  *g = this->gLine;
  *b = this->bLine;
  *a = this->aLine;
}
void OPGWidget::getBgColor(float *r, float *g, float *b, float *a) {
  *r = this->rBg;
  *g = this->gBg;
  *b = this->bBg;
  *a = this->aBg;
}

void OPGWidget::setStartFlag(int flag) { this->flag_first_start = flag; }

int OPGWidget::getStartFlag() { return this->flag_first_start; }

void OPGWidget::setAutorotate(int status) { this->autorotate = status; }

int OPGWidget::getAutorotate() { return this->autorotate; }

QString OPGWidget::getFilePath() { return this->filePath; }

void OPGWidget::setFilePath(QString newFilePath) {
  this->filePath = newFilePath;
}
