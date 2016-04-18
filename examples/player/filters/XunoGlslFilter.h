#ifndef XUNOGLSLFILTER_H
#define XUNOGLSLFILTER_H
#include <QtAV>
#include <QtAV/GLSLFilter.h>
#include <QtAV/OpenGLVideo.h>
#include <QDir>
#include <QFileInfo>
#include <QtCore/QCoreApplication>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QtGui/QDesktopServices>
#else
#include <QtCore/QStandardPaths>
#endif


class XunoGLSLFilter : public QtAV::GLSLFilter
{
public:
    XunoGLSLFilter(QObject* parent = 0);
    void setShader(QtAV::VideoShader *ush);

    void setNeedSave(bool value);
    void setSavePath(const QString &value);
    void setPlayer(QtAV::AVPlayer *player);

protected slots:
    void afterRendering();

private:
    QtAV::VideoShader *user_shader=Q_NULLPTR;
    bool needSave=false;
    QString savePath;
    QtAV::AVPlayer *m_player=Q_NULLPTR;
    QString defineFileName();
};

#endif // XUNOGLSLFILTER_H