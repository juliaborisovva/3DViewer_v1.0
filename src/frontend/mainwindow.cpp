#include "mainwindow.h"

#include "../parser.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), gif_timer(new QTimer) {
  ui->setupUi(this);

  connect(gif_timer, SIGNAL(timeout()), this, SLOT(createGif()));
  connect(ui->saveGIF, &QPushButton::clicked, this,
          &MainWindow::on_saveGIF_clicked);

  connect(ui->saveGIF, &QPushButton::clicked, this,
          &MainWindow::on_saveGIF_clicked);

  connect(ui->widget, &OPGWidget::nameChange, this, &MainWindow::getNameChange);
  loadSettings();

  ui->sliderZoom->setSliderPosition(ui->widget->getZoomSize() * 100);
  ui->spinDotSize->setValue(ui->widget->getDotSize());
  ui->spinLineSize->setValue(ui->widget->getLineSize());
  ui->widget->zoomPointer = ui->sliderZoom;
  ui->widget->spinPointer[0] = ui->spinRotX;
  ui->widget->spinPointer[1] = ui->spinRotY;
  ui->widget->spinPointer[2] = ui->spinRotZ;
  QString path = settings->value("File Path", 0).toString();
  ui->statusBar->showMessage("Путь:  " + path);
}

MainWindow::~MainWindow() {
  saveSettings();

  if (pgif) delete pgif;
  delete gif_timer;
  delete ui;
}

void MainWindow::on_fileDialog_clicked() {
  QString file_path = QFileDialog::getOpenFileName(this, "Open a file", "/");
  QByteArray file_name_buf = file_path.toLocal8Bit();
  char *fileName = file_name_buf.data();

  QFile f(fileName);
  QFileInfo fileInfo(f.fileName());
  QString filename(fileInfo.fileName());

  if (*fileName) {
    ui->fileName->setText(filename);
    ui->widget->setFilePath(file_path);
    ui->widget->setfilename(filename);

    drow(file_path);
  }
}

void MainWindow::drow(QString file_path) {
  if (ui->widget->getStartFlag() != 0) {
    remove_matrix(&(ui->widget->obj.matrix_3d));
    remove_massive_of_polygons(&(ui->widget->obj.polygons));
  }
  ui->widget->obj.count_of_vertex = 0;
  ui->widget->obj.count_of_facets = 0;

  QByteArray a = file_path.toLocal8Bit();
  char *aa = a.data();
  first_parcer_of_file(aa, &ui->widget->obj);
  ui->countVertex->setText(QString::number(ui->widget->obj.count_of_vertex));
  ui->countFacets->setText(QString::number(ui->widget->obj.count_of_facets));

  create_matrix(ui->widget->obj.count_of_vertex + 1, 3,
                &ui->widget->obj.matrix_3d);
  create_massive_of_polygons(ui->widget->obj.count_of_facets + 1,
                             &ui->widget->obj.polygons);
  second_parcer_of_file(aa, &ui->widget->obj);
  ui->widget->setStartFlag(1);
  ui->widget->update();
}

void MainWindow::on_spinMoveX_valueChanged(double arg1) {
  ui->widget->setxMove(arg1);
  ui->widget->update();
}

void MainWindow::on_spinMoveY_valueChanged(double arg1) {
  ui->widget->setyMove(arg1);
  ui->widget->update();
}

void MainWindow::on_spinMoveZ_valueChanged(double arg1) {
  ui->widget->setzMove(arg1);
  ui->widget->update();
}

void MainWindow::on_spinRotX_valueChanged(double arg1) {
  ui->widget->setxRot(arg1);
  ui->widget->update();
}

void MainWindow::on_spinRotY_valueChanged(double arg1) {
  ui->widget->setyRot(arg1);
  ui->widget->update();
}

void MainWindow::on_spinRotZ_valueChanged(double arg1) {
  ui->widget->setzRot(arg1);
  ui->widget->update();
}

void MainWindow::on_bgColor_clicked() {
  float r, g, b, a;
  ui->widget->getBgColor(&r, &g, &b, &a);
  QColor tmp;
  tmp.setRgbF(r, g, b, a);
  QColor color = QColorDialog::getColor(tmp);
  color.getRgbF(&r, &g, &b, &a);
  ui->widget->setBgColor(r, g, b, a);
}

