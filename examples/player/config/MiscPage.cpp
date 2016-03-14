/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2015 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "MiscPage.h"
#include <QLabel>
#include <QLayout>
#include "common/Config.h"
#include <QDebug>

MiscPage::MiscPage()
{
    setObjectName("misc");
    QVBoxLayout *vl= new QVBoxLayout;
    QGroupBox *video = new QGroupBox;
    video->setTitle(tr(" Video"));
    QGridLayout *gl = new QGridLayout(video);
    gl->setSizeConstraint(QLayout::SetFixedSize);
    int r = 0;
    m_preview_on = new QCheckBox(tr("Preview"),video);
    gl->addWidget(m_preview_on, r++, 0);
    m_preview_w = new QSpinBox(video);
    m_preview_w->setRange(1, 1920);
    m_preview_h = new QSpinBox(video);
    m_preview_h->setRange(1, 1080);
    gl->addWidget(new QLabel(QString::fromLatin1("%1 %2: ").arg(tr("Preview")).arg(tr("size")),video), r, 0);
    QHBoxLayout *hb = new QHBoxLayout;
    hb->addWidget(m_preview_w);
    hb->addWidget(new QLabel(QString::fromLatin1("x"),video));
    hb->addWidget(m_preview_h);
    gl->addLayout(hb, r, 1);
    r++;
    gl->addWidget(new QLabel(tr("Force fps"),video), r, 0);
    m_fps = new QDoubleSpinBox(video);
    m_fps->setMinimum(-m_fps->maximum());
    m_fps->setToolTip(QString::fromLatin1("<= 0: ") + tr("Ignore"));
    gl->addWidget(m_fps, r++, 1);

    gl->addWidget(new QLabel(tr("Progress update interval") + QString::fromLatin1("(ms)"),video), r, 0);
    m_notify_interval = new QSpinBox(video);
    m_notify_interval->setEnabled(false);
    gl->addWidget(m_notify_interval, r++, 1);

    gl->addWidget(new QLabel(tr("Buffer frames"),video), r, 0);
    m_buffer_value = new QSpinBox(video);
    m_buffer_value->setRange(-1, 32767);
    m_buffer_value->setToolTip(QString::fromLatin1("-1: auto"));
    gl->addWidget(m_buffer_value, r++, 1);

    gl->addWidget(new QLabel(QString::fromLatin1("%1(%2)").arg(tr("Timeout")).arg(tr("s")),video), r, 0);
    m_timeout = new QDoubleSpinBox(video);
    m_timeout->setDecimals(3);
    m_timeout->setSingleStep(1.0);
    m_timeout->setMinimum(-0.5);
    m_timeout->setToolTip(QString::fromLatin1("<=0: never"));
    m_timeout_abort = new QCheckBox(tr("Abort"));
    hb = new QHBoxLayout;
    hb->addWidget(m_timeout);
    hb->addWidget(m_timeout_abort);
    gl->addLayout(hb, r++, 1);

    gl->addWidget(new QLabel(tr("OpenGL type")), r, 0);
    m_opengl = new QComboBox();
    m_opengl->addItem(QString::fromLatin1("Auto"), Config::Auto);
    m_opengl->addItem(QString::fromLatin1("Desktop"), Config::Desktop);
    m_opengl->addItem(QString::fromLatin1("OpenGLES"), Config::OpenGLES);
    m_opengl->addItem(QString::fromLatin1("Software"), Config::Software);
    m_opengl->setToolTip(tr("Windows only") + " Qt>=5.4 + dynamicgl" + QString::fromLatin1("\n") + tr("OpenGLES is Used by DXVA Zero Copy"));
    gl->addWidget(m_opengl, r, 1);
    m_angle_platform = new QComboBox();
    m_angle_platform->setToolTip(tr("D3D9 has performance if ZeroCopy is disabled or for software decoders") + QString::fromLatin1("\n") + tr("RESTART REQUIRED"));
    m_angle_platform->addItems(QStringList() << QString::fromLatin1("D3D9") << QString::fromLatin1("D3D11") << QString::fromLatin1("AUTO") << QString::fromLatin1("WARP"));
#ifndef QT_OPENGL_DYNAMIC
    m_opengl->setEnabled(false);
    m_angle_platform->setEnabled(false);
#endif
    gl->addWidget(m_angle_platform, r++, 2);

    gl->addWidget(new QLabel("EGL"), r, 0);
    m_egl = new QCheckBox();
    m_egl->setToolTip(tr("Currently only works for Qt>=5.5 XCB build"));
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0) || !defined(Q_OS_LINUX)
    m_egl->setEnabled(false);
