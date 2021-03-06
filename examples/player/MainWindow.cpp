/******************************************************************************
    QtAV Player Demo:  this file is part of QtAV examples
    Copyright (C) 2012-2016 Wang Bin <wbsecg1@gmail.com>

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
#include "MainWindow.h"
#include "EventFilter.h"
#include "eventrenderer.h"
#include <QtAV>
#include <QtAV/GLSLFilter.h>
#include <QtAVWidgets>
#include <QtCore/QtDebug>
#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QTimeEdit>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>
#include <QGraphicsOpacityEffect>
#include <QComboBox>
#include <QResizeEvent>
#include <QWidgetAction>
#include <QLayout>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QToolTip>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QStyleFactory>

#include "ClickableMenu.h"
#include "Slider.h"
#include "StatisticsView.h"
#include "TVView.h"
#include "config/DecoderConfigPage.h"
#include "config/VideoEQConfigPage.h"
#include "config/ConfigDialog.h"
#include "filters/OSDFilter.h"
//#include "filters/AVFilterSubtitle.h"
#include "playlist/PlayList.h"
#include "../common/common.h"

#include <QUrl>
#ifndef QT_NO_OPENGL
#include "QtAV/GLSLFilter.h"
#endif

/*
 *TODO:
 * disable a/v actions if player is 0;
 * use action's value to set player's parameters when start to play a new file
 */


#define AVDEBUG() \
    qDebug("%s %s @%d", __FILE__, __FUNCTION__, __LINE__);

#define IMGSEQOPENGL 0

using namespace QtAV;
const qreal kVolumeInterval = 0.04;

extern QStringList idsToNames(QVector<VideoDecoderId> ids);
extern QVector<VideoDecoderId> idsFromNames(const QStringList& names);

void QLabelSetElideText(QLabel *label, QString text, int W = 0)
{
    QFontMetrics metrix(label->font());
    int width = label->width() - label->indent() - label->margin();
    if (W || label->parent()) {
        int w = W;
        if (!w)
            w = ((QWidget*)label->parent())->width();
        width = qMax(w - label->indent() - label->margin() - 13*(30), 0); //TODO: why 30?
    }
    QString clippedText = metrix.elidedText(text, Qt::ElideRight, width);
    label->setText(clippedText);
}




MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
  , mIsReady(false)
  , mHasPendingPlay(false)
  , mControlOn(false)
  , mShowControl(2)
  , mRepeateMax(0)
  , mpVOAction(nullptr)
  , mpPlayer(nullptr)
  , mpRenderer(nullptr)
  , mpVideoFilter(nullptr)
  , mpAudioFilter(nullptr)
  , mpStatisticsView(nullptr)
  , mpOSD(nullptr)
  , mpSubtitle(nullptr)
  , mCustomFPS(0.)
  , mPlayerScale(1.)
  , m_preview(Q_NULLPTR)
  , m_shader(nullptr)
  , m_glsl(nullptr)
{
    XUNOserverUrl=QString::fromLatin1("http://www.xuno.com");
    XUNOpresetUrl=XUNOserverUrl+QString::fromLatin1("/getpreset.php?");
#if defined(Q_OS_MACX) && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QApplication::setStyle(QStyleFactory::create("Fusion"));
#endif

    setWindowIcon(QIcon(QString::fromLatin1(":/ico/XunoPlayer-QtAV.ico")));

    mpOSD = new OSDFilter(this);
    mpSubtitle = new SubtitleFilter(this);
    mpChannelAction = nullptr;
    mpChannelMenu = nullptr;
    mpAudioTrackAction = nullptr;
    mGlobalMouse = QPointF();

    setMouseTracking(true); //mouseMoveEvent without press.
    connect(this, SIGNAL(ready()), SLOT(processPendingActions()));
    //QTimer::singleShot(10, this, SLOT(setupUi()));
    setupUi();
    //setToolTip(tr("Click black area to use shortcut(see right click menu)"));
    WindowEventFilter *we = new WindowEventFilter(this);
    installEventFilter(we);
    connect(we, SIGNAL(fullscreenChanged()), SLOT(handleFullscreenChange()));
}

MainWindow::~MainWindow()
{
    if (m_preview) {
        m_preview->close();
        delete m_preview;
    }
    mpHistory->save();
    mpPlayList->save();
    if (mpVolumeSlider && !mpVolumeSlider->parentWidget()) {
        mpVolumeSlider->close();
        delete mpVolumeSlider;
        mpVolumeSlider = nullptr;
    }
    if (mpStatisticsView) {
        delete mpStatisticsView;
        mpStatisticsView = nullptr;
    }
}

void MainWindow::initPlayer()
{
    mpFullScreenBtn->hide();
    mpPlayer = new AVPlayer(this);
    mIsReady = true;
    VideoRenderer *vo = VideoRenderer::create((VideoRendererId)property("rendererId").toInt());
    if (!vo || !vo->isAvailable() || !vo->widget()) {
        QMessageBox::critical(0, QString::fromLatin1("QtAV"), tr("Video renderer is ") + tr("not availabe on your platform!"));
    }
    setRenderer(vo);
    //mpSubtitle->installTo(mpPlayer); //filter on frame
    mpSubtitle->setPlayer(mpPlayer);
    //mpPlayer->setAudioOutput(AudioOutputFactory::create(AudioOutputId_OpenAL));
    installGLSLFilter(); //**
    //installSaveGL();

    //installSimpleFilter();

    EventFilter *ef = new EventFilter(mpPlayer);
    qApp->installEventFilter(ef);
    ef->setXunoGLSLFilter(mpGLSLFilter);

    if (mpRenderer && mpRenderer->widget()) {
        EventRenderer *evR = new EventRenderer(this);
        mpRenderer->widget()->installEventFilter(evR);
    }

    connect(ef, SIGNAL(helpRequested()), SLOT(help()));
    connect(ef, SIGNAL(showNextOSD()), SLOT(showNextOSD()));
    onCaptureConfigChanged();
    onAVFilterVideoConfigChanged();
    onAVFilterAudioConfigChanged();
    connect(&Config::instance(), SIGNAL(forceFrameRateChanged()), SLOT(setFrameRate()));
    connect(&Config::instance(), SIGNAL(captureDirChanged(QString)), SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(captureFormatChanged(QString)), SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(captureQualityChanged(int)), SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(captureTypeChanged(int)), SLOT(onCaptureConfigChanged()));
    connect(&Config::instance(), SIGNAL(previewEnabledChanged()), SLOT(onPreviewEnabledChanged()));
    connect(&Config::instance(), SIGNAL(avfilterVideoChanged()), SLOT(onAVFilterVideoConfigChanged()));
    connect(&Config::instance(), SIGNAL(avfilterAudioChanged()), SLOT(onAVFilterAudioConfigChanged()));
    connect(&Config::instance(), SIGNAL(bufferValueChanged()), SLOT(onBufferValueChanged()));
    connect(&Config::instance(), SIGNAL(abortOnTimeoutChanged()), SLOT(onAbortOnTimeoutChanged()));
    connect(mpStopBtn, SIGNAL(clicked()), this, SLOT(stopUnload()));
    connect(mpForwardBtn, SIGNAL(clicked()), mpPlayer, SLOT(seekForward()));
    connect(mpBackwardBtn, SIGNAL(clicked()), mpPlayer, SLOT(seekBackward()));
    connect(mpVolumeBtn, SIGNAL(clicked()), SLOT(showHideVolumeBar()));
    connect(mpVolumeSlider, SIGNAL(sliderPressed()), SLOT(setVolume()));
    connect(mpVolumeSlider, SIGNAL(valueChanged(int)), SLOT(setVolume()));

    connect(mpPlayer, SIGNAL(seekFinished(qint64)), SLOT(onSeekFinished(qint64)));
    connect(mpPlayer, SIGNAL(mediaStatusChanged(QtAV::MediaStatus)), SLOT(onMediaStatusChanged()));
    connect(mpPlayer, SIGNAL(bufferProgressChanged(qreal)), SLOT(onBufferProgress(qreal)));
    connect(mpPlayer, SIGNAL(error(QtAV::AVError)), this, SLOT(handleError(QtAV::AVError)));
    connect(mpPlayer, SIGNAL(started()), this, SLOT(onStartPlay()));
    connect(mpPlayer, SIGNAL(stopped()), this, SLOT(onStopPlay()));
    connect(mpPlayer, SIGNAL(paused(bool)), this, SLOT(onPaused(bool)));
    connect(mpPlayer, SIGNAL(speedChanged(qreal)), this, SLOT(onSpeedChange(qreal)));
    connect(mpPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChange(qint64)));
    //connect(mpPlayer, SIGNAL(volumeChanged(qreal)), SLOT(syncVolumeUi(qreal)));
    connect(mpVideoEQ, SIGNAL(brightnessChanged(int)), this, SLOT(onBrightnessChanged(int)));
    connect(mpVideoEQ, SIGNAL(contrastChanged(int)), this, SLOT(onContrastChanged(int)));
    connect(mpVideoEQ, SIGNAL(hueChanegd(int)), this, SLOT(onHueChanged(int)));
    connect(mpVideoEQ, SIGNAL(saturationChanged(int)), this, SLOT(onSaturationChanged(int)));
    connect(mpVideoEQ, SIGNAL(gammaRGBChanged(int)),  this, SLOT(onGammaRGBChanged(int)));
    connect(mpVideoEQ, SIGNAL(filterSharpChanged(int)),  this, SLOT(onFilterSharpChanged(int)));

    emit ready(); //emit this signal after connection. otherwise the slots may not be called for the first time
}

void MainWindow::onSeekFinished(qint64 pos)
{
    qDebug("seek finished at %lld/%lld", pos, mpPlayer->position());
}

void MainWindow::stopUnload()
{
    if (mpPlayer){
        mpPlayer->stop();
        //        mpPlayer->unload();
    }
}

