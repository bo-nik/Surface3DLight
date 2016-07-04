#include "scene3D.h"

Scene3D::Scene3D(QWidget* parent) : QGLWidget(parent)
{
    xRot = -60;
    yRot = 0;
    zRot = 45;
    xTra = 0;
    zTra = 0;
    nSca = 0.7;
    lightRot = 0;

    prepareSurfaceData();

    m_camera = new QCamera;
    m_camera->setCaptureMode(QCamera::CaptureVideo);

    m_videoSurface = new CamVideoSurface(this);

    QVideoRendererControl *rendererControl = m_camera->service()->requestControl<QVideoRendererControl *>();
    rendererControl->setSurface(m_videoSurface);

    texCam = 0;

    connect(m_videoSurface, &CamVideoSurface::newImage, [=](QImage img) {
        img = QGLWidget::convertToGLFormat(img);

//        uchar blackText[256][256][3];
//        for (int i = 0; i < 256; i++) {
//            for (int j = 0; j < 256; j++) {
//                blackText[i][j][0] = 0;
//                blackText[i][j][1] = 0;
//                blackText[i][j][2] = 0;
//            }
//        }

        glBindTexture(GL_TEXTURE_2D, texCam);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, blackText);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        update();
    });

    m_camera->start();
}

void Scene3D::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texCam);
    glBindTexture(GL_TEXTURE_2D, texCam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
}

void Scene3D::resizeGL(int /*nWidth*/, int /*nHeight*/)
{

}

void Scene3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);

    // draw cam image
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -20.0, 20000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(-1, 1, 1);

    glBindTexture(GL_TEXTURE_2D, texCam);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 1); glVertex3f(-1.0f,  1.0f, -100);
        glTexCoord2f(0, 0); glVertex3f(-1.0f, -1.0f, -100);
        glTexCoord2f(1, 0); glVertex3f( 1.0f, -1.0f, -100);

        glTexCoord2f(0, 1); glVertex3f(-1.0f,  1.0f, -100);
        glTexCoord2f(1, 0); glVertex3f( 1.0f, -1.0f, -100);
        glTexCoord2f(1, 1); glVertex3f( 1.0f,  1.0f, -100);
    glEnd();


    glDisable(GL_TEXTURE_2D);

    applyLighting();

    applyFrustum(eLeftFrustum);
    glColorMask(true, false, false, false);
    drawSurface();

    glClear(GL_DEPTH_BUFFER_BIT);

    applyFrustum(eRightFrustum);
    glColorMask(false, false, true, false);
    drawSurface();

    glColorMask(true, true, true, true);
}

void Scene3D::applyLighting()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(lightRot, 0.0f, 1.0f, 0.0f);

    GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light0_position[] = {50.0, 50.0, 0.0, 1};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
}

void Scene3D::drawAxis()
{
    glLineWidth(3.0f);

    glColor3f(1.00f, 0.00f, 0.00f);
    glBegin(GL_LINES);
    glVertex3f( 1.0f,  0.0f,  0.0f);
    glVertex3f(-1.0f,  0.0f,  0.0f);
    glEnd();

    glColor3f(0.00f, 1.00f, 0.00f);
    glBegin(GL_LINES);
    glVertex3f( 0.0f,  1.0f,  0.0f);
    glVertex3f( 0.0f, -1.0f,  0.0f);
    glEnd();

    glColor3f(0.00f, 0.00f, 1.00f);
    glBegin(GL_LINES);
    glVertex3f( 0.0f,  0.0f,  1.0f);
    glVertex3f( 0.0f,  0.0f, -1.0f);
    glEnd();
}