void MainWindow::on_facetColor_clicked() {
  float r, g, b, a;
  ui->widget->getLineColor(&r, &g, &b, &a);
  QColor tmp;
  tmp.setRgbF(r, g, b, a);
  QColor color = QColorDialog::getColor(tmp);
  color.getRgbF(&r, &g, &b, &a);
  ui->widget->setLineColor(r, g, b, a);
}

void MainWindow::on_vertexColor_clicked() {
  float r, g, b, a;
  ui->widget->getPointColor(&r, &g, &b, &a);
  QColor tmp;
  tmp.setRgbF(r, g, b, a);
  QColor color = QColorDialog::getColor(tmp);
  color.getRgbF(&r, &g, &b, &a);
  ui->widget->setPointColor(r, g, b, a);
}

void MainWindow::on_spinDotSize_valueChanged(double arg1) {
  ui->widget->setDotSize(arg1);
  ui->widget->update();
}

void MainWindow::on_sliderZoom_valueChanged(int value) {
  ui->widget->setZoomSize((float)value / 100);
  ui->widget->update();
}

void MainWindow::on_spinLineSize_valueChanged(double arg1) {
  ui->widget->setLineSize(arg1);
  ui->widget->update();
}

void MainWindow::on_boxVertex_currentIndexChanged(int arg1) {
  ui->widget->setBoxVertexInd(arg1);
  ui->widget->update();
}

void MainWindow::on_boxFacets_currentIndexChanged(int arg1) {
  ui->widget->setBoxFacetsInd(arg1);
  ui->widget->update();
}

void MainWindow::on_boxProjection_currentIndexChanged(int arg1) {
  ui->widget->setBoxProjectionInd(arg1);
  ui->widget->update();
}

void MainWindow::on_autorotate_stateChanged(int arg1) {
  ui->widget->setAutorotate(arg1);
  ui->widget->update();
}

void MainWindow::on_saveJPEG_clicked() {
  QImage img = ui->widget->grabFramebuffer();
  QString dirname =
      QFileDialog::getSaveFileName(this, "Выбeрите папку", "/", ".jpeg");
  img.save(dirname + ".jpeg", "JPEG");
}

void MainWindow::on_saveBMP_clicked() {
  QImage img = ui->widget->grabFramebuffer();
  QString dirname =
      QFileDialog::getSaveFileName(this, "Выбeрите папку", "/", ".bmp");
  img.save(dirname + ".bmp", "BMP");
}

void MainWindow::createGif() {
  if (pgif->frameCount() < 50) {
    switch (pgif->frameCount()) {
      case 10:
        ui->saveGIF->setText("5 sec. left");
        break;
      case 20:
        ui->saveGIF->setText("4 sec. left");
        break;
      case 30:
        ui->saveGIF->setText("3 sec. left");
        break;
      case 40:
        ui->saveGIF->setText("2 sec. left");
        break;
      case 49:
        ui->saveGIF->setText("1 sec. left");
        break;
    }
    QImage frame = ui->widget->grabFramebuffer();
    pgif->addFrame(frame.scaled(640, 480));
    return;
  }
  gif_timer->stop();
  ui->saveGIF->setText("Гифка");
  QString dirname =
      QFileDialog::getSaveFileName(this, "Выбeрите папку", "/", ".gif");
  pgif->save(dirname + ".gif");
}

void MainWindow::on_saveGIF_clicked() {
  if (ui->saveGIF->text() == "Гифка") {
    ui->saveGIF->setText("Запись...");
    pgif = new QGifImage;
    pgif->setDefaultDelay(100);
    gif_timer->start(100);
  }
}

void MainWindow::saveSettings() {
  settings->setValue("Type Point", ui->widget->getBoxVertexInd());
  settings->setValue("Type Line", ui->widget->getBoxFacetsInd());
  settings->setValue("Type Projection", ui->widget->getBoxProjectionInd());
  settings->setValue("Dot Size", ui->widget->getDotSize());
  settings->setValue("Line Size", ui->widget->getLineSize());
  settings->setValue("Zoom", ui->widget->getZoomSize());

  float colorSet[4];
  ui->widget->getPointColor(&colorSet[0], &colorSet[1], &colorSet[2],
                            &colorSet[3]);

  for (int i = 0; i < 4; i++) {
    QString tmp = "Point Color " + QString::number(i);
    settings->setValue(tmp, colorSet[i]);
  }

  ui->widget->getBgColor(&colorSet[0], &colorSet[1], &colorSet[2],
                         &colorSet[3]);

  for (int i = 0; i < 4; i++) {
    QString tmp = "Bg Color " + QString::number(i);
    settings->setValue(tmp, colorSet[i]);
  }

  ui->widget->getLineColor(&colorSet[0], &colorSet[1], &colorSet[2],
                           &colorSet[3]);

  for (int i = 0; i < 4; i++) {
    QString tmp = "Line Color " + QString::number(i);
    settings->setValue(tmp, colorSet[i]);
  }

  settings->setValue("Move X", ui->widget->getxMove());
  settings->setValue("Move Y", ui->widget->getyMove());
  settings->setValue("Move Z", ui->widget->getzMove());

  settings->setValue("Rot X", ui->widget->getxRot());
  settings->setValue("Rot Y", ui->widget->getyRot());
  settings->setValue("Rot Z", ui->widget->getzRot());

  settings->setValue("File Path", ui->widget->getFilePath());

  settings->setValue("File Name", ui->fileName->text());
}