void MainWindow::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(QMargins());
    setLayout(mainLayout);

    mpPlayerLayout = new QVBoxLayout();

    //QVBoxLayout *mpControlLayout = new QVBoxLayout();

    if (Config::instance().floatControlEnabled()){
        detachedControl = new QWidget(this);
        detachedControl->setWindowTitle(tr("XunoPlayer-QtAV Controls"));
        detachedControl->setWindowFlags(Qt::Dialog);
        detachedControl->setWindowFlags(detachedControl->windowFlags() & ~Qt::WindowCloseButtonHint);
        detachedControl->setMaximumHeight(125);//785
        detachedControl->setMaximumHeight(55);
        detachedControl->resize(800,detachedControl->minimumHeight());
        this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
        detachedControl->move(this->pos().x(),this->pos().y()+this->rect().height());
        detachedControlLayout = new QVBoxLayout();
        detachedControlLayout->setContentsMargins(0,0,0,0);
        detachedControl->setLayout(detachedControlLayout);
        detachedControl->show();
        detachedControl->raise();
        mpControl = new QWidget(detachedControl);



    }else{
        mpControl = new QWidget(this);

    }

    QString buttons_style_bg= "background-color: rgba(30, 30, 30, 0);";
    this->setStyleSheet(buttons_style_bg);

    mpControl->setMinimumWidth(865);
    mpControl->setMaximumHeight(25);
    mpControl->setMaximumHeight(30);

    //mpPreview = new QLable(this);

    mpTimeSlider = new Slider(mpControl);
    mpTimeSlider->setDisabled(true);
    mpTimeSlider->setTracking(true);
    mpTimeSlider->setOrientation(Qt::Horizontal);
    mpTimeSlider->setMinimum(0);
    mpTimeSlider->setStyleSheet(" \
         QSlider::groove:horizontal { \
             margin: 4px 0px 2px 0px;\
             background-color: solid #F8F8F8; \
         } \
         QSlider::handle:horizontal { \
             width: 7px;  \
             border: 1px solid #5c5c5c; \
             background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #ffffff); \
             border-radius: 2px;\
             margin: -2px 0;\
         }\
         QSlider::sub-page:horizontal { \
             margin: 2px 0px 3px 0px;\
             background-color: #0066FF; \
         } \
         ");
    mpTimeSlider->addVisualLimits();
    mpCurrent = new QLabel(mpControl);
    mpCurrent->setToolTip(tr("Current time"));
    mpCurrent->setContentsMargins(QMargins(2, 2, 2, 2));
    mpCurrent->setText(QString::fromLatin1("00:00:00"));
    mpEnd = new QLabel(mpControl);
    mpEnd->setToolTip(tr("Duration"));
    mpEnd->setContentsMargins(QMargins(2, 2, 2, 2));
    mpEnd->setText(QString::fromLatin1("00:00:00"));
    //    mpTitle = new QLabel(mpControl);
    //    mpTitle->setToolTip(tr("Render engine"));
    //    mpTitle->setText("QPainter");
    //    mpTitle->setIndent(8);
    mpSpeed = new QLabel(QString::fromLatin1("1.00"));
    mpSpeed->setContentsMargins(QMargins(1, 1, 1, 1));
    mpSpeed->setToolTip(tr("Speed. Ctrl+Up/Down"));

    mpPlayPauseBtn = new QToolButton(mpControl);
    mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/play.svg")));
    mpStopBtn = new QToolButton(mpControl);
    mpStopBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/stop.svg")));
    mpBackwardBtn = new QToolButton(mpControl);
    mpBackwardBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/backward.svg")));
    mpForwardBtn = new QToolButton(mpControl);
    mpForwardBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/forward.svg")));
    mpOpenBtn = new QToolButton(mpControl);
    mpOpenBtn->setToolTip(tr("Open"));
    mpOpenBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/open.svg")));

    mpInfoBtn = new QToolButton();
    mpInfoBtn->setToolTip(QString::fromLatin1("Media information"));
    mpInfoBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/info.svg")));
    mpCaptureBtn = new QToolButton();
    mpCaptureBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/capture.svg")));
    if (Config::instance().captureType()==Config::CaptureType::DecodedFrame)
        mpCaptureBtn->setToolTip(tr("Capture"));
    else
        mpCaptureBtn->setToolTip(tr("Capture Post Filtered"));
    mpVolumeBtn = new QToolButton();
    mpVolumeBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/sound.svg")));

    mpVolumeSlider = new Slider();
    //mpVolumeSlider->hide();
    mpVolumeSlider->setOrientation(Qt::Horizontal);
    mpVolumeSlider->setMinimum(0);
    const int kVolumeSliderMax = 100;
    mpVolumeSlider->setMaximum(kVolumeSliderMax);
    //mpVolumeSlider->setMaximumHeight(12);
    mpVolumeSlider->setMaximumWidth(88);
    //mpVolumeSlider->setValue(int(1.0/kVolumeInterval*qreal(kVolumeSliderMax)/100.0));
    mpVolumeSlider->setValue(80);
    setVolume();

    mpWebBtn = new QToolButton();
    mpWebBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mpWebBtn->setText(tr("Web"));
    //mpWebBtn->setMaximumHeight(kMaxButtonIconMargin);
    mpWebBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mpWebBtn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpWebBtn->setMinimumHeight(mpInfoBtn->sizeHint().height());
    mpWebBtn->setPopupMode(QToolButton::InstantPopup);
    mpWebBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mpWebBtn->setToolTip(tr("Open XunoWeb browser"));
    mpWebBtn->setStyleSheet(QString::fromLatin1("color:grey;"));


    mpWebMenu = new ConfigWebMemu(mpWebBtn);
    mpWebMenu->setXunoVersion(XUNO_QtAV_Version_String(false));
    mpWebBtn->setMenu(mpWebMenu);
    connect(mpWebMenu, SIGNAL(onPlayXunoBrowser(QUrl)), SLOT(onClickXunoBrowser(QUrl)));
    connect(&Config::instance(),SIGNAL(weblinksChanged()),mpWebMenu,SLOT(onChanged()));

    mpFullScreenBtn = new QToolButton();
    mpFullScreenBtn->setIcon(QPixmap(QString::fromLatin1(":/theme/dark/fullscreen.svg")));
    //mpFullScreenBtn->setIconSize(QSize(a, a));
    //mpFullScreenBtn->setMaximumSize(a+kMaxButtonIconMargin+2, a+kMaxButtonIconMargin);
    mpFullScreenBtn->setToolTip(tr("Full Screen"));

    mpScaleX15Btn = new QToolButton();
    mpScaleX15Btn->setText(tr("x1.5"));
    mpScaleX15Btn->setToolTip(tr("Scale X1.5"));
    mpScaleX15Btn->setStyleSheet(QString::fromLatin1("color:grey;"));
    mpScaleX15Btn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpScaleX15Btn->setMinimumHeight(mpInfoBtn->sizeHint().height());


    mpScaleX2Btn = new QToolButton();
    mpScaleX2Btn->setText(tr("x2"));
    mpScaleX2Btn->setToolTip(tr("Scale X2"));
    mpScaleX2Btn->setStyleSheet(QString::fromLatin1("color:grey;"));
    mpScaleX2Btn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpScaleX2Btn->setMinimumHeight(mpInfoBtn->sizeHint().height());

    mpScaleX1Btn = new QToolButton();
    mpScaleX1Btn->setText(tr("N"));
    mpScaleX1Btn->setToolTip(tr("Naitive Resolution"));
    mpScaleX1Btn->setStyleSheet(QString::fromLatin1("color:grey;"));
    mpScaleX1Btn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpScaleX1Btn->setMinimumHeight(mpInfoBtn->sizeHint().height());


    mpMenuBtn = new QToolButton();
    mpMenuBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/menu.svg")));
    //mpMenuBtn->setAutoRaise(true);
    mpMenuBtn->setPopupMode(QToolButton::InstantPopup);

    mpMenuBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    //mpMenuBtn->setMaximumHeight(a+kMaxButtonIconMargin);
    mpMenuBtn->setText(tr("Menu "));
    mpMenuBtn->setToolTip(tr("Configuratrion menu"));
    mpMenuBtn->setStyleSheet("color:grey;");
    mpMenuBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mpMenuBtn->setMaximumHeight(mpInfoBtn->sizeHint().height());
    mpMenuBtn->setMinimumHeight(mpInfoBtn->sizeHint().height());

    QMenu *subMenu = Q_NULLPTR;
    QWidgetAction *pWA = Q_NULLPTR;
    mpMenu = new QMenu(mpMenuBtn);
    //mpMenu->setStyleSheet("color:white;");
    mpMenu->addAction(tr("Open File"), this, SLOT(openFile()));
    mpMenu->addAction(tr("Open Url"), this, SLOT(openUrl()));
    //mpMenu->addAction(tr("Online channels"), this, SLOT(onTVMenuClick()));

    //    subMenu = new ClickableMenu(tr("Image Sequence"));
    //mpMenu->addMenu(subMenu);

    mpImageSequence = new ImageSequenceConfigPage();
    connect(mpImageSequence, SIGNAL(play(QString)), SLOT(play(QString)));
    connect(mpImageSequence, SIGNAL(stop()), SLOT(stopUnload()));
    connect(mpImageSequence, SIGNAL(repeatAChanged(QTime)), SLOT(repeatAChanged(QTime)));
    connect(mpImageSequence, SIGNAL(repeatBChanged(QTime)), SLOT(repeatBChanged(QTime)));
    connect(mpImageSequence, SIGNAL(toggleRepeat(bool)), SLOT(toggleRepeat(bool)));
    connect(mpImageSequence, SIGNAL(customfpsChanged(double)), SLOT(customfpsChanged(double)));
    connect(mpImageSequence, SIGNAL(toogledFrameExtractor(bool)), SLOT(onImageSequenceToogledFrameExtractor(bool)));
    connect(mpImageSequence, SIGNAL(setPlayerScale(double)), SLOT(setPlayerScale(double)));
    connect(mpImageSequence, SIGNAL(RepeatLoopChanged(int)), SLOT(RepeatLoopChanged(int)));

    mpMenu->addAction(tr("Image Sequence"), this, SLOT(onImageSequenceConfig()));

    //    pWA = new QWidgetAction(0);
    //    pWA->setDefaultWidget(mpImageSequence);
    //    subMenu->addAction(pWA);

    mpMenu->addSeparator();
    subMenu = new QMenu(tr("Play list"));
    mpMenu->addMenu(subMenu);
    mpPlayList = new PlayList(this);
    mpPlayList->setSaveFile(Config::instance().defaultDir() + QString::fromLatin1("/playlist.qds"));
    mpPlayList->load();
    connect(mpPlayList, SIGNAL(aboutToPlay(QString)), SLOT(play(QString)));
    pWA = new QWidgetAction(Q_NULLPTR);
    pWA->setDefaultWidget(mpPlayList);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    subMenu = new QMenu(tr("History"));
    mpMenu->addMenu(subMenu);
    mpHistory = new PlayList(this);
    mpHistory->setMaxRows(20);
    mpHistory->setSaveFile(Config::instance().defaultDir() + QString::fromLatin1("/history.qds"));
    mpHistory->load();
    connect(mpHistory, SIGNAL(aboutToPlay(QString)), SLOT(play(QString)));
    pWA = new QWidgetAction(Q_NULLPTR);
    pWA->setDefaultWidget(mpHistory);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    mpMenu->addSeparator();

    //mpMenu->addAction(tr("Report"))->setEnabled(false); //report bug, suggestions etc. using maillist?
    mpMenu->addAction(tr("About"), this, SLOT(about()));
    mpMenu->addAction(tr("Help"), this, SLOT(help()));
    mpMenu->addAction(tr("Donate"), this, SLOT(donate()));
    mpMenu->addAction(tr("Setup"), this, SLOT(setup()));
    mpMenu->addSeparator();
    mpMenuBtn->setMenu(mpMenu);
    mpMenu->addSeparator();

    subMenu = new QMenu(tr("Speed"));
    mpMenu->addMenu(subMenu);
    QDoubleSpinBox *pSpeedBox = new QDoubleSpinBox(Q_NULLPTR);
    pSpeedBox->setRange(0.01, 20);
    pSpeedBox->setValue(1.0);
    pSpeedBox->setSingleStep(0.01);
    pSpeedBox->setCorrectionMode(QAbstractSpinBox::CorrectToPreviousValue);
    pWA = new QWidgetAction(Q_NULLPTR);
    pWA->setDefaultWidget(pSpeedBox);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?

    subMenu = new ClickableMenu(tr("Repeat"));
    mpMenu->addMenu(subMenu);
    subMenu->setObjectName(QString::fromUtf8("RepeatMenu"));
    //subMenu->setEnabled(false);
    mpRepeatEnableAction = subMenu->addAction(tr("Enable"));
    mpRepeatEnableAction->setCheckable(true);
    connect(mpRepeatEnableAction, SIGNAL(toggled(bool)), SLOT(toggleRepeat(bool)));
    // TODO: move to a func or class
    mpRepeatLoop = new QCheckBox(tr("Continuous Loop"), this);
    connect(mpRepeatLoop, SIGNAL(stateChanged(int)), SLOT(RepeatLoopChanged(int)));
    mpRepeatBox = new QSpinBox(Q_NULLPTR);
    mpRepeatBox->setMinimum(-1);
    mpRepeatBox->setValue(1);
    mpRepeatBox->setToolTip(QString::fromLatin1("-1: ") + tr("infinity"));
    connect(mpRepeatBox, SIGNAL(valueChanged(int)), SLOT(setRepeateMax(int)));
    QLabel *pRepeatLabel = new QLabel(tr("Times"));
    QHBoxLayout *hb = new QHBoxLayout;
    hb->setObjectName(QString::fromUtf8("TimesLayout"));
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatBox);
    QVBoxLayout *vb = new QVBoxLayout;
    vb->addWidget(mpRepeatLoop);
    vb->addLayout(hb);
    pRepeatLabel = new QLabel(tr("From"));
    mpRepeatA = new QTimeEdit();
    mpRepeatA->setDisplayFormat(QString::fromLatin1("HH:mm:ss.zzz"));
    mpRepeatA->setToolTip(tr("negative value means from the end"));
    connect(mpRepeatA, SIGNAL(timeChanged(QTime)), SLOT(repeatAChanged(QTime)));
    hb = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatA);
    vb->addLayout(hb);
    pRepeatLabel = new QLabel(tr("To"));
    mpRepeatB = new QTimeEdit();
    mpRepeatB->setDisplayFormat(QString::fromLatin1("HH:mm:ss.zzz"));
    mpRepeatB->setToolTip(tr("negative value means from the end"));
    connect(mpRepeatB, SIGNAL(timeChanged(QTime)), SLOT(repeatBChanged(QTime)));
    hb = new QHBoxLayout;
    hb->addWidget(pRepeatLabel);
    hb->addWidget(mpRepeatB);
    vb->addLayout(hb);
    QWidget *wgt = new QWidget;
    wgt->setLayout(vb);

    pWA = new QWidgetAction(Q_NULLPTR);
    pWA->setDefaultWidget(wgt);
    pWA->defaultWidget()->setEnabled(false);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    mpRepeatAction = pWA;
    mpRepeatLoop->setCheckState(Qt::Checked);

    mpMenu->addSeparator();

    mpClockMenu = new ClickableMenu(tr("Clock"));
    mpClockMenu->setObjectName(QStringLiteral("Clock"));
    mpMenu->addMenu(mpClockMenu);
    mpClockMenuAction = new QActionGroup(mpClockMenu);
    mpClockMenuAction->setObjectName(QStringLiteral("ClockAction"));
    mpClockMenuAction->setExclusive(true);
    connect(mpClockMenu, SIGNAL(triggered(QAction*)), SLOT(changeClockType(QAction*)));
    mpClockMenu->addAction(tr("Auto"))->setData(-1);
    mpClockMenu->addAction(tr("Audio"))->setData(AVClock::AudioClock);
    mpClockMenu->addAction(tr("Video"))->setData(AVClock::VideoClock);
    foreach(QAction* action, mpClockMenu->actions()) {
        action->setActionGroup(mpClockMenuAction);
        action->setCheckable(true);
    }
    QAction *autoClockAction = mpClockMenu->actions().at(0);
    autoClockAction->setChecked(true);
    autoClockAction->setToolTip(tr("Take effect in next playback"));

    subMenu = new ClickableMenu(tr("Subtitle"));
    mpMenu->addMenu(subMenu);
    QAction *act = subMenu->addAction(tr("Enable"));
    act->setCheckable(true);
    act->setChecked(mpSubtitle->isEnabled());
    connect(act, SIGNAL(toggled(bool)), SLOT(toggoleSubtitleEnabled(bool)));
    act = subMenu->addAction(tr("Auto load"));
    act->setCheckable(true);
    act->setChecked(mpSubtitle->autoLoad());
    connect(act, SIGNAL(toggled(bool)), SLOT(toggleSubtitleAutoLoad(bool)));
    subMenu->addAction(tr("Open"), this, SLOT(openSubtitle()));

    wgt = new QWidget();
    hb = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(tr("Engine")));
    QComboBox *box = new QComboBox();
    hb->addWidget(box);
    pWA = new QWidgetAction(Q_NULLPTR);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    box->addItem(QString::fromLatin1("FFmpeg"), QString::fromLatin1("FFmpeg"));
    box->addItem(QString::fromLatin1("LibASS"), QString::fromLatin1("LibASS"));
    connect(box, SIGNAL(activated(QString)), SLOT(setSubtitleEngine(QString)));
    mpSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
    box->setToolTip(tr("FFmpeg supports more subtitles but only render plain text") + QString::fromLatin1("\n") + tr("LibASS supports ass styles"));

    wgt = new QWidget();
    hb = new QHBoxLayout();
    wgt->setLayout(hb);
    hb->addWidget(new QLabel(tr("Charset")));
    box = new QComboBox();
    hb->addWidget(box);
    pWA = new QWidgetAction(Q_NULLPTR);
    pWA->setDefaultWidget(wgt);
    subMenu->addAction(pWA); //must add action after the widget action is ready. is it a Qt bug?
    box->addItem(tr("Auto detect"), QString::fromLatin1("AutoDetect"));
    box->addItem(tr("System"), QString::fromLatin1("System"));
    foreach (const QByteArray& cs, QTextCodec::availableCodecs()) {
        box->addItem(QString::fromLatin1(cs), QString::fromLatin1(cs));
    }
    connect(box, SIGNAL(activated(QString)), SLOT(setSubtitleCharset(QString)));
    mpSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
    box->setToolTip(tr("Auto detect requires libchardet"));

    subMenu = new ClickableMenu(tr("Audio track"));
    mpMenu->addMenu(subMenu);
    mpAudioTrackMenu = subMenu;
    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeAudioTrack(QAction*)));
    initAudioTrackMenu();

    subMenu = new ClickableMenu(tr("Channel"));
    mpMenu->addMenu(subMenu);
    mpChannelMenu = subMenu;
    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeChannel(QAction*)));
    subMenu->addAction(tr("As input"))->setData(AudioFormat::ChannelLayout_Unsupported); //will set to input in resampler if not supported.
    subMenu->addAction(tr("Stereo"))->setData(AudioFormat::ChannelLayout_Stereo);
    subMenu->addAction(tr("Mono (center)"))->setData(AudioFormat::ChannelLayout_Center);
    subMenu->addAction(tr("Left"))->setData(AudioFormat::ChannelLayout_Left);
    subMenu->addAction(tr("Right"))->setData(AudioFormat::ChannelLayout_Right);
    QActionGroup *ag = new QActionGroup(subMenu);
    ag->setExclusive(true);
    foreach(QAction* action, subMenu->actions()) {
        ag->addAction(action);
        action->setCheckable(true);
    }

    subMenu = new QMenu(tr("Aspect ratio"));
    mpMenu->addMenu(subMenu);
    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(switchAspectRatio(QAction*)));
    mpARAction = subMenu->addAction(tr("Video"));
    mpARAction->setData(0);
    subMenu->addAction(tr("Window"))->setData(-1);
    subMenu->addAction(QString::fromLatin1("4:3"))->setData(4.0/3.0);
    subMenu->addAction(QString::fromLatin1("16:9"))->setData(16.0/9.0);
    subMenu->addAction(tr("Custom"))->setData(-2);
    foreach(QAction* action, subMenu->actions()) {
        action->setCheckable(true);
    }
    mpARAction->setChecked(true);

    subMenu = new ClickableMenu(tr("Color space"));
    mpMenu->addMenu(subMenu);
    mpVideoEQ = new VideoEQConfigPage();
    connect(mpVideoEQ, SIGNAL(engineChanged()), SLOT(onVideoEQEngineChanged()));
    pWA = new QWidgetAction(Q_NULLPTR);
    pWA->setDefaultWidget(mpVideoEQ);
    subMenu->addAction(pWA);
    mpVideoEQ->setXunoVersion(XUNO_QtAV_Version_String(false));
    mpVideoEQ->setSaveFile(Config::instance().defaultDir() + "/presets.ini");
    mpVideoEQ->loadLocalPresets();

    subMenu = new ClickableMenu(tr("Decoder"));
    mpMenu->addMenu(subMenu);
    mpDecoderConfigPage = new DecoderConfigPage();
    pWA = new QWidgetAction(Q_NULLPTR);
    pWA->setDefaultWidget(mpDecoderConfigPage);
    subMenu->addAction(pWA);

    subMenu = new ClickableMenu(tr("Renderer"));
    mpMenu->addMenu(subMenu);
    connect(subMenu, SIGNAL(triggered(QAction*)), SLOT(changeVO(QAction*)));
    //TODO: AVOutput.name,detail(description). check whether it is available
    VideoRendererId *vo = Q_NULLPTR;
    while ((vo = VideoRenderer::next(vo))) {
        // skip non-widget renderers
        if (*vo == VideoRendererId_OpenGLWindow || *vo == VideoRendererId_GraphicsItem)
            continue;
        QAction *voa = subMenu->addAction(QString::fromLatin1(VideoRenderer::name(*vo)));
        voa->setData(*vo);
        voa->setCheckable(true);
        if (!mpVOAction)
            mpVOAction = voa;
    }
    mpVOAction->setChecked(true);
    mVOActions = subMenu->actions();

    //mpImgSeqExtract->hide();
    mainLayout->addLayout(mpPlayerLayout);

    if (detachedControlLayout){
        detachedControlLayout->addWidget(mpTimeSlider);
        detachedControlLayout->addWidget(mpControl);
    }else{
        mainLayout->addWidget(mpTimeSlider);
        mainLayout->addWidget(mpControl);
    }

    // mpControlLayout->addWidget(mpImgSeqExtract);
    //mainLayout->addWidget(mpControl);

    QVBoxLayout *controlVLayout = new QVBoxLayout();
    controlVLayout->setSpacing(0);
    controlVLayout->setMargin(0);

    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(0);
    controlLayout->setContentsMargins(QMargins(1, 1, 1, 1));
    mpControl->setLayout(controlVLayout);
    controlLayout->addWidget(mpCurrent);
    //controlLayout->addWidget(mpTitle);
    //QSpacerItem *space = new QSpacerItem(mpPlayPauseBtn->width(), mpPlayPauseBtn->height(), QSizePolicy::MinimumExpanding);
    QSpacerItem *space = new QSpacerItem(1, mpPlayPauseBtn->height(), QSizePolicy::MinimumExpanding);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpVolumeSlider);
    controlLayout->addWidget(mpVolumeBtn);
    controlLayout->addWidget(mpCaptureBtn);
    controlLayout->addWidget(mpPlayPauseBtn);
    controlLayout->addWidget(mpStopBtn);
    controlLayout->addWidget(mpBackwardBtn);
    controlLayout->addWidget(mpForwardBtn);
    space = new QSpacerItem(mpPlayPauseBtn->width()/2, mpPlayPauseBtn->height(), QSizePolicy::Fixed);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpWebBtn);
    controlLayout->addWidget(mpMenuBtn);
    controlLayout->addWidget(mpOpenBtn);
    controlLayout->addWidget(mpInfoBtn);
    //controlLayout->addWidget(mpSpeed);
    //controlLayout->addWidget(mpSetupBtn);
    //controlLayout->addWidget(mpMenuBtn);
    controlLayout->addWidget(mpFullScreenBtn);
    controlLayout->addWidget(mpScaleX15Btn);
    controlLayout->addWidget(mpScaleX2Btn);
    controlLayout->addWidget(mpScaleX1Btn);

    space = new QSpacerItem(mpPlayPauseBtn->width(), mpPlayPauseBtn->height(), QSizePolicy::Expanding);
    controlLayout->addSpacerItem(space);
    controlLayout->addWidget(mpEnd);

    controlVLayout->setSizeConstraint(QLayout::SetMinimumSize);
    mpImgSeqExtract = new ImgSeqExtractControl(mpControl);
    connect(mpImgSeqExtract,SIGNAL(seek(QTime)),SLOT(seek(QTime)));
    connect(mpImgSeqExtract,SIGNAL(pause()),SLOT(pause()));
    connect(mpImgSeqExtract,SIGNAL(onStartPlay()),SLOT(onStartPlay()));
    connect(mpImgSeqExtract,SIGNAL(toggleRepeat(bool)),SLOT(toggleRepeat(bool)));
    connect(mpImgSeqExtract,SIGNAL(RepeatLoopChanged(int)),SLOT(RepeatLoopChanged(int)));
    connect(mpImgSeqExtract,SIGNAL(togglePlayPause()),SLOT(togglePlayPause()));
    connect(mpImgSeqExtract,SIGNAL(repeatAChanged(QTime)),SLOT(repeatAChanged(QTime)));
    connect(mpImgSeqExtract,SIGNAL(repeatBChanged(QTime)),SLOT(repeatBChanged(QTime)));
    connect(mpImgSeqExtract,SIGNAL(setTimeSliderVisualMinLimit(QTime)),SLOT(setTimeSliderVisualMinLimit(QTime)));
    connect(mpImgSeqExtract,SIGNAL(setTimeSliderVisualMaxLimit(QTime)),SLOT(setTimeSliderVisualMaxLimit(QTime)));
    controlVLayout->addWidget(mpImgSeqExtract);
    controlVLayout->addLayout(controlLayout);
    mpImgSeqExtract->setVisible(false);
    //    mpImgSeqExtract->setStyleSheet(" \
    //    QSpinBox::up-arrow { \
    //        image: url(:/theme/up_arrow-bw.png); \
    //        width: 7px; \
    //        height: 7px; \
    //    } \
    //    QSpinBox::down-arrow { \
    //        image: url(:/theme/down_arrow-bw.png); \
    //        width: 7px; \
    //        height: 7px; \
    //    } \
    //    ");

    connect(pSpeedBox, SIGNAL(valueChanged(double)), SLOT(onSpinBoxChanged(double)));
    connect(mpOpenBtn, SIGNAL(clicked()), SLOT(openFile()));
    connect(mpPlayPauseBtn, SIGNAL(clicked()), SLOT(togglePlayPause()));
    connect(mpInfoBtn, SIGNAL(clicked()), SLOT(showInfo()));
    //valueChanged can be triggered by non-mouse event
    connect(mpTimeSlider, SIGNAL(sliderMoved(int)), SLOT(seek(int)));
    connect(mpTimeSlider, SIGNAL(sliderPressed()), SLOT(seek()));
    connect(mpTimeSlider, SIGNAL(onLeave()), SLOT(onTimeSliderLeave()));
    connect(mpTimeSlider, SIGNAL(onHover(int,int)), SLOT(onTimeSliderHover(int,int)));

    //connect(mpWebBtn, SIGNAL(clicked()), SLOT(onXunoBrowser()));
    connect(mpFullScreenBtn, SIGNAL(clicked()), SLOT(onFullScreen()));

    connect(mpScaleX2Btn, SIGNAL(clicked()), SLOT(onScaleX2Btn()));
    connect(mpScaleX15Btn, SIGNAL(clicked()), SLOT(onScaleX15Btn()));
    connect(mpScaleX1Btn, SIGNAL(clicked()), SLOT(onScaleX1Btn()));

    connect(&Config::instance(), SIGNAL(userShaderEnabledChanged()), SLOT(onUserShaderChanged()));
    connect(&Config::instance(), SIGNAL(intermediateFBOChanged()), SLOT(onUserShaderChanged()));
    connect(&Config::instance(), SIGNAL(fragHeaderChanged()), SLOT(onUserShaderChanged()));
    connect(&Config::instance(), SIGNAL(fragSampleChanged()), SLOT(onUserShaderChanged()));
    connect(&Config::instance(), SIGNAL(fragPostProcessChanged()), SLOT(onUserShaderChanged()));

    QTimer::singleShot(0, this, SLOT(initPlayer()));
}