void Scene3D::drawSurface()
{
//    glColor3f(0.55f, 0.92f, 0.32f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < surfaceVertexes.count() - 1; i++) {
            for (int j = 0; j < surfaceVertexes[i].count() - 1; j++) {
                glNormal3f(surfaceVertexNormales[i][j].x(), surfaceVertexNormales[i][j].y(), surfaceVertexNormales[i][j].z());
                glVertex3f(surfaceVertexes[i][j].x(), surfaceVertexes[i][j].y(), surfaceVertexes[i][j].z());
                glNormal3f(surfaceVertexNormales[i + 1][j].x(), surfaceVertexNormales[i + 1][j].y(), surfaceVertexNormales[i + 1][j].z());
                glVertex3f(surfaceVertexes[i + 1][j].x(), surfaceVertexes[i + 1][j].y(), surfaceVertexes[i + 1][j].z());
                glNormal3f(surfaceVertexNormales[i + 1][j + 1].x(), surfaceVertexNormales[i + 1][j + 1].y(), surfaceVertexNormales[i + 1][j + 1].z());
                glVertex3f(surfaceVertexes[i + 1][j + 1].x(), surfaceVertexes[i + 1][j + 1].y(), surfaceVertexes[i + 1][j + 1].z());

                glNormal3f(surfaceVertexNormales[i][j].x(), surfaceVertexNormales[i][j].y(), surfaceVertexNormales[i][j].z());
                glVertex3f(surfaceVertexes[i][j].x(), surfaceVertexes[i][j].y(), surfaceVertexes[i][j].z());
                glNormal3f(surfaceVertexNormales[i + 1][j + 1].x(), surfaceVertexNormales[i + 1][j + 1].y(), surfaceVertexNormales[i + 1][j + 1].z());
                glVertex3f(surfaceVertexes[i + 1][j + 1].x(), surfaceVertexes[i + 1][j + 1].y(), surfaceVertexes[i + 1][j + 1].z());
                glNormal3f(surfaceVertexNormales[i][j + 1].x(), surfaceVertexNormales[i][j + 1].y(), surfaceVertexNormales[i][j + 1].z());
                glVertex3f(surfaceVertexes[i][j + 1].x(), surfaceVertexes[i][j + 1].y(), surfaceVertexes[i][j + 1].z());
            }
        }
    glEnd();

    glLineWidth(1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < surfaceVertexes.count() - 1; i++) {
        for (int j = 0; j < surfaceVertexes[i].count() - 1; j++) {
            glBegin(GL_LINE_LOOP);
            glVertex3f(surfaceVertexes[i][j].x(), surfaceVertexes[i][j].y(), surfaceVertexes[i][j].z() + 0.01);
            glVertex3f(surfaceVertexes[i + 1][j].x(), surfaceVertexes[i + 1][j].y(), surfaceVertexes[i + 1][j].z() + 0.01);
            glVertex3f(surfaceVertexes[i + 1][j + 1].x(), surfaceVertexes[i + 1][j + 1].y(), surfaceVertexes[i + 1][j + 1].z() + 0.01);
            glVertex3f(surfaceVertexes[i][j + 1].x(), surfaceVertexes[i][j + 1].y(), surfaceVertexes[i][j + 1].z() + 0.01);
            glEnd();
        }
    }
}

void Scene3D::mousePressEvent(QMouseEvent* e)
{
    ptrMousePosition = e->pos();
}

void Scene3D::mouseMoveEvent(QMouseEvent* e)
{
    xRot += 180 / nSca * (GLfloat)(e->y() - ptrMousePosition.y()) / height();
    zRot += 180 / nSca * (GLfloat)(e->x() - ptrMousePosition.x()) / width();

    ptrMousePosition = e->pos();

    updateGL();
}

void Scene3D::wheelEvent(QWheelEvent* e)
{
//    if (e->delta() > 0) {
//        scalePlus();
//    } else if (e->delta() < 0) {
//        scaleMinus();
//    }

//    updateGL();
}

void Scene3D::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
    case Qt::Key_Plus:
        scalePlus();
        break;
    case Qt::Key_Equal:
        scalePlus();
        break;
    case Qt::Key_Minus:
        scaleMinus();
        break;
    case Qt::Key_Down:
        rotateDown();
        break;
    case Qt::Key_Up:
        rotateUp();
        break;
    case Qt::Key_Left:
        rotateLeft();
        break;
    case Qt::Key_Right:
        rotateRight();
        break;
    case Qt::Key_S:
        translateUp();
        break;
    case Qt::Key_W:
        translateDown();
        break;
    case Qt::Key_A:
        translateLeft();
        break;
    case Qt::Key_D:
        translateRight();
        break;
    case Qt::Key_1:
        lightRotateLeft();
        break;
    case Qt::Key_2:
        lightRotateRight();
        break;
    case Qt::Key_Space:
        defaultScene();
        break;
    case Qt::Key_Escape:
        this->close();
        break;
    }

    updateGL();
}

void Scene3D::scalePlus()
{
    nSca = nSca * 1.1;
}

void Scene3D::scaleMinus()
{
    nSca = nSca / 1.1;
}

void Scene3D::rotateUp()
{
    xRot -= 1.0;
}

void Scene3D::rotateDown()
{
    xRot += 1.0;
}

void Scene3D::rotateLeft()
{
    zRot -= 1.0;
}

void Scene3D::rotateRight()
{
    zRot += 1.0;
}

void Scene3D::translateDown()
{
    zTra += 0.05;
}

void Scene3D::translateUp()
{
    zTra -= 0.05;
}

void Scene3D::translateLeft()
{
    xTra -= 0.05;
}

void Scene3D::translateRight()
{
    xTra += 0.05;
}

void Scene3D::defaultScene()
{
    xRot = -60;
    yRot = 0;
    zRot = 45;
    xTra = 0;
    zTra = 0;
    nSca = 0.7;
    lightRot = 0;
}

void Scene3D::lightRotateLeft()
{
    lightRot -= 5.0;
}

void Scene3D::lightRotateRight()
{
    lightRot += 5.0;
}