#endif
    gl->addWidget(m_egl, r++, 1);

    gl->addWidget(new QLabel("Log"), r, 0);
    m_log = new QComboBox();
    m_log->addItems(QStringList() << QString::fromLatin1("off") << QString::fromLatin1("warning") << QString::fromLatin1("debug") << QString::fromLatin1("all"));
    gl->addWidget(m_log, r++, 1);


    //gl->addItem(new QSpacerItem(185, 0, QSizePolicy::Minimum, QSizePolicy::Minimum),r,0);
    //gl->addItem(new QSpacerItem(120, 0, QSizePolicy::Minimum, QSizePolicy::Minimum),r,1);
    //gl->addItem(new QSpacerItem(120, 0, QSizePolicy::Minimum, QSizePolicy::Minimum),r++,2);

    vl->addWidget(video,1, Qt::AlignTop);

    //------ imgseq
    QGroupBox *imgSeq = new QGroupBox(this);
    imgSeq->setTitle(tr(" Image Sequences"));
    QGridLayout *gli = new QGridLayout(imgSeq);
    //setLayout(gl);
    gli->setSizeConstraint(QLayout::SetFixedSize);
    //gli->
    r = 0;

    gli->addWidget(new QLabel(tr("Buffer frames"),imgSeq), r, 0);
    m_buffer_valueI = new QSpinBox(imgSeq);
    m_buffer_valueI->setRange(-1, 32767);
    m_buffer_valueI->setToolTip("-1: auto");
    gli->addWidget(m_buffer_valueI, r, 1);

    m_forceVideoClockI = new QCheckBox(tr("Force Video Clock"),imgSeq);
    gli->addWidget(m_forceVideoClockI, ++r, 0);
    gli->addItem(new QSpacerItem(178, 0, QSizePolicy::Minimum, QSizePolicy::Minimum),++r,0);
    gli->addItem(new QSpacerItem(140, 0, QSizePolicy::Minimum, QSizePolicy::Minimum),r,1);
    vl->addWidget(imgSeq,1, Qt::AlignTop);


    QGroupBox *playerView = new QGroupBox(this);
    playerView->setTitle(tr(" Player View"));
    QGridLayout *glpv = new QGridLayout(playerView);
    glpv->setSizeConstraint(QLayout::SetFixedSize);
    r = 0;
    m_floatcontrol = new QCheckBox(tr("Float Control Window"));
    m_floatcontrol->setToolTip(tr("Application restart required"));
    glpv->addWidget(m_floatcontrol, r, 0);
    glpv->addWidget(new QLabel(tr("(application restart required)")), r, 1);
    glpv->addItem(new QSpacerItem(178, 0, QSizePolicy::Minimum, QSizePolicy::Minimum),++r,0);

    m_advancedFilter = new QCheckBox(tr("Enable Advanced Filters"));
    m_advancedFilter->setToolTip(tr("Enable Advanced Filters"));
    glpv->addWidget(m_advancedFilter, r, 0);
    glpv->addWidget(new QLabel(tr("(movie restart required)")), r, 1);
    glpv->addItem(new QSpacerItem(178, 0, QSizePolicy::Minimum, QSizePolicy::Minimum),++r,0);
    glpv->addItem(new QSpacerItem(140, 0, QSizePolicy::Minimum, QSizePolicy::Minimum),r,1);


    vl->addWidget(playerView,1, Qt::AlignTop);

    setLayout(vl);


    applyToUi();
}

QString MiscPage::name() const
{
    return tr("View");
}


void MiscPage::applyFromUi()
{
    Config::instance().setPreviewEnabled(m_preview_on->isChecked())
            .setPreviewWidth(m_preview_w->value())
            .setPreviewHeight(m_preview_h->value())
            .setEGL(m_egl->isChecked())
            .setOpenGLType((Config::OpenGLType)m_opengl->itemData(m_opengl->currentIndex()).toInt())
            .setANGLEPlatform(m_angle_platform->currentText().toLower())
            .setForceFrameRate(m_fps->value())
            .setBufferValue(m_buffer_value->value())
            .setTimeout(m_timeout->value())
            .setAbortOnTimeout(m_timeout_abort->isChecked())
            .setBufferValueI(m_buffer_valueI->value())
            .setForceVideoClockI(m_forceVideoClockI->isChecked())
            .setFloatControlEnabled(m_floatcontrol->isChecked())
            .setAdvancedFilterEnabled(m_advancedFilter->isChecked())
            .setLogLevel(m_log->currentText().toLower())
            ;
}

void MiscPage::applyToUi()
{
    m_preview_on->setChecked(Config::instance().previewEnabled());
    m_preview_w->setValue(Config::instance().previewWidth());
    m_preview_h->setValue(Config::instance().previewHeight());
    m_opengl->setCurrentIndex(m_opengl->findData(Config::instance().openGLType()));
    m_angle_platform->setCurrentIndex(m_angle_platform->findText(Config::instance().getANGLEPlatform().toUpper()));
    m_egl->setChecked(Config::instance().isEGL());
    m_fps->setValue(Config::instance().forceFrameRate());
    //m_notify_interval->setValue(Config::instance().avfilterOptions());
    m_buffer_value->setValue(Config::instance().bufferValue());
    m_timeout->setValue(Config::instance().timeout());
    m_timeout_abort->setChecked(Config::instance().abortOnTimeout());
    m_buffer_valueI->setValue(Config::instance().bufferValueI());
    m_forceVideoClockI->setChecked(Config::instance().forceVideoClockI());
    m_floatcontrol->setChecked(Config::instance().floatControlEnabled());
    m_advancedFilter->setChecked(Config::instance().advancedFilterEnabled());
    m_log->setCurrentIndex(m_log->findText(Config::instance().logLevel().toLower()));
}