void MainWindow::changeChannel(QAction *action)
{
    if (action == mpChannelAction) {
        action->toggle();
        return;
    }
    AudioFormat::ChannelLayout cl = (AudioFormat::ChannelLayout)action->data().toInt();
    AudioOutput *ao = mpPlayer ? mpPlayer->audio() : 0; //getAO()?
    if (!ao) {
        qWarning("No audio output!");
        return;
    }
    mpChannelAction->setChecked(false);
    mpChannelAction = action;
    mpChannelAction->setChecked(true);
    if (!ao->close()) {
        qWarning("close audio failed");
        return;
    }
    AudioFormat af(ao->audioFormat());
    af.setChannelLayout(cl);
    ao->setAudioFormat(af);
    if (!ao->open()) {
        qWarning("open audio failed");
        return;
    }
}

void MainWindow::changeAudioTrack(QAction *action)
{
    int track = action->data().toInt();
    if (mpAudioTrackAction == action && track >= 0) { // external action is always clickable
        action->toggle();
        return;
    }
    if (track < 0) {
        QString f = QFileDialog::getOpenFileName(0, tr("Open an external audio track"));
        if (f.isEmpty()) {
            action->toggle();
            return;
        }
        mpPlayer->setExternalAudio(f);
    } else {
        mpPlayer->setExternalAudio(QString());
        if (!mpPlayer->setAudioStream(track)) {
            action->toggle();
            return;
        }
    }

    if (mpAudioTrackAction)
        mpAudioTrackAction->setChecked(false);
    mpAudioTrackAction = action;
    mpAudioTrackAction->setChecked(true);
    if (mpStatisticsView && mpStatisticsView->isVisible())
        mpStatisticsView->setStatistics(mpPlayer->statistics());
}

void MainWindow::changeVO(QAction *action)
{
    if (action == mpVOAction) {
        action->toggle(); //check state changes if clicked
        return;
    }
    VideoRendererId vid = (VideoRendererId)action->data().toInt();
    VideoRenderer *vo = VideoRenderer::create(vid);
    if (vo && vo->isAvailable()) {
        if (!setRenderer(vo))
            action->toggle();
    } else {
        action->toggle(); //check state changes if clicked
        QMessageBox::critical(0, QString::fromLatin1("QtAV"), tr("not availabe on your platform!"));
        return;
    }
}

void MainWindow::processPendingActions()
{
    if (mHasPendingPlay) {
        mHasPendingPlay = false;
        play(mFile);
    }
}

void MainWindow::setAudioBackends(const QStringList& backends)
{
    mAudioBackends = backends;
}