void MainWindow::loadSettings() {
  settings = new QSettings("3DView", "3DViewer Settings", this);

  if (settings->status() == QSettings::NoError) {
    ui->boxFacets->setCurrentIndex(settings->value("Type Line", 0).toInt());
    ui->boxVertex->setCurrentIndex(settings->value("Type Point", 0).toInt());
    ui->boxProjection->setCurrentIndex(
        settings->value("Type Projection", 0).toInt());

    ui->spinDotSize->setValue(settings->value("Dot Size", 0).toFloat());
    ui->spinLineSize->setValue(settings->value("Line Size", 0).toFloat());
    ui->widget->setZoomSize(settings->value("Zoom", 0).toFloat());

    QString s = settings->value("File Path", 0).toString();
    QFile a = s;
    if (a.exists()) {
      drow(settings->value("File Path", 0).toString());
      ui->widget->setFilePath(s);
      ui->statusBar->showMessage("Путь:  " + s);
      ui->fileName->setText(settings->value("File Name", 0).toString());
    }

    float colorSet[4];
    for (int i = 0; i < 4; i++) {
      QString tmp = "Point Color " + QString::number(i);
      colorSet[i] = settings->value(tmp, 0).toFloat();
    }

    ui->widget->setPointColor(colorSet[0], colorSet[1], colorSet[2],
                              colorSet[3]);
    for (int i = 0; i < 4; i++) {
      QString tmp = "Bg Color " + QString::number(i);
      colorSet[i] = settings->value(tmp, 0).toFloat();
    }
    ui->widget->setBgColor(colorSet[0], colorSet[1], colorSet[2], colorSet[3]);
    for (int i = 0; i < 4; i++) {
      QString tmp = "Line Color " + QString::number(i);
      colorSet[i] = settings->value(tmp, 0).toFloat();
    }
    ui->widget->setLineColor(colorSet[0], colorSet[1], colorSet[2],
                             colorSet[3]);

    ui->widget->setxRot(settings->value("Move X", 0).toFloat());
    ui->widget->setyRot(settings->value("Move Y", 0).toFloat());
    ui->widget->setzRot(settings->value("Move Z", 0).toFloat());
    ui->spinMoveX->setValue(settings->value("Move X", 0).toFloat());
    ui->spinMoveY->setValue(settings->value("Move Y", 0).toFloat());
    ui->spinMoveZ->setValue(settings->value("Move Z", 0).toFloat());

    ui->widget->setxRot(settings->value("Rot X", 0).toFloat());
    ui->widget->setyRot(settings->value("Rot Y", 0).toFloat());
    ui->widget->setzRot(settings->value("Rot Z", 0).toFloat());
    ui->spinRotX->setValue(settings->value("Rot X", 0).toFloat());
    ui->spinRotY->setValue(settings->value("Rot Y", 0).toFloat());
    ui->spinRotZ->setValue(settings->value("Rot Z", 0).toFloat());
  }
}

void MainWindow::getNameChange(QString newName) {
  ui->widget->setFilePath(newName);

  QFile f(newName);
  QFileInfo fileInfo(f.fileName());
  QString fileName(fileInfo.fileName());

  QString path = ui->widget->getFilePath();
  ui->statusBar->showMessage("Путь:  " + path);

  ui->fileName->setText(fileName);
  drow(newName);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
  if (event->type() == QMouseEvent::MouseButtonPress) {
    ui->statusBar->showMessage("У самурая нет цели, только путь.");
  }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
  if (event->type() == QMouseEvent::MouseButtonRelease) {
    QString path = ui->widget->getFilePath();
    ui->statusBar->showMessage("Путь:  " + path);
  }
}
