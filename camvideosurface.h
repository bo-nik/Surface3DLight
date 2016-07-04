#ifndef VIDEOSURFACE_H
#define VIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QImage>
#include <QDebug>

class CamVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    CamVideoSurface(QObject *parent = 0);
    bool present(const QVideoFrame &frame);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType type = QAbstractVideoBuffer::NoHandle ) const;

signals:
    void newImage(QImage img);
};

#endif // VIDEOSURFACE_H