bool MainWindow::setRenderer(QtAV::VideoRenderer *renderer)
{
    if (!renderer)
        return false;
    if (!renderer->widget()) {
        QMessageBox::warning(0, QString::fromLatin1("QtAV"), tr("Can not use this renderer"));
        return false;
    }
    mpOSD->uninstall();
    mpSubtitle->uninstall();
    renderer->widget()->setMouseTracking(true); //mouseMoveEvent without press.
    mpPlayer->setRenderer(renderer);
    QWidget *r = 0;
    if (mpRenderer)
        r = mpRenderer->widget();
    //release old renderer and add new
    if (r) {
        mpPlayerLayout->removeWidget(r);
        if (r->testAttribute(Qt::WA_DeleteOnClose)) {
            r->close();
        } else {
            r->close();
            delete r;
        }
        r = 0;
    }
    mpRenderer = renderer;
    //setInSize?
    mpPlayerLayout->addWidget(renderer->widget());
    if (mpVOAction) {
        mpVOAction->setChecked(false);
    }
    foreach (QAction *action, mVOActions) {
        if (action->data() == renderer->id()) {
            mpVOAction = action;
            break;
        }
    }
    mpVOAction->setChecked(true);
    //mpTitle->setText(mpVOAction->text()); //was crash of player here after merge 2015-01-27
    const VideoRendererId vid = mpPlayer->renderer()->id();
    if (vid == VideoRendererId_GLWidget
            || vid == VideoRendererId_GLWidget2
            || vid == VideoRendererId_OpenGLWidget
            ) {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale << VideoEQConfigPage::GLSL);
        mpVideoEQ->setEngine(VideoEQConfigPage::GLSL);
        mpPlayer->renderer()->forcePreferredPixelFormat(true);
        installShaderXuno();
    } else if (vid == VideoRendererId_XV) {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::XV);
        mpVideoEQ->setEngine(VideoEQConfigPage::XV);
        mpPlayer->renderer()->forcePreferredPixelFormat(true);
    } else {
        mpVideoEQ->setEngines(QVector<VideoEQConfigPage::Engine>() << VideoEQConfigPage::SWScale);
        mpVideoEQ->setEngine(VideoEQConfigPage::SWScale);
        mpPlayer->renderer()->forcePreferredPixelFormat(false);
    }
    onVideoEQEngineChanged();
    mpOSD->installTo(mpRenderer);
    mpSubtitle->installTo(mpRenderer);

    onUserShaderChanged();
#define GL_ASS 0
#if GL_ASS
    GLSLFilter* glsl = new GLSLFilter(this);
    glsl->setOutputSize(QSize(4096, 2160));
    //mpRenderer->installFilter(glsl);
    if (mpRenderer->opengl()) {
        connect(mpRenderer->opengl(), &OpenGLVideo::beforeRendering, [this](){
            OpenGLVideo* glv = mpRenderer->opengl();
            glv->setSubImages(mpSubtitle->subImages(glv->frameTime(), glv->frameWidth(), glv->frameHeight()));
        });
    }
#endif
    return true;
}

void MainWindow::play(const QString &name)
{
    mFile = name;
    if (!mIsReady) {
        mHasPendingPlay = true;
        return;
    }
    mTitle = mFile;
    if (!mFile.contains(QLatin1String("://")) || mFile.startsWith(QLatin1String("file://"))) {
        mTitle = QFileInfo(mFile).fileName();
    }
    if (isFileImgageSequence()) {
        mTitle = QString("Sequence of images: %1/%2").arg(QFileInfo(mFile).dir().dirName()).arg(QFileInfo(mFile).fileName());
        //toggleRepeat(true);
    }
    if (mFile.startsWith("http://")){
        mTitle = QString("http://%1/.../%2").arg(QUrl(mFile).host(),QString(QUrl(mFile).fileName()));
    }else if (mFile.startsWith("https://")){
        mTitle = QString("httsp://%1/.../%2").arg(QUrl(mFile).host(),QString(QUrl(mFile).fileName()));
    }


    setWindowTitle(mTitle);
    mpPlayer->stop(); //if no stop, in setPriority decoder will reopen
    mpPlayer->setFrameRate(Config::instance().forceFrameRate());
    if (!mAudioBackends.isEmpty())
        mpPlayer->audio()->setBackends(mAudioBackends);
    if (!mpRepeatEnableAction->isChecked())
        mRepeateMax = 0;
    else
        mRepeateMax = (mpRepeatLoop->isChecked())?-1:mpRepeatBox->value()-1;
    qDebug()<<"mRepeateMax"<<mRepeateMax;
    mpPlayer->setInterruptOnTimeout(Config::instance().abortOnTimeout());
    mpPlayer->setInterruptTimeout(Config::instance().timeout()*1000.0);
    mpPlayer->setBufferMode(QtAV::BufferPackets);
    if (isFileImgageSequence())
        mpPlayer->setBufferValue(Config::instance().bufferValueI());
    else
        mpPlayer->setBufferValue(Config::instance().bufferValue());
    mpPlayer->setRepeat(mRepeateMax);
    mpPlayer->setPriority(idsFromNames(Config::instance().decoderPriorityNames()));
    mpPlayer->setOptionsForAudioCodec(mpDecoderConfigPage->audioDecoderOptions());
    mpPlayer->setOptionsForVideoCodec(mpDecoderConfigPage->videoDecoderOptions());

    if (isFileImgageSequence()){
        applyCustomFPS(); //set options  avformatOptions included
    }else{
        if (Config::instance().avformatOptionsEnabled())
            mpPlayer->setOptionsForFormat(Config::instance().avformatOptions());
    }

    setClockType();

    qDebug() << Config::instance().avformatOptions();
    PlayListItem item;
    item.setUrl(mFile);
    item.setTitle(mTitle);
    item.setLastTime(0);
    mpHistory->remove(mFile);
    mpHistory->insertItemAt(item, 0);
    loadRemoteUrlPresset(mFile);
    mpImgSeqExtract->setMovieName(name);
    mpPlayer->play(name);
}

void MainWindow::play(const QUrl &url)
{
    play(QUrl::fromPercentEncoding(url.toEncoded()));
}

void MainWindow::play()
{
    mpPlayer->play();
    mpPlayPauseBtn->setIcon(QIcon(":/theme/dark/pause.svg"));
}


void MainWindow::setFpsSequenceFrame(const double fps)
{
    if (mpImageSequence && fps>0) mpImageSequence->setFPS(fps);
}

void MainWindow::setStartSequenceFrame(const quint32 sf)
{
    if (mpImageSequence && sf>0) mpImageSequence->setStartFrame(sf);
}

void MainWindow::setEndSequenceFrame(const quint32 ef)
{
    if (mpImageSequence && ef>0) mpImageSequence->setEndFrame(ef);
}

void MainWindow::setRepeatLoop(const bool loop)
{
    if (mpRepeatLoop) {
        mpRepeatLoop->setChecked(loop);
    }
    if (mpImageSequence)
        mpImageSequence->setRepeatLoop(loop);

}

void MainWindow::setPlayerScale(const double scale)
{
    if (scale>0) {
        mPlayerScale=scale;
        mNaitiveScaleOn=false;
        this->setMaximumSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
        //reset wheelzoom of player
        //emit wheelEvent(new QWheelEvent(QPoint(0,0),0,Qt::NoButton,Qt::NoModifier));
        if (mpRenderer) {
            QSize videoFrame=mpRenderer->videoFrameSize();
            if (mPlayerScale>0.) videoFrame*=mPlayerScale;
            mpRenderer->setRegionOfInterest(QRectF(QPointF(0,0), videoFrame));
        }
    }
}

void MainWindow::setFileName(const QString fname)
{
    if (!fname.isEmpty()){
        mFile=fname;
        qDebug()<<"Name filename"<<fname;
        if (isFileImgageSequence() && mpImageSequence){
            mpImageSequence->setImageSequenceFileName(fname);
        }
    }
}

void MainWindow::setExtractTask(const QString fname){
    if (!fname.isEmpty()){
        qDebug()<<"MainWindow :: setExtractTask"<<fname;
        onImageSequenceToogledFrameExtractor(true);
        if (mpImgSeqExtract) {
            mpImgSeqExtract->setOutputPath(fname);
        }
    }
}

void MainWindow::setVideoDecoderNames(const QStringList &vd)
{
    QStringList vdnames;
    foreach (const QString& v, vd) {
        vdnames << v.toLower();
    }
    QStringList vidp;
    QStringList vids = idsToNames(VideoDecoder::registered());
    foreach (const QString& v, vids) {
        if (vdnames.contains(v.toLower())) {
            vidp.append(v);
        }
    }
    Config::instance().setDecoderPriorityNames(vidp);
}

void MainWindow::openFile()
{
    QString file = QFileDialog::getOpenFileName(0, tr("Open a media file"), Config::instance().lastFile());
    if (file.isEmpty())
        return;
    Config::instance().setLastFile(file);
    play(file);
}

void MainWindow::togglePlayPause()
{
    if (mpPlayer->isPlaying()) {
        qDebug("isPaused = %d", mpPlayer->isPaused());
        mpPlayer->pause(!mpPlayer->isPaused());
    } else {
        if (mFile.isEmpty())
            return;
        applyCustomFPS();
        if (!mpPlayer->isPlaying())
            play(mFile);
        else
            mpPlayer->play();
        mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/pause.svg")));
    }
}

void MainWindow::pause()
{
    if (mpPlayer->isPlaying()) {
        qDebug("isPaused = %d", mpPlayer->isPaused());
        mpPlayer->pause(true);
    }
    if (mpImgSeqExtract) {
        Statistics st=mpPlayer->statistics();
        mpImgSeqExtract->setOutputDimension(QSize(st.video_only.width,st.video_only.height));
    }
}

void MainWindow::showNextOSD()
{
    if (!mpOSD)
        return;
    mpOSD->useNextShowType();
}

void MainWindow::onSpinBoxChanged(double v)
{
    if (!mpPlayer)
        return;
    mpPlayer->setSpeed(v);
}

void MainWindow::onPaused(bool p)
{
    if (p) {
        qDebug("start pausing...");
        mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/play.svg")));
        if (mpImgSeqExtract) mpImgSeqExtract->onPaused();
    } else {
        qDebug("stop pausing...");
        mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/pause.svg")));
    }
}

void MainWindow::onStartPlay()
{
    //--- TODO --- remove after recover OpenGL rgb48le
    //#if !IMGSEQOPENGL
    //    bool rgb48=mpPlayer->statistics().video_only.pix_fmt.contains("rgb48be");
    //    VideoRenderer *vo = VideoRendererFactory::create( (isFileImgageSequence() && rgb48) ? VideoRendererId_Widget : VideoRendererId_GLWidget2);
    //    if (vo && vo->isAvailable()) {
    //        setRenderer(vo);
    //    }
    //#endif

    mpRenderer->setRegionOfInterest(QRectF());
    mFile = mpPlayer->file(); //open from EventFilter's menu
    mTitle = mFile;
    if (!mFile.contains(QLatin1String("://")) || mFile.startsWith(QLatin1String("file://")))
        mTitle = QFileInfo(mFile).fileName();
    setWindowTitle(mTitle);

    mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/pause.svg")));
    mpTimeSlider->clearLimits();
    mpTimeSlider->setMinimum(mpPlayer->mediaStartPosition());
    mpTimeSlider->setMaximum(mpPlayer->mediaStopPosition());
    setPlayerPosFromRepeat();
    mpTimeSlider->setValue(0);
    mpTimeSlider->setEnabled(mpPlayer->isSeekable());
    mpEnd->setText(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()).toString(QString::fromLatin1("HH:mm:ss")));
    setVolume();
    mShowControl = 0;
    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
    ScreenSaver::instance().disable();
    initAudioTrackMenu();

    if (!isFileImgageSequence()){
        mpRepeatA->clearMinimumTime();
        mpRepeatA->clearMaximumTime();
        mpRepeatB->clearMinimumTime();
        mpRepeatB->clearMaximumTime();

        mpRepeatA->setMinimumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStartPosition()));
        mpRepeatA->setMaximumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
        mpRepeatB->setMinimumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStartPosition()));
        mpRepeatB->setMaximumTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
        if (mpRepeatAction->isChecked()) {
            mpRepeatA->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->startPosition()));
            mpRepeatB->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->stopPosition()));
        }else{
            mpRepeatA->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStartPosition()));
            mpRepeatB->setTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));

        }
    }else{
        repeatAChanged(mpRepeatA->time());
        repeatBChanged(mpRepeatB->time());
    }
    mCursorTimer = startTimer(3000);
    PlayListItem item = mpHistory->itemAt(0);
    item.setUrl(mFile);
    item.setTitle(mTitle);
    item.setDuration(mpPlayer->duration());
    mpHistory->setItemAt(item, 0);
    updateChannelMenu();

    if (mpStatisticsView && mpStatisticsView->isVisible())
        mpStatisticsView->setStatistics(mpPlayer->statistics());
    analyeUsedFPS();
    if (mpImgSeqExtract) mpImgSeqExtract->setEndTime(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()));
    scaleReset();
    reSizeByMovie();
}

void MainWindow::onStopPlay()
{
    mpPlayer->setPriority(idsFromNames(Config::instance().decoderPriorityNames()));
    if (mpPlayer->currentRepeat() >= 0 && mpPlayer->currentRepeat() < mpPlayer->repeat())
        return;
    // use shortcut to replay in EventFilter, the options will not be set, so set here
    mpPlayer->setFrameRate(Config::instance().forceFrameRate());
    mpPlayer->setOptionsForAudioCodec(mpDecoderConfigPage->audioDecoderOptions());
    mpPlayer->setOptionsForVideoCodec(mpDecoderConfigPage->videoDecoderOptions());
    if (Config::instance().avformatOptionsEnabled())
        mpPlayer->setOptionsForFormat(Config::instance().avformatOptions());

    mpPlayPauseBtn->setIcon(QIcon(QString::fromLatin1(":/theme/dark/play.svg")));
    mpTimeSlider->setValue(0);
    qDebug(">>>>>>>>>>>>>>disable slider");
    mpTimeSlider->setDisabled(true);
    mpTimeSlider->setMinimum(0);
    mpTimeSlider->setMaximum(0);
    mpCurrent->setText(QString::fromLatin1("00:00:00"));
    mpEnd->setText(QString::fromLatin1("00:00:00"));
    if (mpImgSeqExtract){
        mpImgSeqExtract->setEndTime(QTime(0, 0, 0));
        mpImgSeqExtract->setStartTime(QTime(0, 0, 0));
    }
    tryShowControlBar();
    ScreenSaver::instance().enable();
    //toggleRepeat(mpRepeatEnableAction->isChecked());  //after stop not reset repeat task
    //mRepeateMax = 0;
    killTimer(mCursorTimer);
    unsetCursor();
    if (m_preview)
        m_preview->setFile(QString());
}

