#include "camvideosurface.h"

CamVideoSurface::CamVideoSurface(QObject *parent) :
    QAbstractVideoSurface(parent)
{

}

bool CamVideoSurface::present(const QVideoFrame &frame)
{
    QVideoFrame currentFrame = frame;
    if (currentFrame.map(QAbstractVideoBuffer::ReadOnly)) {
        QImage img(currentFrame.bits(),
                   currentFrame.width(),
                   currentFrame.height(),
                   currentFrame.bytesPerLine(),
                   currentFrame.imageFormatFromPixelFormat(currentFrame.pixelFormat()));

        emit newImage(img);
        return true;
    }
    return false;
}

QList<QVideoFrame::PixelFormat> CamVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
    if (type == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}