void Scene3D::prepareSurfaceData()
{
    surfaceVertexes.clear();

    float l = 0.8;
    float a = 0.7;
    float f = 0.4;
    float d = 0.05;
    for (float u = -1; u <= 1.00001; u += d) {
        QVector<QVector3D> uLine;
        for (float v = -1; v <= 1.00001; v += d) {
            uLine.push_back(QVector3D(u,
                                      v,
                                      u / l * (a * cosh(v / a) - a - f)));
        }
        surfaceVertexes.push_back(uLine);
    }

    // normales //////////

    auto surfaceNormale = [](const QVector3D &p1, const QVector3D &p2, const QVector3D &p3) -> QVector3D {
        QVector3D edge1(p2.x() - p1.x(), p2.y() - p1.y(), p2.z() - p1.z());
        QVector3D edge2(p3.x() - p1.x(), p3.y() - p1.y(), p3.z() - p1.z());
        QVector3D n(edge1.y() * edge2.z() - edge1.z() * edge2.y(),
                    edge1.x() * edge2.z() - edge1.z() * edge2.x(),
                    edge1.x() * edge2.y() - edge1.y() * edge2.x());
        qreal nLen = sqrt(n.x() * n.x() + n.y() * n.y() + n.z() * n.z());
        n = QVector3D(n.x() / nLen, n.y() / nLen, n.z() / nLen);
        return n;
    };

    auto vertexNormale = [](const QList<QVector3D> &surfaceNormales) -> QVector3D {
        QVector3D n;
        foreach (QVector3D surfaceN, surfaceNormales) {
            n.setX(n.x() + surfaceN.x());
            n.setY(n.y() + surfaceN.y());
            n.setZ(n.z() + surfaceN.z());
        }
        qreal nLen = sqrt(n.x() * n.x() + n.y() * n.y() + n.z() * n.z());
        n = QVector3D(n.x() / nLen, n.y() / nLen, n.z() / nLen);
        return n;
    };

    auto extractVector = [](const QVector<QVector<QVector3D> > &points, int i, int j, QVector3D &point) -> bool {
        if (i >= 0 && j >= 0 && i < points.count() - 1 && j < points[i].count() - 1) {
            point = points[i][j];
            return true;
        }
        return false;
    };

    QVector<QVector<QVector3D> > surfaceNormales;
    for (int i = 0; i < surfaceVertexes.count() - 1; i++) {
        QVector<QVector3D> surfaceNormalesLine;
        for (int j = 0; j < surfaceVertexes[i].count() - 1; j++) {
            surfaceNormalesLine << surfaceNormale(surfaceVertexes[i][j], surfaceVertexes[i + 1][j], surfaceVertexes[i + 1][j + 1]);
            surfaceNormalesLine << surfaceNormale(surfaceVertexes[i][j], surfaceVertexes[i + 1][j + 1], surfaceVertexes[i][j + 1]);
        }
        surfaceNormales << surfaceNormalesLine;
    }

    for (int i = 0; i < surfaceVertexes.count(); i++) {
        QVector<QVector3D> surfaceVertexNormalesRow;
        for (int j = 0; j < surfaceVertexes[i].count(); j++) {
            QList<QVector3D> normales;
            QVector3D v;
            if (extractVector(surfaceNormales, i - 1, (j - 1) * 2 + 0, v)) {
                normales << v;
            }
            if (extractVector(surfaceNormales, i - 1, (j - 1) * 2 + 1, v)) {
                normales << v;
            }
            if (extractVector(surfaceNormales, i - 1, (j - 1) * 2 + 2, v)) {
                normales << v;
            }
            if (extractVector(surfaceNormales, i    , (j - 1) * 2 + 1, v)) {
                normales << v;
            }
            if (extractVector(surfaceNormales, i    , (j - 1) * 2 + 2, v)) {
                normales << v;
            }
            if (extractVector(surfaceNormales, i    , (j - 1) * 2 + 3, v)) {
                normales << v;
            }
            surfaceVertexNormalesRow << vertexNormale(normales);
        }
        surfaceVertexNormales << surfaceVertexNormalesRow;;
    }
}

void Scene3D::applyFrustum(Scene3D::FrustumType ft)
{
    const float zNear = -20;
    const float zFar = 20000;
    const float fConvergence = 2000;
    const float dEye = 1;
    const float FOV = M_PI / 2.;
    const float ratio = (float)width() / (float)height();

    float a = ratio * tan(FOV / 2) * fConvergence;
    float b = a - dEye / 2;
    float c = a + dEye / 2;

    float top = zNear * tan(FOV / 2);
    float bottom = -top;
    float left = 0;
    float right = 0;
    if (ft == eLeftFrustum) {
        left = -b * zNear / fConvergence;
        right = c * zNear / fConvergence;
    } else if (ft == eRightFrustum) {
        left = -c * zNear / fConvergence;
        right = b * zNear / fConvergence;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // костыль
    if (width() > height()) {
        glOrtho(-1.0 * ratio, 1.0 * ratio, -1.0, 1.0, -20.0, 20000);
    } else {
        glOrtho(-1.0, 1.0, -1.0 / ratio, 1.0 / ratio, -20.0, 20000);
    }

    glFrustum(left, right, bottom, top, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (ft == eLeftFrustum) {
        glTranslatef(0.03, 0, 0);
    } else if (ft == eRightFrustum) {
        glTranslatef(-0.03, 0, 0);
    }
    glScalef(nSca, nSca, nSca);
    glTranslatef(xTra, zTra, 0.0f);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot, 0.0f, 0.0f, 1.0f);
}