void MainWindow::onSpeedChange(qreal speed)
{
    mpSpeed->setText(QString::fromLatin1("%1").arg(speed, 4, 'f', 2, QLatin1Char('0')));
}

void MainWindow::setFrameRate()
{
    if (!mpPlayer)
        return;
    mpPlayer->setFrameRate(Config::instance().forceFrameRate());
}

void MainWindow::seek(int value)
{
    mpPlayer->setSeekType(AccurateSeek);
    mpPlayer->seek((qint64)value);
    if (!m_preview || !Config::instance().previewEnabled())
        return;
    m_preview->setTimestamp(value);
    m_preview->preview();
    m_preview->setWindowFlags(Qt::Tool |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    m_preview->resize(Config::instance().previewWidth(), Config::instance().previewHeight());
    m_preview->show();
    if (mpImgSeqExtract) mpImgSeqExtract->setStartTime(QTime(0,0,0).addMSecs((qint64)mpTimeSlider->value()));
}

void MainWindow::seek(qint64 msec)
{
    mpPlayer->seek(msec);
}
void MainWindow::seek(QTime time)
{
    mpPlayer->seek((qint64)QTime(0,0,0).msecsTo(time));
}

void MainWindow::seek()
{
    seek(mpTimeSlider->value());
}

void MainWindow::showHideVolumeBar()
{
    if (mpVolumeSlider->isHidden()) {
        mpVolumeSlider->show();
    } else {
        mpVolumeSlider->hide();
    }
}

void MainWindow::setVolume()
{
    AudioOutput *ao = mpPlayer ? mpPlayer->audio() : 0;
    //qreal v = qreal(mpVolumeSlider->value())*kVolumeInterval;
    qreal v = qreal(mpVolumeSlider->value())/100.;
    if (ao) {
        if (qAbs(int(ao->volume()/kVolumeInterval) - mpVolumeSlider->value()) >= int(0.1/kVolumeInterval)) {
            ao->setVolume(v);
        }
    }
    mpVolumeSlider->setToolTip(QString::number(v));
    mpVolumeBtn->setToolTip(QString::number(v));
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    if (mpPlayer)
        mpPlayer->stop();
    qApp->quit();
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    QWidget::resizeEvent(e);
    //e->ignore();
   // qDebug()<<"MainWindow::resizeEvent"<<e->size()<<e->oldSize();

    //calcToUseSuperResolution();
    if (mpRenderer && mPlayerScale<=1.5) {
        QSize framesize=mpRenderer->videoFrameSize();
        //qDebug()<<"MainWindow::resizeEvent size:"<<mpRenderer->videoFrameSize()<<mpRenderer->rendererSize()<<mpRenderer->widget()->size();
        if (!framesize.isEmpty()) this->setMaximumSize(framesize*mPlayerScale);
    }else{
        this->setMaximumSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
    }
    /*
    if (mpTitle)
        QLabelSetElideText(mpTitle, QFileInfo(mFile).fileName(), e->size().width());
    */
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == mCursorTimer) {
        if (mpControl->isVisible())
            return;
        setCursor(Qt::BlankCursor);
    }
}

void MainWindow::onPositionChange(qint64 pos)
{
    if (mpPlayer->isSeekable())
        mpTimeSlider->setValue(pos);
    mpCurrent->setText(QTime(0, 0, 0).addMSecs(pos).toString(QString::fromLatin1("HH:mm:ss")));
    if (mpImgSeqExtract) {
        mpImgSeqExtract->setStartTime(QTime(0, 0, 0).addMSecs(pos));
        if (mpImgSeqExtract->regionPlaying() && mpImgSeqExtract->EndPosExtract() && pos >= mpImgSeqExtract->EndPosExtract()) pause();
    }
    //setWindowTitle(QString::number(mpPlayer->statistics().video_only.currentDisplayFPS(), 'f', 2).append(" ").append(mTitle));
}

void MainWindow::repeatAChanged(const QTime& t)
{
    //qDebug()<<"repeatAChanged start"<<t;
    mpRepeatA->setTime(t);
    if (!mpPlayer)
        return;
    mpPlayer->setStartPosition(QTime(0, 0, 0).msecsTo(t));
    mpTimeSlider->setVisualMinLimit(QTime(0, 0, 0).msecsTo(t));
    //qDebug()<<"repeatAChanged end"<<t;
}

void MainWindow::repeatBChanged(const QTime& t)
{
    // when this slot is called? even if only range is set?
    //qDebug()<<"repeatBChanged start"<<t;
    if (t <= mpRepeatA->time())
        return;
    mpRepeatB->setTime(t);
    if (!mpPlayer)
        return;
    mpPlayer->setStopPosition(QTime(0, 0, 0).msecsTo(t));
    mpTimeSlider->setVisualMaxLimit(QTime(0, 0, 0).msecsTo(t));
    //qDebug()<<"repeatBChanged end"<<t;
}

void MainWindow::setTimeSliderVisualMinLimit(const QTime& t)
{
    mpTimeSlider->setVisualMinLimit(QTime(0, 0, 0).msecsTo(t));
    mpTimeSlider->setVisibleVisualLimit(true);
}
void MainWindow::setTimeSliderVisualMaxLimit(const QTime& t)
{
    qint64 p=QTime(0, 0, 0).msecsTo(t);
    mpTimeSlider->setVisualMaxLimit(p);
    mpTimeSlider->setVisibleVisualLimit(p!=0);
}


void MainWindow::keyPressEvent(QKeyEvent *e)
{
    mControlOn = e->key() == Qt::Key_Control;
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    Q_UNUSED(e);
    mControlOn = false;
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //if (!mControlOn)
    //    return;
    mGlobalMouse = e->globalPos();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    mGlobalMouse = QPointF();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    unsetCursor();
    if (e->pos().y() > height() - mpTimeSlider->height() - mpControl->height()) {
        if (mShowControl == 0) {
            mShowControl = 1;
            tryShowControlBar();
        }
    } else {
        if (mShowControl == 1) {
            mShowControl = 0;
            QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
        }
    }
    if (!mGlobalMouse.isNull()  && (e->buttons() & Qt::LeftButton) ) {
        if (!mpRenderer || !mpRenderer->widget())
            return;
        QRectF roi = mpRenderer->realROI();

        //roi.setTopLeft(roi.topLeft());
        //roi.setSize(roi.size());

        QPointF delta = e->globalPos() - mGlobalMouse;
        mGlobalMouse=e->globalPos();
        QPointF center=roi.center()-delta;
        roi.moveCenter(center);
        if (mNaitiveScaleOn){
            //qDebug()<<"mouseMoveEvent mNaitiveScaleOn";
            //qDebug()<<"mouseMoveEvent roi"<<roi<<"rendererSize"<<mpRenderer->rendererSize();
            QRectF roi2=roi;
//            roi2.setTopLeft(roi2.topLeft()*mPlayerScale);
//            roi2.setSize(roi2.size()*mPlayerScale);

            QSize videoFrame=mpRenderer->videoFrameSize();
            if (mPlayerScale>0.) videoFrame*=mPlayerScale;

            QRectF roi3=QRectF(QPointF(0,0), videoFrame);

            ///qDebug()<<"mouseMoveEvent scaled roi"<<roi<<"rendererSize"<<mpRenderer->rendererSize()<<"roi2"<<roi2<<"roi3"<<roi3<<"r3 intersects r2"<<roi3.intersects(roi2);
            if (roi2.top()>1 && roi2.left()>1 && roi2.right() < roi3.right() && roi2.bottom() < roi3.bottom()){
                mpRenderer->setRegionOfInterest(roi2);
            }
        }else{
            //qDebug()<<"mouseMoveEvent roi"<<roi<<"rendererSize"<<mpRenderer->rendererSize();

            if (roi.top()>1 && roi.left()>1 && roi.right() < mpRenderer->rendererWidth() && roi.bottom() < mpRenderer->rendererHeight()){
                mpRenderer->setRegionOfInterest(roi);
            }
        }
    }
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    if (!mpRenderer || !mpRenderer->widget()) {
        return;
    }
    QPoint dp;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qreal deg = e->angleDelta().y()/8;
    dp = e->pixelDelta();
#else
    qreal deg = e->delta()/8;
#endif //QT_VERSION
#if WHEEL_SPEED
    if (!mControlOn) {
        qreal speed = mpPlayer->speed();
        mpPlayer->setSpeed(qMax(0.01, speed + deg/15.0*0.02));
        return;
    }
#endif //WHEEL_SPEED
    QSize videoFrame=mpRenderer->videoFrameSize();
    if (mPlayerScale>0.) videoFrame*=mPlayerScale;
    QPointF p = mpRenderer->widget()->mapFrom(this, e->pos());
    QPointF fp = mpRenderer->mapToFrame(p);
    //qDebug() <<  p << fp;
    if (fp.x() < 0)
        fp.setX(0);
    if (fp.y() < 0)
        fp.setY(0);
    if (fp.x() > videoFrame.width())
        fp.setX(videoFrame.width());
    if (fp.y() > videoFrame.height())
        fp.setY(videoFrame.height());
    QRectF viewport = QRectF(mpRenderer->mapToFrame(QPointF(0, 0)), mpRenderer->mapToFrame(QPointF(mpRenderer->rendererWidth(), mpRenderer->rendererHeight())));
    //qDebug("vo: (%.1f, %.1f)=> frame: (%.1f, %.1f)", p.x(), p.y(), fp.x(), fp.y());
    qreal zoom = 1.0 + deg*3.14/180.0;
    if (!dp.isNull()) {
        zoom = 1.0 + (qreal)dp.y()/100.0;
    }
    static qreal z = 1.0;
    z *= zoom;
    if (z < 1.0)
        z = 1.0;
    qreal x0 = fp.x() - fp.x()/z;
    qreal y0 = fp.y() - fp.y()/z;
    //qDebug() << "fr: " << QRectF(x0, y0, qreal(mpRenderer->videoFrameSize().width())/z, qreal(mpRenderer->videoFrameSize().height())/z) << fp << z;
    mpRenderer->setRegionOfInterest(QRectF(x0, y0, qreal(videoFrame.width())/z, qreal(videoFrame.height())/z));
    mNaitiveScaleOn=false;
    return;
    QTransform m;
    m.translate(fp.x(), fp.y());
    m.scale(1.0/zoom, 1.0/zoom);
    m.translate(-fp.x(), -fp.y());
    QRectF r = m.mapRect(mpRenderer->realROI());
    mpRenderer->setRegionOfInterest((r | m.mapRect(viewport))&QRectF(QPointF(0,0), videoFrame));
}

QString MainWindow::aboutXunoQtAV_PlainText()
{
    return aboutXunoQtAV_HTML().remove(QRegExp(QStringLiteral("<[^>]*>")));
}

QString MainWindow::aboutXunoQtAV_HTML()
{
    static QString about = "<h3>XunoPlayer-QtAV " XUNO_QTAV_VERSION_STR_LONG "</h3>\n"
                           "<p>" + QObject::tr("Fork project (Xuno-QtAV) of QtAV \n") + QTAV_VERSION_STR "</p>";
    return about;
}


QUrl MainWindow::remove_fistsubdomain(QUrl url)
{
    QString host = url.host();
    QStringList hosts=host.split('.');
    hosts.removeFirst();
    url.setHost(hosts.join('.'));
    return url;
}

