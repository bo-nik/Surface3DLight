#ifndef SCENE3D_H
#define SCENE3D_H

#include <QGLWidget>
#include <QtWidgets>
#include <QVector3D>
#include <QCamera>
#include <QVideoRendererControl>
#include <cmath>
#include <QDebug>
#include "camvideosurface.h"

class Scene3D : public QGLWidget
{
    enum FrustumType {
        eLeftFrustum,
        eRightFrustum
    };

public:
    Scene3D(QWidget* parent = 0);

public slots:
    void scalePlus();
    void scaleMinus();
    void rotateUp();
    void rotateDown();
    void rotateLeft();
    void rotateRight();
    void translateDown();
    void translateUp();
    void translateLeft();
    void translateRight();
    void defaultScene();

    void lightRotateLeft();
    void lightRotateRight();

protected:
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void wheelEvent(QWheelEvent* e);
    void keyPressEvent(QKeyEvent* e);

private:
    void drawAxis();
    void drawSurface();
    void prepareSurfaceData();
    void applyFrustum(FrustumType ft);
    void applyLighting();

private:
    GLfloat xRot;
    GLfloat yRot;
    GLfloat zRot;
    GLfloat zTra;
    GLfloat xTra;
    GLfloat nSca;

    GLfloat lightRot;

    QPoint ptrMousePosition;

    QVector<QVector<QVector3D> > surfaceVertexes;
    QVector<QVector<QVector3D> > surfaceVertexNormales;

    QCamera *m_camera;
    CamVideoSurface *m_videoSurface;

    GLuint texCam;
};
#endif