bool MainWindow::same_site_domain(const QUrl &url1, const QUrl &url2)
{
    return (remove_fistsubdomain(url1).host() == remove_fistsubdomain(url2).host());
}
void MainWindow::calcToUseSuperResolution()
{
   // qDebug()<<"MainWindow::calcToUseSuperResolution";
    if (mpRenderer && mpPlayer) {
        Statistics st=mpPlayer->statistics();
        QSize framesize,rendersize;

        if (st.video_only.width>0 && st.video_only.height>0){
            framesize.setWidth(st.video_only.width);
            framesize.setHeight(st.video_only.height);
        }else{
            framesize=mpRenderer->videoFrameSize();
        }
        rendersize=mpRenderer->videoRect().size();//-QSize(1,1);//widget()->size();

        if (framesize.isEmpty()) {
            //qDebug()<<"MainWindow::calcToUseSuperResolution framesize.isEmpty";
            return;
        }
        if (rendersize.isEmpty()) {
//            qDebug()<<"MainWindow::calcToUseSuperResolution rendersize.isEmpty()"<<mpRenderer->videoRect().size();
//            return;
            rendersize=framesize;
        }

        qDebug()<<"MainWindow::calcToUseSuperResolution size: framesize:"<<framesize<<"rendersize:"<<rendersize;
        qreal sscaleWidth=qreal(rendersize.width())/qreal(framesize.width());
        qreal sscaleHeight=qreal(rendersize.height())/qreal(framesize.height());
        //qDebug()<<"MainWindow::calcToUseSuperResolution opengl()->video_size"<<mpRenderer
        qreal scale=(sscaleWidth+sscaleHeight)/2.;
        scale=ceil((scale)*100)/100.;
        qDebug()<<"MainWindow::calcToUseSuperResolution Scale WxH:"<<sscaleWidth<<sscaleHeight<<"Middle:"<<scale;

        needToUseSuperResolutionLastLinearFiltering=true;
        needToUseSuperResolution=true;

        if (scale<=1.){
            needToUseSuperResolution=false;
        }else if(scale>1.){
            needToUseSuperResolution=true;
        }

        if ((scale>=1.5) || (framesize.width()>=1920.)){
            needToUseFXAAFiltering=true;
        }else{
            needToUseFXAAFiltering=false;
        }
        qDebug()<<"needToUseFXAAFiltering:"<<needToUseFXAAFiltering;

//        if (scale==2.){
//            needToUseSuperResolutionLastLinearFiltering=false;
//        }

        //limit of upper size for frame size more than 1920 pix
        if (framesize.width()>1920 || framesize.height()>1920) {
            qDebug()<<"superscale skipped >1920"<<framesize;
            needToUseSuperResolution=false;
        }

        qDebug()<<"needToUseSuperResolution"<<needToUseSuperResolution;
        qDebug()<<"needToUseSuperResolutionLastLinearFiltering"<<needToUseSuperResolutionLastLinearFiltering;


        if (mpGLSLFilter!=Q_NULLPTR){
            mpGLSLFilter->setNeedSuperScale(needToUseSuperResolution);
            mpGLSLFilter->setNeedSuperScaleLastLinearFiltering(needToUseSuperResolutionLastLinearFiltering);
            mpGLSLFilter->setNeedToUseFXAAFiltering(needToUseFXAAFiltering);
        }

        //calculate tunes for XunoSharp values
        const float sharpScaler_x1=0.0001562f;
        const float sharpScaler_x2=0.0002604f;
        const double brightnessScaler=0.0000000;
        const double contrastScaler=0.0000625;
        const double saturationScaler=0.0000312;
        const float gammaScaler=0.0000625f;

        float sharpValue=0.f,gammaValue=0.f;
        double contrastValue=0.,brightnessValue=0.,saturationValue=0.;

        if (scale==1.){
          sharpValue=(framesize.width()*sharpScaler_x1);
        }else if (scale>1.){
            if (scale<2.){
              sharpValue=(framesize.width()*(sharpScaler_x1+sharpScaler_x2*(scale-1.f)));
              gammaValue=framesize.width()*gammaScaler*(scale-1.f);
              contrastValue=framesize.width()*contrastScaler*(scale-1.f);
              brightnessValue=framesize.width()*brightnessScaler*(scale-1.f);
              saturationValue=framesize.width()*saturationScaler*(scale-1.f);
            }else{
              sharpValue=(framesize.width()*(sharpScaler_x1+sharpScaler_x2));
              gammaValue=framesize.width()*gammaScaler;
              contrastValue=framesize.width()*contrastScaler;
              brightnessValue=framesize.width()*brightnessScaler;
              saturationValue=framesize.width()*saturationScaler;
            }
        }

        if (shaderXuno && mpVideoEQ) {
            if (1) {
                qreal userValue=mpVideoEQ->filterSharp();
                qreal correctedValue=userValue+sharpValue;
                shaderXuno->setSharpValue(correctedValue);
                qDebug()<<"Set XunoSharp Value (corrected,user,correction)"<<correctedValue<<userValue<<sharpValue;
            }
            if (gammaValue>=0.f) {
                qreal userValue=mpVideoEQ->gammaRGB();
                qreal correctedValue=userValue+gammaValue;
                shaderXuno->setGammaValue(correctedValue);
                qDebug()<<"Set XunoGamma Value (corrected,user,correction)"<<correctedValue<<userValue<<gammaValue;
            }
        }

        if (shaderXuno && mpVideoEQ) {//mpGLSLFilter
            if (contrastValue>=0.) {
                qreal userValue=mpVideoEQ->contrast();
                qreal correctedValue=userValue+contrastValue;
                mpGLSLFilter->setContrast(correctedValue);
                qDebug()<<"Set XunoContrast Value (corrected,user,correction)"<<correctedValue<<userValue<<contrastValue;
            }
            if (brightnessValue>=0.) {
                qreal userValue=mpVideoEQ->brightness();
                qreal correctedValue=userValue+brightnessValue;
                mpGLSLFilter->setBrightness(correctedValue);
                qDebug()<<"Set XunoBrightness Value (corrected,user,correction)"<<correctedValue<<userValue<<brightnessValue;
            }
            if (saturationValue>=0.) {
                qreal userValue=mpVideoEQ->saturation();
                qreal correctedValue=userValue+saturationValue;
                mpGLSLFilter->setSaturation(correctedValue);
                qDebug()<<"Set XunoSaturation Value (corrected,user,correction)"<<correctedValue<<userValue<<saturationValue;
            }
        }





    }
}

void MainWindow::about()
{
    //QtAV::about();
    //we should use new because a qobject will delete it's children
    QTextBrowser *viewQtAV = new QTextBrowser;
    QTextBrowser *viewFFmpeg = new QTextBrowser;
    viewQtAV->setOpenExternalLinks(true);
    viewFFmpeg->setOpenExternalLinks(true);
    viewQtAV->setHtml(aboutXunoQtAV_HTML().append(aboutQtAV_HTML()));
    viewFFmpeg->setHtml(aboutFFmpeg_HTML());
    QTabWidget *tab = new QTabWidget;
    tab->addTab(viewQtAV, QStringLiteral("Xuno-QtAV"));
    tab->addTab(viewFFmpeg, QStringLiteral("FFmpeg"));
    QPushButton *qbtn = new QPushButton(QObject::tr("About Qt"));
    QPushButton *btn = new QPushButton(QObject::tr("Ok"));
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btn);
    btnLayout->addStretch();
    btnLayout->addWidget(qbtn);
    btn->setFocus();
    QDialog dialog;
    dialog.setWindowTitle(QObject::tr("About") + QStringLiteral("  Xuno-QtAV"));
    QVBoxLayout *layout = new QVBoxLayout;
    dialog.setLayout(layout);
    layout->addWidget(tab);
    layout->addLayout(btnLayout);
    QObject::connect(qbtn, SIGNAL(clicked()), qApp, SLOT(aboutQt()));
    QObject::connect(btn, SIGNAL(clicked()), &dialog, SLOT(accept()));
    dialog.exec();
}

void MainWindow::help()
{
    QString name = QString::fromLatin1("help-%1.html").arg(QLocale::system().name());
    QFile f(qApp->applicationDirPath() + QString::fromLatin1("/") + name);
    if (!f.exists()) {
        f.setFileName(QString::fromLatin1(":/") + name);
    }
    if (!f.exists()) {
        f.setFileName(qApp->applicationDirPath() + QString::fromLatin1("/help.html"));
    }
    if (!f.exists()) {
        f.setFileName(QString::fromLatin1(":/help.html"));
    }
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open help-%s.html and help.html: %s", qPrintable(QLocale::system().name()), qPrintable(f.errorString()));
        return;
    }
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    QString text = ts.readAll();
    QMessageBox::information(0, tr("Help"), text);
}

void MainWindow::openUrl()
{
    QString url = QInputDialog::getText(0, tr("Open an url"), tr("Url"));
    if (url.isEmpty())
        return;
    play(url);
}


void MainWindow::updateChannelMenu()
{
    if (mpChannelAction)
        mpChannelAction->setChecked(false);
    AudioOutput *ao = mpPlayer ? mpPlayer->audio() : 0; //getAO()?
    if (!ao) {
        return;
    }
    AudioFormat::ChannelLayout cl = ao->audioFormat().channelLayout();
    QList<QAction*> as = mpChannelMenu->actions();
    foreach (QAction *action, as) {
        if (action->data().toInt() != (int)cl)
            continue;
        action->setChecked(true);
        mpChannelAction = action;
        break;
    }
}

void MainWindow::initAudioTrackMenu()
{
    int track = -2;
    QAction *a = 0;
    QList<QAction*> as;
    int tracks = 0;
    if (!mpPlayer) {
        a = mpAudioTrackMenu->addAction(tr("External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        mpAudioTrackAction = 0;
        goto end;
    }
    track = mpPlayer->currentAudioStream();
    as = mpAudioTrackMenu->actions();
    tracks = mpPlayer->audioStreamCount();
    if (mpAudioTrackAction && tracks == as.size()-1 && mpAudioTrackAction->data().toInt() == track)
        return;
    while (tracks + 1 < as.size()) {
        a = as.takeLast();
        mpAudioTrackMenu->removeAction(a);
        delete a;
    }
    if (as.isEmpty()) {
        a = mpAudioTrackMenu->addAction(tr("External"));
        a->setData(-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
        mpAudioTrackAction = 0;
    }
    while (tracks + 1 > as.size()) {
        a = mpAudioTrackMenu->addAction(QString::number(as.size()-1));
        a->setData(as.size()-1);
        a->setCheckable(true);
        a->setChecked(false);
        as.push_back(a);
    }
end:
    foreach(QAction *ac, as) {
        if (ac->data().toInt() == track && track >= 0) {
            if (mpPlayer && mpPlayer->externalAudio().isEmpty()) {
                qDebug("track found!!!!!");
                mpAudioTrackAction = ac;
                ac->setChecked(true);
            }
        } else {
            ac->setChecked(false);
        }
    }
    if (mpPlayer && !mpPlayer->externalAudio().isEmpty()) {
        mpAudioTrackAction = as.first();
    }
    if (mpAudioTrackAction)
        mpAudioTrackAction->setChecked(true);
}

void MainWindow::switchAspectRatio(QAction *action)
{
    qreal r = action->data().toDouble();
    if (action == mpARAction && r != -2) {
        action->toggle(); //check state changes if clicked
        return;
    }
    if (r == 0) {
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::VideoAspectRatio);
    } else if (r == -1) {
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::RendererAspectRatio);
    } else {
        if (r == -2)
            r = QInputDialog::getDouble(0, tr("Aspect ratio"), QString(), 1.0);
        mpPlayer->renderer()->setOutAspectRatioMode(VideoRenderer::CustomAspectRation);
        mpPlayer->renderer()->setOutAspectRatio(r);
    }
    mpARAction->setChecked(false);
    mpARAction = action;
    mpARAction->setChecked(true);
}

void MainWindow::toggleRepeat(bool r)
{
    mpRepeatEnableAction->setChecked(r);
    mpRepeatAction->defaultWidget()->setEnabled(r); //why need defaultWidget?
    if (r) {
        mRepeateMax = mpRepeatBox->value();
    } else {
        mRepeateMax = 0;
    }
    if (mpPlayer) {
        mpPlayer->setRepeat(mpRepeatLoop->isChecked()?-1:mRepeateMax-1);
        mpTimeSlider->setVisibleVisualLimit(r);

        if (r) {
            repeatAChanged(mpRepeatA->time());
            repeatBChanged(mpRepeatB->time());
        } else {
            mpPlayer->setTimeRange(0);
        }
    }
}

void MainWindow::setRepeateMax(int m)
{
    if (m!=0) mRepeateMax = m;
    if (m<1)  mpRepeatBox->setValue(1);
    if (mpPlayer) {
        mpPlayer->setRepeat(mpRepeatLoop->isChecked()?-1:m-1);
    }
}

void MainWindow::playOnlineVideo(QAction *action)
{
    mTitle = action->text();
    play(action->data().toString());
}

void MainWindow::onTVMenuClick()
{
    static TVView *tvv = new TVView;
    tvv->show();
    connect(tvv, SIGNAL(clicked(QString,QString)), SLOT(onPlayListClick(QString,QString)));
    tvv->setMaximumHeight(qApp->desktop()->height());
    tvv->setMinimumHeight(tvv->width()*2);
}

void MainWindow::onPlayListClick(const QString &key, const QString &value)
{
    mTitle = key;
    play(value);
}

void MainWindow::tryHideControlBar()
{
    if (mShowControl > 0) {
        return;
    }
    if ( (mpTimeSlider && mpTimeSlider->isHidden()) &&
         (mpControl && mpControl->isHidden())
         )
        return;

    qDebug()<<"tryHideControlBar";
    if (!detachedControl) mpControl->hide();
    if (mpControl->isHidden()) mpTimeSlider->hide();
    workaroundRendererSize();
//    if (mpRenderer) {
//        qDebug()<<"tryHideControlBar size:"<<mpRenderer->videoFrameSize()<<mpRenderer->rendererSize()<<mpRenderer->widget()->size();
//    }
    //calcToUseSuperResolution();
}

void MainWindow::tryShowControlBar()
{
    unsetCursor();
    if ((mpTimeSlider && mpTimeSlider->isHidden()) &&
            (mpControl && mpControl->isHidden())
            )
        mpTimeSlider->show();
    if (detachedControl)
        detachedControl->show();
    else
        mpControl->show();

//    if (mpRenderer) {
//        qDebug()<<"MainWindow::tryShowControlBar size:"<<mpRenderer->videoFrameSize()<<mpRenderer->rendererSize()<<mpRenderer->widget()->size();
//    }
    //calcToUseSuperResolution();

}

void MainWindow::showInfo()
{
    if (!mpStatisticsView)
        mpStatisticsView = new StatisticsView();
    if (mpPlayer)
        mpStatisticsView->setStatistics(mpPlayer->statistics());
    mpStatisticsView->show();
}

void MainWindow::onTimeSliderHover(int pos, int value)
{
    QPoint gpos;
    if (detachedControl){
        gpos = (detachedControl->pos() + QPoint(pos, 0));
    }else{
        gpos = mapToGlobal(mpTimeSlider->pos() + QPoint(pos, 0));
    }
    QToolTip::showText(gpos, QTime(0, 0, 0).addMSecs(value).toString(QString::fromLatin1("HH:mm:ss.zzz")));
    if (!Config::instance().previewEnabled())
        return;
    if (!m_preview)
        m_preview = new VideoPreviewWidget();

    if (!m_preview) return;

    m_preview->setFile(mpPlayer->file());
    m_preview->setTimestamp(value);
    m_preview->preview();
    const int w = Config::instance().previewWidth();
    const int h = Config::instance().previewHeight();
    m_preview->setWindowFlags(m_preview->windowFlags() |Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    m_preview->resize(w, h);
    m_preview->move(gpos - QPoint(w/2, h));
    m_preview->show();

}

void MainWindow::onTimeSliderLeave()
{
    /*if (m_preview && m_preview->isVisible())
         m_preview->hide();
         m_preview->hide();*/
    if (!m_preview)
    {
        return;
    }
    if (m_preview->isVisible())
    {
        m_preview->close();
    }
    delete m_preview;
    m_preview = Q_NULLPTR;
}

void MainWindow::handleError(const AVError &e)
{
    QMessageBox::warning(0, tr("Player error"), e.string());
}

void MainWindow::onMediaStatusChanged()
{
    QString status;
    AVPlayer *player = reinterpret_cast<AVPlayer*>(sender());
    if (!player) { //why it happens? reinterpret_cast  works.
        qWarning() << "invalid sender() " << sender() << player;
        return;
    }
    switch (player->mediaStatus()) {
    case NoMedia:
        status = tr("No media");
        break;
    case InvalidMedia:
        status = tr("Invalid meida");
        break;
    case BufferingMedia:
        status = tr("Buffering...");
        break;
    case BufferedMedia:
        status = tr("Buffered");
        break;
    case LoadingMedia:
        status = tr("Loading...");
        break;
    case LoadedMedia:
        status = tr("Loaded");
        break;
    case StalledMedia:
        status = tr("Stalled");
        break;
    default:
        status = QString();
        onStopPlay();
        break;
    }
    qDebug() << "status changed " << status;
    setWindowTitle(status + QString::fromLatin1(" ") + mTitle);
}

void MainWindow::onBufferProgress(qreal percent)
{
    const qreal bs = mpPlayer->bufferSpeed();
    QString s;
    if (bs > 1024*1024*1024)
        s = QString("%1G/s").arg(bs/1024.0/1024.0/1024.0, 6, 'f', 1);
    else if (bs > 1024*1024)
        s = QString("%1M/s").arg(bs/1024.0/1024.0, 6, 'f', 1);
    else if (bs > 1024)
        s = QString("%1K/s").arg(bs/1024.0, 6, 'f', 1);
    else
        s = QString("%1B/s").arg(bs, 6, 'f', 1);
    setWindowTitle(QString::fromLatin1("Buffering... %1% @%2 ").arg(percent*100.0, 5, 'f', 1).arg(s) + mTitle);
}

void MainWindow::onVideoEQEngineChanged()
{
    VideoRenderer *vo = mpPlayer->renderer();
    //vo->setRenderRAWImage(true);
    //connect(vo,SIGNAL(onRenderRAWImage(const uchar*,int,int,int)),this,SLOT(renderedRAWImage(const uchar*,int,int,int)));
    VideoEQConfigPage::Engine e = mpVideoEQ->engine();
    if (e == VideoEQConfigPage::SWScale
            && vo->id() != VideoRendererId_X11 // X11 scales in the renderer
            ) {
        vo->forcePreferredPixelFormat(true);
        vo->setPreferredPixelFormat(VideoFormat::Format_RGB32);
    } else {
        vo->forcePreferredPixelFormat(false);
    }
    onBrightnessChanged(mpVideoEQ->brightness()*100.0);
    onContrastChanged(mpVideoEQ->contrast()*100.0);
    onHueChanged(mpVideoEQ->hue()*100.0);
    onSaturationChanged(mpVideoEQ->saturation()*100.0);
    //onGammaRGBChanged(mpVideoEQ->gammaRGB()*100.0);
    //onFilterSharpChanged(mpVideoEQ->filterSharp()*100.0);
}

void MainWindow::onBrightnessChanged(int b)
{
    //    VideoRenderer *vo = mpPlayer->renderer();
    //    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
    //            && vo->setBrightness(mpVideoEQ->brightness())) {
    //        mpPlayer->setBrightness(0);
    //    } else {
    //        vo->setBrightness(0);
    //        mpPlayer->setBrightness(b);
    //    }
    Q_UNUSED(b);

    if (mpGLSLFilter) {
        mpGLSLFilter->setBrightness(mpVideoEQ->brightness());
    }else{
        VideoRenderer *vo = mpPlayer->renderer();
        vo->setBrightness(mpVideoEQ->brightness());
    }

    calcToUseSuperResolution();

}

void MainWindow::onContrastChanged(int c)
{
    //    VideoRenderer *vo = mpPlayer->renderer();
    //    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
    //            && vo->setContrast(mpVideoEQ->contrast())) {
    //        mpPlayer->setContrast(0);
    //    } else {
    //        vo->setContrast(0);
    //        mpPlayer->setContrast(c);
    //    }
    Q_UNUSED(c);
    if (mpGLSLFilter) {
        mpGLSLFilter->setContrast(mpVideoEQ->contrast());
    }else{
        VideoRenderer *vo = mpPlayer->renderer();
        vo->setContrast(mpVideoEQ->contrast());
    }

    calcToUseSuperResolution();
}

void MainWindow::onHueChanged(int h)
{
    //    VideoRenderer *vo = mpPlayer->renderer();
    //    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
    //            && vo->setHue(mpVideoEQ->hue())) {
    //        mpPlayer->setHue(0);
    //    } else {
    //        vo->setHue(0);
    //        mpPlayer->setHue(h);
    //    }
    Q_UNUSED(h);
    if (mpGLSLFilter) {
        mpGLSLFilter->setHue(mpVideoEQ->hue());
    }else{
        VideoRenderer *vo = mpPlayer->renderer();
        vo->setHue(mpVideoEQ->hue());
    }

    calcToUseSuperResolution();
}

void MainWindow::onSaturationChanged(int s)
{
    //    VideoRenderer *vo = mpPlayer->renderer();
    //    if (mpVideoEQ->engine() != VideoEQConfigPage::SWScale
    //            && vo->setSaturation(mpVideoEQ->saturation())) {
    //        mpPlayer->setSaturation(0);
    //    } else {
    //        vo->setSaturation(0);
    //        mpPlayer->setSaturation(s);
    //    }
    Q_UNUSED(s);
    if (mpGLSLFilter) {
        mpGLSLFilter->setSaturation(mpVideoEQ->saturation());
    }else{
        VideoRenderer *vo = mpPlayer->renderer();
        vo->setSaturation(mpVideoEQ->saturation());
    }

    calcToUseSuperResolution();
}

void MainWindow::onGammaRGBChanged(int g)
{
    Q_UNUSED(g);
    if (shaderXuno) shaderXuno->setGammaValue(mpVideoEQ->gammaRGB());

    calcToUseSuperResolution();
}

void MainWindow::onFilterSharpChanged(int fs)
{
    Q_UNUSED(fs);
    if (shaderXuno) shaderXuno->setSharpValue(mpVideoEQ->filterSharp());

    calcToUseSuperResolution();
}

void MainWindow::onCaptureConfigChanged()
{
    qDebug()<<"onCaptureConfigChanged";
    mpPlayer->videoCapture()->setCaptureDir(Config::instance().captureDir());
    mpPlayer->videoCapture()->setQuality(Config::instance().captureQuality());
    if (Config::instance().captureFormat().toLower() == QLatin1String("original")) {
        mpPlayer->videoCapture()->setOriginalFormat(true);
    } else {
        mpPlayer->videoCapture()->setOriginalFormat(false);
        mpPlayer->videoCapture()->setSaveFormat(Config::instance().captureFormat());
    }

    if (Config::instance().captureType()==Config::CaptureType::DecodedFrame){
        mpCaptureBtn->setToolTip(QString::fromLatin1("%1\n%2: %3\n%4: %5")
                                 .arg(tr("Capture video frame"))
                                 .arg(tr("Save to"))
                                 .arg(mpPlayer->videoCapture()->captureDir())
                                 .arg(tr("Format"))
                                 .arg(Config::instance().captureFormat()));
        disconnect(mpCaptureBtn, 0, 0, 0);
        connect(mpCaptureBtn, &QToolButton::clicked, mpPlayer->videoCapture(), &QtAV::VideoCapture::capture);
    }else{
        mpCaptureBtn->setToolTip(QString::fromLatin1("%1\n%2: %3\n%4: %5")
                                 .arg(tr("Capture video frame with post filter"))
                                 .arg(tr("Save to"))
                                 .arg(mpPlayer->videoCapture()->captureDir())
                                 .arg(tr("Format"))
                                 .arg(Config::instance().captureFormat()));
        disconnect(mpCaptureBtn, 0, 0, 0);
        connect(mpCaptureBtn, &QToolButton::clicked, this, &MainWindow::captureGL);
    }
}

void MainWindow::onAVFilterVideoConfigChanged()
{
    if (mpVideoFilter) {
        mpVideoFilter->uninstall();
        delete mpVideoFilter;
        mpVideoFilter = Q_NULLPTR;
    }
    mpVideoFilter = new LibAVFilterVideo(this);
    mpVideoFilter->setEnabled(Config::instance().avfilterVideoEnable());
    mpPlayer->installFilter(mpVideoFilter);
    mpVideoFilter->setOptions(Config::instance().avfilterVideoOptions());
}

void MainWindow::onAVFilterAudioConfigChanged()
{
    if (mpAudioFilter) {
        mpAudioFilter->uninstall();
        delete mpAudioFilter;
        mpAudioFilter = Q_NULLPTR;
    }
    mpAudioFilter = new LibAVFilterAudio(this);
    mpAudioFilter->setEnabled(Config::instance().avfilterAudioEnable());
    mpAudioFilter->installTo(mpPlayer);
    mpAudioFilter->setOptions(Config::instance().avfilterAudioOptions());
}

void MainWindow::donate()
{
    //QDesktopServices::openUrl(QUrl("https://sourceforge.net/p/qtav/wiki/Donate%20%E6%8D%90%E8%B5%A0/"));
    QDesktopServices::openUrl(QUrl(QString::fromLatin1("http://www.qtav.org/donate.html")));
}

void MainWindow::onBufferValueChanged()
{
    if (!mpPlayer)
        return;
    mpPlayer->setBufferValue(Config::instance().bufferValue());
}

void MainWindow::onAbortOnTimeoutChanged()
{
    if (!mpPlayer)
        return;
    mpPlayer->setInterruptOnTimeout(Config::instance().abortOnTimeout());
}

void MainWindow::onUserShaderChanged()
{
    if (!mpRenderer || !mpRenderer->opengl())
        return;
#ifndef QT_NO_OPENGL
    if (Config::instance().userShaderEnabled()) {
        if (Config::instance().intermediateFBO()) {
            if (!m_glsl)
                m_glsl = new GLSLFilter(this);
            m_glsl->installTo(mpRenderer);
        } else {
            if (m_glsl)
                m_glsl->uninstall();
        }
        if (!m_shader)
            m_shader = new DynamicShaderObject(this);
        m_shader->setHeader(Config::instance().fragHeader());
        m_shader->setSample(Config::instance().fragSample());
        m_shader->setPostProcess(Config::instance().fragPostProcess());
        mpRenderer->opengl()->setUserShader(m_shader);
    } else {
        mpRenderer->opengl()->setUserShader(Q_NULLPTR);
    }
#endif
}

void MainWindow::setup()
{
    ConfigDialog::display();
}

void MainWindow::handleFullscreenChange()
{
    workaroundRendererSize();

    // workaround renderer display size for ubuntu
    tryShowControlBar();
    QTimer::singleShot(3000, this, SLOT(tryHideControlBar()));
}

void MainWindow::toggoleSubtitleEnabled(bool value)
{
    mpSubtitle->setEnabled(value);
}

void MainWindow::toggleSubtitleAutoLoad(bool value)
{
    mpSubtitle->setAutoLoad(value);
}

void MainWindow::openSubtitle()
{
    QString file = QFileDialog::getOpenFileName(Q_NULLPTR, tr("Open a subtitle file"));
    if (file.isEmpty())
        return;
    mpSubtitle->setFile(file);
}

void MainWindow::setSubtitleCharset(const QString &charSet)
{
    Q_UNUSED(charSet);
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    if (!box)
        return;
    mpSubtitle->setCodec(box->itemData(box->currentIndex()).toByteArray());
}

void MainWindow::setSubtitleEngine(const QString &value)
{
    Q_UNUSED(value)
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    if (!box)
        return;
    mpSubtitle->setEngines(QStringList() << box->itemData(box->currentIndex()).toString());
}

void MainWindow::changeClockType(QAction *action)
{
    action->setChecked(true);
    int value = action->data().toInt();
    if (value < 0) {
        mpPlayer->masterClock()->setClockAuto(true);
        // TODO: guess clock type
        return;
    }
    mpPlayer->masterClock()->setClockAuto(false);
    mpPlayer->masterClock()->setClockType(AVClock::ClockType(value));
}

void MainWindow::setClockType()
{
    if (isFileImgageSequence() && Config::instance().forceVideoClockI()){
        qDebug()<<"setClockType for ImgageSequence force AVClock::VideoClock";
        mpPlayer->masterClock()->setClockAuto(false);
        mpPlayer->masterClock()->setClockType(AVClock::VideoClock);
    }else{
        qDebug()<<"setClockType for Video from Menu";
        if (mpClockMenuAction) {
            QAction *a = mpClockMenuAction->checkedAction();
            if (a){
                int value = mpClockMenuAction->checkedAction()->data().toInt();
                qDebug()<<"setClockType Video object action from menu action "<<value;
                if (value < 0) {
                    mpPlayer->masterClock()->setClockAuto(true);
                    // TODO: guess clock type
                    return;
                }else{
                    mpPlayer->masterClock()->setClockAuto(false);
                    mpPlayer->masterClock()->setClockType(AVClock::ClockType(value));
                }
            }
        }
    }
}

void MainWindow::syncVolumeUi(qreal value)
{
    const int v(value/kVolumeInterval);
    if (mpVolumeSlider->value() == v)
        return;
    mpVolumeSlider->setValue(v);
}

void MainWindow::workaroundRendererSize()
{
    if (!mpRenderer)
        return;

    QSize s = rect().size();
    //resize(QSize(s.width()-1, s.height()-1));
    //resize(s); //window resize to fullscreen size will create another fullScreenChange event
    mpRenderer->widget()->resize(QSize(s.width()+1, s.height()+1));
    mpRenderer->widget()->resize(s);
}

void MainWindow::loadRemoteUrlPresset(const QString& url){
    QString lurl=url;
    if (!lurl.contains("://") || lurl.startsWith("file://")) {
        lurl=QFileInfo(lurl).absolutePath()+"/"+QFileInfo(lurl).baseName()+".config";
    }
    qDebug("MainWindow::loadRemoteUrlPresset url: %s, lurl: %s",qPrintable(url),qPrintable(lurl));
    if (same_site_domain(QUrl(url),QUrl(XUNOserverUrl))){
        QString surl=XUNOpresetUrl;
        QByteArray ba;
        ba.append("m="+lurl.remove(XUNOserverUrl+"/",Qt::CaseInsensitive));
        surl.append("q="+ba.toBase64());
        qDebug("MainWindow::openUrl surl: %s",qPrintable(surl));
        mpVideoEQ->setRemoteUrlPresset(surl);
        mpVideoEQ->getRemotePressets();
    }else if (lurl.endsWith(".config")){
        mpVideoEQ->setRemoteUrlPresset(lurl);
        mpVideoEQ->getLocalPressets();
    }
}

void MainWindow::reSizeByMovie()
{
    if (isFullScreen()) {
        qDebug()<<"skipped, MainWindow::reSizeByMovie(). isFullScreen";
        return;
    }
    QSize t=mpRenderer->rendererSize();
    Statistics st=mpPlayer->statistics();

    if (st.video_only.width>0 && st.video_only.height>0 && mPlayerScale>0 ){ //(t.width()+t.height())==0
        t.setWidth(st.video_only.width*mPlayerScale);
        t.setHeight(st.video_only.height*mPlayerScale);
    }else{
        qDebug()<<"skipped, MainWindow::reSizeByMovie(). st.video_only"<<st.video_only.width<<st.video_only.height<<mPlayerScale;
    }
    if (t.isValid() && (!t.isNull())) {
        resize(t);
        if (mpGLSLFilter) {
            mpGLSLFilter->setOutputSize(t);
        }
        //installGLSLFilter(t);
    }else{
       qDebug()<<"skipped, MainWindow::reSizeByMovie(). t.is Not Valid()"<<t;
    }
    //qDebug()<<"reSizeByMovie before calcToUseSuperResolution";
    //calcToUseSuperResolution();
}

void MainWindow::onClickXunoBrowser(QUrl url){
    if (url.isValid()) play(url.toString());
}

void MainWindow::onFullScreen(){
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void MainWindow::setPlayerPosFromRepeat(){
    tuneRepeatMovieDuration();
    if (mpRepeatEnableAction->isChecked()){
        qint64 RA=QTime(0, 0, 0).msecsTo(mpRepeatA->time());
        qint64 RB=QTime(0, 0, 0).msecsTo(mpRepeatB->time());
        if (RB>RA){
            if (RA>=mpPlayer->mediaStartPosition() && RA<=mpPlayer->mediaStopPosition()) {
                mpPlayer->setStartPosition(RA);
            }
            if (RB>=mpPlayer->mediaStartPosition() && RB<=mpPlayer->mediaStopPosition()) {
                mpPlayer->setStopPosition(RB);
            }
        }
    }
}

void MainWindow::customfpsChanged(double n){
    mCustomFPS=n;
    if (mpImgSeqExtract) mpImgSeqExtract->setFPS(n);
}
void MainWindow::tuneRepeatMovieDuration(){
    mpImageSequence->setMovieDuration(mpPlayer->mediaStopPosition());
}

bool MainWindow::isFileImgageSequence(){
    return (QDir::toNativeSeparators(mFile).contains("*"));// && mFile.contains("d.")
}

bool MainWindow::applyCustomFPS(){
    bool ret=false; // return true if custom fps was applied
    if (isFileImgageSequence() && (mCustomFPS>0)){
        QVariantHash tmp;
        if (Config::instance().avformatOptionsEnabledI()) tmp=Config::instance().avformatOptionsI();
        tmp["framerate"]=mCustomFPS;
        mpPlayer->setOptionsForFormat(tmp);
        ret=true;
    }else{
        customfpsChanged(0.);
    }
    return ret;
}

void MainWindow::RepeatLoopChanged(int i){
    bool checked=(i==Qt::Checked);
    mpRepeatLoop->setChecked(i);
    qDebug()<<"RepeatLoopChanged:"<<i;
    mpRepeatBox->setValue(checked?-1:1);
    mpRepeatBox->setVisible(!checked);
    QLayout *layout = mpRepeatBox->parentWidget()->layout();
    QHBoxLayout *layout2 = layout->findChild<QHBoxLayout *>("TimesLayout");
    if (layout2){
        for (int j = 0; j < layout2->count(); ++j){
            QWidget *widget =layout2->itemAt(j)->widget();
            if (widget){
                widget->setVisible(!checked);
            }
        }
    }
    bool ch=mpRepeatEnableAction->isChecked();
    mpRepeatEnableAction->setChecked(!ch);
    mpRepeatEnableAction->setChecked(ch);
}

void MainWindow::onPreviewEnabledChanged(){
    //qDebug()<<"onPreviewEnabledChanged"<<Config::instance().previewEnabled();
    if (m_preview && !Config::instance().previewEnabled()) {
        m_preview->close();
        delete m_preview;
        m_preview = Q_NULLPTR;
        return;
    }
}

void MainWindow::onImageSequenceConfig()
{
    int ret=0;
    bool state=!mpPlayer->isPaused() && mpPlayer->isPlaying();
    if (mpImageSequence) {
        if (state) stopUnload();//togglePlayPause();
        mpImageSequence->setWindowFlags(Qt::WindowStaysOnTopHint);
        if (isFileImgageSequence()) mpImageSequence->setImageSequenceFileName(mFile);
        ret = mpImageSequence->exec();
        if (ret==QDialog::Rejected && state) togglePlayPause();
    }
    qDebug()<<"onImageSequenceConfig after show"<<ret;
}

void MainWindow::onImageSequenceToogledFrameExtractor(bool state)
{
    qDebug()<<"onImageSequenceToogledFrameExtractor "<<state;
    mpImgSeqExtract->setVisible(state);
    mpCurrent->setVisible(!state);
    mpEnd->setVisible(!state);
    if (detachedControl){
        QSize s=QSize(detachedControl->width(),state?detachedControl->maximumHeight():detachedControl->minimumHeight());
        detachedControl->resize(s);
    }

}

void MainWindow::analyeUsedFPS()
{
    Statistics st=mpPlayer->statistics();
    qDebug()<<"analyeUsedFPS"<<st.video.frame_rate<<mCustomFPS;
    if (mCustomFPS==0.)
        if (mpImgSeqExtract){
            //Statistics st=mpPlayer->statistics();
            if (st.video.frame_rate>0.){
                qDebug()<<"setFPS analyeUsedFPS video.frame_rate"<<st.video.frame_rate;
                mpImgSeqExtract->setFPS(st.video.frame_rate);
            }
        }
}

void MainWindow::installShaderXuno()
{
    if (mpRenderer && mpRenderer->opengl()){
        if (shaderXuno==Q_NULLPTR) shaderXuno=new ShaderFilterXuno();
        if (shaderXuno!=Q_NULLPTR) {
            shaderXuno->setGammaValue(0.f);
            shaderXuno->setSharpValue(-1.f); //-1..0..1
            //mpRenderer->opengl()->setUserShader(shaderXuno);
        }
    }
}

void MainWindow::installSaveGL()
{
    if (mSaveGLXuno==Q_NULLPTR && mpPlayer){
        mSaveGLXuno=new SaveGLXuno(this);
        mSaveGLXuno->setPlayer(mpPlayer);
    }else if (mpRenderer && mpRenderer->opengl()){
        if (mSaveGLXuno==Q_NULLPTR) mSaveGLXuno=new SaveGLXuno(this);
    }
}

void MainWindow::installGLSLFilter()
{
    qDebug()<<"installGLSLFilter";
    if (mpGLSLFilter == Q_NULLPTR && mpRenderer && mpRenderer->opengl() ){
        mpGLSLFilter = new XunoGLSLFilter(this);
        mpGLSLFilter->setEnabled(true);
        mpGLSLFilter->setPlayer(mpPlayer);
        if (shaderXuno) mpGLSLFilter->setShader(shaderXuno);
        bool state=mpRenderer->installFilter(mpGLSLFilter);
        qDebug()<<"installXunoGLSLFilter state"<<state;
    }
}

void MainWindow::installSimpleFilter()
{
    qDebug()<<"installSimpleFilter";
    if (mpRenderer && mpRenderer->opengl() ){

        XunoSimpleFilter *filter = new XunoSimpleFilter(mpRenderer->widget());
        filter->setText(QString::fromLatin1("Filter on Renderer"));
        VideoFilterContext *ctx = static_cast<VideoFilterContext*>(filter->context());
        ctx->rect = QRect(200, 150, 400, 60);
        ctx->opacity = 0.7;
        filter->enableRotate(false);
        filter->prepare();
        mpRenderer->installFilter(filter);

        filter = new XunoSimpleFilter(mpRenderer->widget());
        filter->setText(QString());
        filter->setImage(QImage(QString::fromLatin1(":/images/qt-logo.png")));
        ctx = static_cast<VideoFilterContext*>(filter->context());
        ctx->rect = QRect(400, 80, 200, 200);
        ctx->opacity = 0.618;
        filter->enableRotate(true);
        filter->enableWaveEffect(false);
        filter->prepare();
        mpRenderer->installFilter(filter);

        filter = new XunoSimpleFilter(mpPlayer);
        filter->setText(QString::fromLatin1("<h1 style='color:#ffff00'>HTML Filter on<span style='color:#ff0000'>Video Frame</span></h2>"));
        filter->enableWaveEffect(false);
        filter->enableRotate(true);
        ctx = static_cast<VideoFilterContext*>(filter->context());
        ctx->rect = QRect(200, 100, 400, 60);
        filter->prepare();
        bool state= mpPlayer->installFilter(filter);
        qDebug()<<"installSimpleFilter state"<<state;
    }
}

void MainWindow::scaleReset()
{
    qreal scale=1.0;
    qreal nextscale15=1.5;
    qreal nextscale20=2.0;
    setPlayerScale(scale);
    mpScaleX15Btn->setText(QString("x%1").arg(nextscale15));
    mpScaleX15Btn->setToolTip(QString("Scale X%1").arg(nextscale15));
    mpScaleX2Btn->setText(QString("x%1").arg(nextscale20));
    mpScaleX2Btn->setToolTip(QString("Scale X%1").arg(nextscale20));
    if (mpRenderer) {
        QSize renderFrame=mpRenderer->videoRect().size();
        QRectF roi2=QRectF(QPointF(0,0), renderFrame);
        mpRenderer->setRegionOfInterest(roi2);
    }
}


void MainWindow::onScaleBtn(qreal _scale)
{
    //qDebug()<<"MainWindow: onScaleBtn"<<_scale;
    qreal scale = 0.0,nextscale15 = 0.0,nextscale20 = 0.0;
    //mPlayerScale

    if (_scale==1.0) {
        nextscale15=1.5;
        nextscale20=2.0;
        scale=1.0;
    }else if (_scale==1.5) {
        scale=mpScaleX15Btn->text().split('x')[1].toFloat();
        nextscale15=(scale==1.)?1.5:1.0;
        nextscale20=2.0;
    }else if (_scale==2.0){
        scale=mpScaleX2Btn->text().split('x')[1].toFloat();
        nextscale15=1.5;
        nextscale20=(scale==1.)?2.0:1.0;
    }
    qDebug()<<"MainWindow: onScaleBtn scale set:"<<scale;

    setPlayerScale(scale);

    mpScaleX15Btn->setText(QString("x%1").arg(nextscale15));
    mpScaleX15Btn->setToolTip(QString("Scale X%1").arg(nextscale15));
    mpScaleX2Btn->setText(QString("x%1").arg(nextscale20));
    mpScaleX2Btn->setToolTip(QString("Scale X%1").arg(nextscale20));
    reSizeByMovie();

    if (scale==2.0){
        qDebug()<<"mpFullScreenBtn Show"<<scale;
        mpFullScreenBtn->show();
    }else{
        qDebug()<<"mpFullScreenBtn Hide"<<scale;
        mpFullScreenBtn->hide();
    }

}

void MainWindow::onScaleX2Btn()
{
    onScaleBtn(2.0);
}

void MainWindow::onScaleX15Btn()
{
    onScaleBtn(1.5);
}

void MainWindow::onScaleX1Btn()
{
    //onScaleBtn(1.0);
    //setPlayerScale(1.0);
    if (mpRenderer){
        QSize videoFrame=mpRenderer->videoFrameSize();
        //QSize renderFrame=mpRenderer->rendererSize();
        QSize renderFrame=mpRenderer->videoRect().size();
        if (mPlayerScale>0.) videoFrame*=mPlayerScale;
        //QRectF viewport = QRectF(mpRenderer->mapToFrame(QPointF(0, 0)), mpRenderer->mapToFrame(QPointF(mpRenderer->rendererWidth(), mpRenderer->rendererHeight())));
        qDebug()<<"onScaleX1Btn roi"<<mpRenderer->realROI();
        qDebug()<<"onScaleX1Btn videoFrame"<<videoFrame<<"renderFrame"<<renderFrame<<"maptoFrame"<<QRectF(mpRenderer->mapToFrame(QPointF(0,0)), renderFrame);
        QRectF roi2=QRectF(QPointF(0,0), renderFrame);
        roi2.moveCenter(QPointF(videoFrame.width()/2,videoFrame.height()/2));
        mpRenderer->setRegionOfInterest(roi2);
        mNaitiveScaleOn=true;
    }
}

void MainWindow::captureGL()
{
    //    if (mSaveGLXuno!=Q_NULLPTR) {
    //        mSaveGLXuno->saveimg();
    //    }
    if (mpGLSLFilter!=Q_NULLPTR){
        mpGLSLFilter->setNeedSave(true);
    }
}

QString MainWindow::XUNO_QtAV_Version_String(bool longstring)
{
    if (longstring) return XUNO_QTAV_VERSION_STR;
    else return QString(XUNO_QTAV_VERSION_STR).split(' ').at(0);

}

QString  MainWindow::XUNO_QtAV_Version_String_Long()
{
    return XUNO_QTAV_VERSION_STR_LONG;
}
