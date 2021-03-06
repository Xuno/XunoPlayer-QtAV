#include "ImageSequenceExtractControl.h"


ImgSeqExtractControl::ImgSeqExtractControl(QWidget *parent) :
    QWidget (parent),
    isFPS(0.),
    isRegionPlaying(false)
{

    //customStyles();

    baseParentMaxHeight=parentWidget()->maximumHeight();

    verticalLayoutWidget=this;
    verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
    verticalLayout = new QVBoxLayout(verticalLayoutWidget);
    verticalLayoutWidget->setLayout(verticalLayout);
    verticalLayout->setSpacing(2);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setContentsMargins(4, 0, 4, 0);

    hlStart_6 = new QHBoxLayout();
    hlStart_6->setSpacing(2);
    hlStart_6->setObjectName(QStringLiteral("hlStart_6"));
    hlStart_6->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Minimum));
    startTime = new QLabel(verticalLayoutWidget);
    startTime->setObjectName(QStringLiteral("startTime"));
    hlStart_6->addWidget(startTime);

    hs_start = new QSpacerItem(12, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

    hlStart_6->addItem(hs_start);

    labelSF = new QLabel(verticalLayoutWidget);
    labelSF->setObjectName(QStringLiteral("labelSF"));

    hlStart_6->addWidget(labelSF);

    ImageSequenceStartFrame = new QSpinBox(verticalLayoutWidget);
    ImageSequenceStartFrame->setObjectName(QStringLiteral("ImageSequenceStartFrame"));
    ImageSequenceStartFrame->setEnabled(true);
    ImageSequenceStartFrame->setMinimum(1);
    ImageSequenceStartFrame->setMaximum(999999999);

    hlStart_6->addWidget(ImageSequenceStartFrame);

    buttonSetStartSeekFrame = new QToolButton(verticalLayoutWidget);
    buttonSetStartSeekFrame->setObjectName(QStringLiteral("buttonSetStartSeekFrame"));

    hlStart_6->addWidget(buttonSetStartSeekFrame);

    buttonSetStartFrame = new QToolButton(verticalLayoutWidget);
    buttonSetStartFrame->setObjectName(QStringLiteral("buttonSetStartFrame"));

    hlStart_6->addWidget(buttonSetStartFrame);

    hs_total = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlStart_6->addItem(hs_total);

    labelTF = new QLabel(verticalLayoutWidget);
    labelTF->setObjectName(QStringLiteral("labelTF"));

    hlStart_6->addWidget(labelTF);

    ImageSequenceTotalFrame = new QSpinBox(verticalLayoutWidget);
    ImageSequenceTotalFrame->setObjectName(QStringLiteral("ImageSequenceTotalFrame"));
    ImageSequenceTotalFrame->setEnabled(false);
    ImageSequenceTotalFrame->setMinimum(1);
    ImageSequenceTotalFrame->setMaximum(999999999);

    hlStart_6->addWidget(ImageSequenceTotalFrame);

//    mPlayPixmap = QPixmap(":/theme/button-play-pause.png");
//    int w = mPlayPixmap.width(), h = mPlayPixmap.height();
//    mPausePixmap = mPlayPixmap.copy(QRect(w/2, 0, w/2, h));
//    mPlayPixmap = mPlayPixmap.copy(QRect(0, 0, w/2, h));
//    qDebug("mPlayPixmap %d x %d", mPlayPixmap.width(), mPlayPixmap.height());
    mpPlayPauseBtn = new QToolButton(verticalLayoutWidget);
    mpPlayPauseBtn->setObjectName(QStringLiteral("mpPlayPauseBtn"));
//    int a = qMin(w/2, h);
//    const int kMaxButtonIconWidth = 20;
//    const int kMaxButtonIconMargin = kMaxButtonIconWidth/3;
//    a = qMin(a, kMaxButtonIconWidth);
    mpPlayPauseBtn->setIcon(QIcon(":/theme/dark/play.svg"));
    //mpPlayPauseBtn->setIconSize(QSize(a, a/2));
//    mpPlayPauseBtn->setMaximumSize(a+kMaxButtonIconMargin+2, a+kMaxButtonIconMargin);
    //connect(mpPlayPauseBtn, SIGNAL(clicked()), SLOT(on_mpPlayPauseBtn_clicked()));

    hlStart_6->addWidget(mpPlayPauseBtn);

    hs_end = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hlStart_6->addItem(hs_end);

    labelEF = new QLabel(verticalLayoutWidget);
    labelEF->setObjectName(QStringLiteral("labelEF"));

    hlStart_6->addWidget(labelEF);

    ImageSequenceEndFrame = new QSpinBox(verticalLayoutWidget);
    ImageSequenceEndFrame->setObjectName(QStringLiteral("ImageSequenceEndFrame"));
    ImageSequenceEndFrame->setEnabled(false);
    ImageSequenceEndFrame->setMinimum(1);
    ImageSequenceEndFrame->setMaximum(999999999);

    hlStart_6->addWidget(ImageSequenceEndFrame);

    buttonSetEndSeekFrame = new QToolButton(verticalLayoutWidget);
    buttonSetEndSeekFrame->setObjectName(QStringLiteral("buttonSetEndSeekFrame"));

    hlStart_6->addWidget(buttonSetEndSeekFrame);

    buttonSetEndFrame = new QToolButton(verticalLayoutWidget);
    buttonSetEndFrame->setObjectName(QStringLiteral("buttonSetEndFrame"));

    hlStart_6->addWidget(buttonSetEndFrame);

    horizontalSpacer_10 = new QSpacerItem(12, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

    hlStart_6->addItem(horizontalSpacer_10);

    endTime = new QLabel(verticalLayoutWidget);
    endTime->setObjectName(QStringLiteral("endTime"));

    hlStart_6->addWidget(endTime);

    hlStart_6->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Minimum));

    verticalLayout->addLayout(hlStart_6);

    horizontalLayout_6 = new QHBoxLayout();
    horizontalLayout_6->setSpacing(2);
    horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
    btSelectOutputPath = new QPushButton(verticalLayoutWidget);
    btSelectOutputPath->setObjectName(QStringLiteral("btSelectOutputPath"));

    horizontalLayout_6->addWidget(btSelectOutputPath);

    OutputPath = new QLineEdit(verticalLayoutWidget);
    OutputPath->setObjectName(QStringLiteral("OutputPath"));

    horizontalLayout_6->addWidget(OutputPath);

    cb_OutputType = new QComboBox(verticalLayoutWidget);
    cb_OutputType->setObjectName(QStringLiteral("cb_OutputType"));
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(cb_OutputType->sizePolicy().hasHeightForWidth());
    cb_OutputType->setSizePolicy(sizePolicy);
    cb_OutputType->setMinimumSize(QSize(45, 0));

    horizontalLayout_6->addWidget(cb_OutputType);

    cbColorTypeOutput = new QComboBox(verticalLayoutWidget);
    cbColorTypeOutput->setObjectName(QStringLiteral("cbColorTypeOutput"));
    cbColorTypeOutput->setMinimumSize(QSize(55, 0));

    horizontalLayout_6->addWidget(cbColorTypeOutput);
    horizontalLayout_6->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Minimum));

    verticalLayout->addLayout(horizontalLayout_6);

    hlPrefix_3 = new QHBoxLayout();
    hlPrefix_3->setSpacing(2);
    hlPrefix_3->setObjectName(QStringLiteral("hlPrefix_3"));
    hlPrefix_3->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Minimum));
    labelFilePrefix = new QLabel(verticalLayoutWidget);
    labelFilePrefix->setObjectName(QStringLiteral("labelFilePrefix"));
    hlPrefix_3->addWidget(labelFilePrefix);

    OutputFileNumberingPrefix = new QLineEdit(verticalLayoutWidget);
    OutputFileNumberingPrefix->setObjectName(QStringLiteral("OutputFileNumberingPrefix"));
    OutputFileNumberingPrefix->setMaxLength(80);

    hlPrefix_3->addWidget(OutputFileNumberingPrefix);

    hlPrefix_3->addItem(new QSpacerItem(8, 1, QSizePolicy::Minimum, QSizePolicy::Minimum));

    labelFileSeparator = new QLabel(verticalLayoutWidget);
    labelFileSeparator->setObjectName(QStringLiteral("labelFileSeparator"));

    hlPrefix_3->addWidget(labelFileSeparator);

    OutputFileNumberingSeperator = new QLineEdit(verticalLayoutWidget);
    OutputFileNumberingSeperator->setObjectName(QStringLiteral("OutputFileNumberingSeperator"));
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(OutputFileNumberingSeperator->sizePolicy().hasHeightForWidth());
    OutputFileNumberingSeperator->setSizePolicy(sizePolicy1);
    OutputFileNumberingSeperator->setMinimumSize(QSize(0, 0));
    OutputFileNumberingSeperator->setMaximumSize(QSize(15, 16777215));
    OutputFileNumberingSeperator->setMaxLength(1);

    hlPrefix_3->addWidget(OutputFileNumberingSeperator, 0, Qt::AlignRight);

    hlPrefix_3->addItem(new QSpacerItem(6, 1, QSizePolicy::Minimum, QSizePolicy::Minimum));

    cbOutputFileSizeUse = new QCheckBox(verticalLayoutWidget);
    cbOutputFileSizeUse->setObjectName(QStringLiteral("cbOutputFileSizeUse"));
    cbOutputFileSizeUse->setChecked(false);

    hlPrefix_3->addWidget(cbOutputFileSizeUse);

    cbOutputFileDepthUse = new QCheckBox(verticalLayoutWidget);
    cbOutputFileDepthUse->setObjectName(QStringLiteral("cbOutputFileDepthUse"));
    cbOutputFileDepthUse->setChecked(false);

    hlPrefix_3->addWidget(cbOutputFileDepthUse);


    hlPrefix_3->addItem(new QSpacerItem(4, 1, QSizePolicy::Minimum, QSizePolicy::Minimum));


    buttonExtractFrames = new QPushButton(verticalLayoutWidget);
    buttonExtractFrames->setObjectName(QStringLiteral("buttonExtractFrames"));

    hlPrefix_3->addWidget(buttonExtractFrames);

    verticalLayout->addLayout(hlPrefix_3);

    retranslateUi();
    adjustSize();

    QMetaObject::connectSlotsByName(verticalLayoutWidget);
}
void ImgSeqExtractControl::retranslateUi()
{
    const QString buttTextPadding="padding: 3px 7px 4px 7px;";
    startTime->setText(QApplication::translate("ImageSequenceExtract", "00:00:00.000", 0));
    labelSF->setText(QApplication::translate("ImageSequenceExtract", " Start Frames:", 0));
    //buttonSetStartFrame->setStyleSheet(buttTextPadding);
    buttonSetStartFrame->setText(QApplication::translate("ImageSequenceExtract", "Set", 0));
    //buttonSetStartSeekFrame->setStyleSheet(buttTextPadding);
    buttonSetStartSeekFrame->setText(QApplication::translate("ImageSequenceExtract", "Seek", 0));
    labelTF->setText(QApplication::translate("ImageSequenceExtract", " Total Frames:", 0));
    labelEF->setText(QApplication::translate("ImageSequenceExtract", " End Frame:", 0));
    //buttonSetEndFrame->setStyleSheet(buttTextPadding);
    buttonSetEndFrame->setText(QApplication::translate("ImageSequenceExtract", "Set", 0));
    //buttonSetEndSeekFrame->setStyleSheet(buttTextPadding);
    buttonSetEndSeekFrame->setText(QApplication::translate("ImageSequenceExtract", "Seek", 0));
    endTime->setText(QApplication::translate("ImageSequenceExtract", "00:00:00.000", 0));
    btSelectOutputPath->setStyleSheet(buttTextPadding);
    btSelectOutputPath->setText(QApplication::translate("ImageSequenceExtract", "Select Output Path", 0));
    setupOutputType();
    setupColorTypeOutput();
    QObject::connect(cb_OutputType,SIGNAL(currentIndexChanged(int)),SLOT(setupColorTypeOutput(int)));
    labelFilePrefix->setText(QApplication::translate("ImageSequenceExtract", "Prefix:", 0));
    OutputFileNumberingPrefix->setText("img");
    labelFileSeparator->setText(QApplication::translate("ImageSequenceExtract", "Separator:", 0));
    OutputFileNumberingSeperator->setText("-");
    buttonExtractFrames->setStyleSheet(buttTextPadding);
    buttonExtractFrames->setText(QApplication::translate("ImageSequenceExtract", "Extract Frames", 0));

    cbOutputFileSizeUse->setText(QApplication::translate("ImageSequenceExtract", "Size", 0));
    cbOutputFileDepthUse->setText(QApplication::translate("ImageSequenceExtract", "Depth", 0));

#ifndef QT_NO_TOOLTIP
    cbOutputFileSizeUse->setToolTip(QApplication::translate("ImageSequenceExtract", "Use information about image dimension in file name", 0));
    cbOutputFileDepthUse->setToolTip(QApplication::translate("ImageSequenceExtract", "Use information about color depth in file name", 0));
#endif // QT_NO_TOOLTIP



    mpPlayPauseBtn->setToolTip(QApplication::translate("ImageSequenceExtract", "play selected region form start to end frames", 0));
} // retranslateUi



ImgSeqExtractControl::~ImgSeqExtractControl()
{

}

void ImgSeqExtractControl::RecalculateParentHeight(bool visible)
{
    if (parentWidget()){
        parentWidget()->setMaximumHeight(baseParentMaxHeight+(visible?height():0));
    }
}

void ImgSeqExtractControl::setVisible(bool visible)
{
    RecalculateParentHeight(visible);
    QWidget::setVisible(visible);
    if (!visible) emit setTimeSliderVisualMaxLimit(QTime(0,0,0));
    //qDebug()<<"ImgSeqExtractControl::setVisible"<<visible;
}

void ImgSeqExtractControl::setStartTime(QTime time)
{
    qDebug()<<"ImgSeqExtractControl setStartTime"<<time.toString("hh:mm:ss.zzz");
    if (isFPS==0. ||isStartTime==time) return;
    isStartTime=time;
    startTime->setText(time.toString("hh:mm:ss.zzz"));
    startTime->setToolTip(tr("Frame:%1").arg(timeToFrame(time)));
}

void ImgSeqExtractControl::setEndTime(QTime time)
{
    qDebug()<<"ImgSeqExtractControl setEndTime"<<time.toString("hh:mm:ss.zzz");
    if (time==QTime(0,0,0)) onPaused();
    if (isFPS==0. || isEndTime==time) return;
    isEndTime=time;
    int frame=timeToFrame(time);
    endTime->setText(time.toString("hh:mm:ss.zzz"));
    endTime->setToolTip(tr("Frame:%1").arg(frame));
    ImageSequenceStartFrame->setMaximum(frame);
    ImageSequenceEndFrame->setMaximum(frame);
    ImageSequenceTotalFrame->setMaximum(frame);
    calcEndFrame(time);
}

void ImgSeqExtractControl::setTotalFrames(int frames)
{
    if (frames>0 && (ImageSequenceTotalFrame->value()!=frames)){
        if (!ImageSequenceTotalFrame->isEnabled()) ImageSequenceTotalFrame->setEnabled(true);
        ImageSequenceTotalFrame->setValue(frames);
    }
}

void ImgSeqExtractControl::setStartFrame(int frame)
{
    if (frame>0 && (ImageSequenceStartFrame->value()!=frame)){
        if (!ImageSequenceStartFrame->isEnabled()) ImageSequenceStartFrame->setEnabled(true);
        ImageSequenceStartFrame->setValue(frame);
    }
}

void ImgSeqExtractControl::setEndFrame(int frame)
{
    if (frame>0 && (ImageSequenceEndFrame->value()!=frame)){
        if (!ImageSequenceEndFrame->isEnabled()) ImageSequenceEndFrame->setEnabled(true);
        ImageSequenceEndFrame->setValue(frame);
    }
}

int ImgSeqExtractControl::getTotalFrames() const
{
    return ImageSequenceTotalFrame->value();
}

int ImgSeqExtractControl::getStartFrame() const
{
    return ImageSequenceStartFrame->value();
}

int ImgSeqExtractControl::getEndFrame() const
{
    return ImageSequenceEndFrame->value();
}

void ImgSeqExtractControl::calcStartFrame()
{
    setStartFrame(getTotalFrames()-getEndFrame()+1);
}


void ImgSeqExtractControl::calcStartFrame(QTime t)
{
    setStartFrame(timeToFrame(t));
    calcTotalFrames();
}

int ImgSeqExtractControl::timeToFrame(QTime t)
{
    return 1-t.msecsTo(QTime(0,0,0))/1000.*isFPS;
}

QTime ImgSeqExtractControl::frameToTime(int f)
{
    return QTime(0,0,0).addMSecs((float(f)/isFPS)*1000);
}

void ImgSeqExtractControl::calcEndFrame()
{
    //qDebug()<<"calcEndFrame"<<getTotalFrames()+getStartFrame()-1;
    setEndFrame(getTotalFrames()+getStartFrame()-1);
}

void ImgSeqExtractControl::calcEndFrame(QTime t)
{
    setEndFrame(timeToFrame(t));
    calcTotalFrames();
}

void ImgSeqExtractControl::calcTotalFrames()
{
    setTotalFrames(getEndFrame()-getStartFrame()+1);
}


void ImgSeqExtractControl::setFPS(float fps)
{
    isFPS=fps;
    qDebug()<<"ImgSeqExtractControl::setFPS"<<isFPS;
}

qint64 ImgSeqExtractControl::StartPosExtract()
{
    return  isStartPosExtract;
}

qint64 ImgSeqExtractControl::EndPosExtract()
{
    return  isEndPosExtract;
}

bool ImgSeqExtractControl::regionPlaying()
{
    return  isRegionPlaying;
}




// ---------------  SLOTS  ---------------------

void ImgSeqExtractControl::onPaused(){
    isRegionPlaying=false;
    mpPlayPauseBtn->setDisabled(false);
}

void ImgSeqExtractControl::on_buttonSetStartFrame_clicked(bool state)
{
    Q_UNUSED(state);
    qDebug()<<"on_buttonSetStartFrame_clicked"<<state;
    calcStartFrame(isStartTime);
}

void ImgSeqExtractControl::on_buttonSetEndFrame_clicked(bool state)
{
    Q_UNUSED(state);
    calcEndFrame(isStartTime);
}

void ImgSeqExtractControl::on_ImageSequenceStartFrame_valueChanged(int i)
{
    QSpinBox *sb = qobject_cast<QSpinBox *>(sender());
    QTime t=frameToTime(i);
    isStartPosExtract = QTime(0,0,0).msecsTo(t);
    sb->setToolTip(t.toString(timeFormat));
    calcTotalFrames();
    if (this->isVisible())
        emit setTimeSliderVisualMinLimit(t);
}

void ImgSeqExtractControl::on_ImageSequenceEndFrame_valueChanged(int i)
{
    QSpinBox *sb = qobject_cast<QSpinBox *>(sender());
    QTime t=frameToTime(i);
    isEndPosExtract = QTime(0,0,0).msecsTo(t);
    sb->setToolTip(t.toString(timeFormat));
    calcTotalFrames();
    if (this->isVisible())
        emit setTimeSliderVisualMaxLimit(t);
}


void ImgSeqExtractControl::on_ImageSequenceTotalFrame_valueChanged(int i)
{
    QSpinBox *sb = qobject_cast<QSpinBox *>(sender());
    sb->setToolTip(frameToTime(i).toString("hh:mm:ss.zzz"));
    calcEndFrame();
}

void ImgSeqExtractControl::on_mpPlayPauseBtn_clicked()
{
    //qDebug()<<"on_mpPlayPauseBtn_clicked";
    if (ImageSequenceStartFrame->isEnabled() && ImageSequenceEndFrame->isEnabled()){
        QTime start=frameToTime(ImageSequenceStartFrame->value());
        emit pause();
        emit seek(start);
        isRegionPlaying=true;
        emit togglePlayPause();
    }
}

void ImgSeqExtractControl::on_btSelectOutputPath_clicked()
{
    qDebug()<<"on_btSelectOutputPath_clicked";
    //get latest used path
    QString prevPath="E:\\public\\Videos\\OWN\\work\\image-seq\\testout";//= config.getLastUsedPath();
    QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("Directory path for exctract"), prevPath, QFileDialog::ShowDirsOnly));
    if (!directory.isEmpty()){
        qDebug()<<"on_btSelectOutputPath_clicked"<<directory;
        OutputPath->setText(directory);
    }
}

void ImgSeqExtractControl::setOutputPath(QString name){
    OutputPath->setText(name);
}

void ImgSeqExtractControl::on_buttonSetStartSeekFrame_clicked()
{
    QTime t=frameToTime(ImageSequenceStartFrame->value());
    emit seek(t);
}

void ImgSeqExtractControl::on_buttonSetEndSeekFrame_clicked()
{
    QTime t=frameToTime(ImageSequenceEndFrame->value());
    emit seek(t);
}

void ImgSeqExtractControl::on_buttonExtractFrames_clicked()
{
    //qDebug()<<"on_buttonExtractFrames_clicked";
    QString file_path=OutputPath->text();
    if (file_path.isEmpty() || movieName.isEmpty()) return;
    if (ImageSequenceStartFrame->isEnabled() && ImageSequenceEndFrame->isEnabled()){
        emit pause();
        QString file_prefix=OutputFileNumberingPrefix->text();
        //if (file_prefix.isEmpty()) file_prefix="img";
        QString file_separator=OutputFileNumberingSeperator->text();
        //if (file_separator.isEmpty()) file_separator="-";
        QString file_ext=cb_OutputType->currentText();
        int sf=getStartFrame();
        QString sft=frameToTime(sf).toString("hh:mm:ss.zzz");
        int tf=getTotalFrames();
        int idig=(QString("%1").arg(timeToFrame(isEndTime))).count();
        QString informatonMedia="";
        QString colordepth=getColorDepth(true);
        if (cbOutputFileSizeUse->isChecked()){
            if (outputDimension.isValid()){
                informatonMedia.append(QString("%1%2x%3").arg(file_prefix.isEmpty()?"":dataImageSeparator).arg(outputDimension.width()).arg(outputDimension.height()));
            }
        }
        if (cbOutputFileDepthUse->isChecked()){
            if (!colordepth.isEmpty()){
                QString sep=(file_prefix.isEmpty() && informatonMedia.isEmpty())?"":dataImageSeparator;
                informatonMedia.append(QString("%1%2bit").arg(sep).arg(colordepth));
            }
        }

        if (!informatonMedia.isEmpty() && file_separator.isEmpty()) {
            informatonMedia.append(QString("%1").arg(dataImageSeparator));
        }

        QString firstfilename=QString("%1").arg(sf,idig,10,QLatin1Char('0'));
        QString outputfirstframe=QString().append(file_path).append(QString(QDir::separator())).append(file_prefix).append(informatonMedia).append(file_separator).append(firstfilename).append(file_ext);
        qDebug()<<"do checkDirectoryPermissions";
        if (!checkDirectoryPermissions(outputfirstframe)) return;
        qDebug()<<"pass checkDirectoryPermissions";
        QString digits=QString("%").append(QString("%1d").arg(idig,2,10,QLatin1Char('0')));
        QString output=QString().append(file_path).append(QString(QDir::separator())).append(file_prefix).append(informatonMedia).append(file_separator).append(digits).append(file_ext);
        QString imgParams;
        colordepth=getColorDepth(false);
        if (!colordepth.isEmpty())
            imgParams.append("-pix_fmt|").append(colordepth);

        file_ext.replace('.',"");
        if (ImageTypes_rawvideo.contains(file_ext)) {
            imgParams.append("|-vcodec|rawvideo");
        }
        QString exefile=QDir::toNativeSeparators(qApp->applicationDirPath()).append(QDir::separator()).append(ffmpegexecute);
        QString exeparam=QString("-v|32|-ss|%1|-i|%4|-start_number|%2|-vframes|%3|%5|-f|image2").arg(sft).arg(sf).arg(tf).arg(movieName).arg(imgParams);
        exeparam.append("|").append(output);
        qDebug()<<exefile<<exeparam;
        //ffmpeg -start_number 1 -vframes 222 -i foo.avi -r 1 -s WxH -f image2 foo-%03d.jpeg
        ExecuteExtApp(exefile,false,exeparam);
    }
}

bool ImgSeqExtractControl::checkDirectoryPermissions(QString pathfile)
{
    QFileInfo fi=QFileInfo(pathfile);
    if (fi.exists() && fi.isWritable()){
        int ret=QMessageBox::question(this,tr("Files present"),tr("The filename file already exists.\nDo you want to overwrite it?"));
        //qDebug()<<"checkDirectoryPermissions ret "<<ret;
        if (ret!=QMessageBox::Yes) return false;
    }
    //qDebug()<<"checkDirectoryPermissions"<<pathfile<<true;
    return true;
}


void ImgSeqExtractControl::ExecuteExtApp(QString file,bool searchEnv, QString param){
    QProcessEnvironment env;
    QStringList envSearch;
    if (searchEnv){
        envSearch << "ProgramFiles(x86)" << "ProgramFiles" << "ProgramW6432";
    }else{
        envSearch << "";
    }
    bool foundEnv = false;
    QFileInfo fi;
    QString pathfile;
    for (int i=0; i< envSearch.size();i++){
        //qDebug()<<"ExecuteExtApp"<<i;
        QString progPath = env.systemEnvironment().value(envSearch.at(i));
        if (!progPath.isEmpty()) progPath.append("\\");
        pathfile=QString(progPath.append(file));
        fi=QFileInfo(pathfile);
        if (fi.exists() && fi.isExecutable()){
            foundEnv=true;
            break;
        }
    }
    if (foundEnv){
        if (param.isEmpty()){
            // can run app with administarative rights request if need, but without params
            QDesktopServices::openUrl(QUrl(QString("file:///%1").arg(fi.absoluteFilePath()), QUrl::TolerantMode));
        }else{
            //QProcess::startDetached(fi.absoluteFilePath(), param.split(" "));
            builder=new QProcess();
            builder->setProcessChannelMode(QProcess::MergedChannels);
            qDebug()<<"builder.start"<<fi.absoluteFilePath()<< param.split("|");
            QObject::connect(builder,SIGNAL( finished(int,QProcess::ExitStatus) ),SLOT( onEXE_finished(int,QProcess::ExitStatus) ));
            QObject::connect(builder,SIGNAL( started() ),SLOT( onEXE_started() ));
            builder->start(fi.absoluteFilePath(), param.split("|"));
        }
    }else{
        QMessageBox::warning(this, tr("Run external application"),
                             tr("File not found for execute\n%1"
                                ).arg(pathfile),
                             QMessageBox::Cancel);
    }
}

void ImgSeqExtractControl::setMovieName(QString name)
{
    movieName=name;//QDir::toNativeSeparators
}

QStringList ImgSeqExtractControl::getImageTypes(){
    QStringList IT = QStringList();
    IT.append(ImageTypes_8bit);
    IT.append(ImageTypes_10bit);
    IT.append(ImageTypes_12bit);
    IT.append(ImageTypes_14bit);
    IT.append(ImageTypes_16bit);
    IT.removeDuplicates();
    IT.sort();
    return IT;
}


void ImgSeqExtractControl::setupOutputType()
{
    cb_OutputType->clear();
    QStringList IT = getImageTypes();
    for (int i=0; i<IT.size(); i++)
    {
        cb_OutputType->addItem(QString(".").append(IT.at(i)));
    }
}

void ImgSeqExtractControl::setupColorTypeOutput(int id)
{
    Q_UNUSED(id);
    QString cType=cb_OutputType->currentText();
    cType.replace('.',"");
    cbColorTypeOutput->clear();
    if (ImageTypes_8bit.indexOf(cType)!=-1) cbColorTypeOutput->addItem("8-bit");
    if (ImageTypes_10bit.indexOf(cType)!=-1) cbColorTypeOutput->addItem("10-bit");
    if (ImageTypes_12bit.indexOf(cType)!=-1) cbColorTypeOutput->addItem("12-bit");
    if (ImageTypes_14bit.indexOf(cType)!=-1) cbColorTypeOutput->addItem("14-bit");
    if (ImageTypes_16bit.indexOf(cType)!=-1) cbColorTypeOutput->addItem("16-bit");

    if(ImageTypes_rawvideo.indexOf(cType)!=-1) {
        cbOutputFileDepthUse->setChecked(true);
        cbOutputFileSizeUse->setChecked(true);
    }
}

QString ImgSeqExtractControl::getColorDepth(bool numberOutput)
{
    QString file_colorspace=cbColorTypeOutput->currentText();
    QString pixfmt;
    int cd=file_colorspace.split("-")[0].toInt();

    QString currentOutputType=cb_OutputType->currentText();
    currentOutputType.replace('.',"");
    qDebug()<<"ImgSeqExtractControl::getColorDepth currentOutputType"<<currentOutputType;
    if (!numberOutput){
        switch (cd) {
        case 8:
            if (currentOutputType.endsWith("420")) {
                pixfmt="yuv420p";
            }else if (currentOutputType.endsWith("422")) {
                pixfmt="yuv422p";
            }else if (currentOutputType.startsWith("RGBA")) {
                pixfmt="rgb32";
            }else if (currentOutputType.endsWith("RGB")) {
                pixfmt="rgb24";
            }else{
                pixfmt="rgb24";
            }
            break;
        case 10:
            if (currentOutputType.startsWith("420")) {
                pixfmt="yuv420p10le";
            }else if (currentOutputType.startsWith("422")) {
                pixfmt="yuv422p10le";
            }
            break;
        case 16:
            if (currentOutputType.startsWith("RGBA")) {
                pixfmt="rgba64le";
            }else if (currentOutputType.endsWith("RGB")) {
                pixfmt="rgb48le";
            } else {
                pixfmt="rgb48le";
            }
            break;
        }
    }else{
        pixfmt=QString("%1").arg(cd);
    }
    return pixfmt;
}

void ImgSeqExtractControl::onEXE_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug()<<"ImgSeqExtractControl::onEXE_finished"<<exitCode<<exitStatus;
    if (modalinfo)
        modalinfo->done(QMessageBox::NoButton);
    if (exitStatus==QProcess::NormalExit) {
        if (exitCode){
            QString outtext = QString(builder->readAll());
            QMessageBox::warning(this, tr("Extract"),
                                 tr("Extraction ffmpeg error:\n%1").arg(outtext),
                                 QMessageBox::Ok);
        }else{
            int tf=getTotalFrames();
            QMessageBox::information(this, tr("Extract"),
                                     tr("Frame Extraction Completed!\nTotal Frames Extracted: %1").arg(tf),
                                     QMessageBox::Ok);
        }
    }
}

void ImgSeqExtractControl::onEXE_started()
{
    qDebug()<<"ImgSeqExtractControl::onEXE_started()";
    modalinfo=new QMessageBox();
    modalinfo->setIcon(QMessageBox::Information);
    modalinfo->setWindowTitle(tr("Extract"));
    modalinfo->setText(tr("Frame Extraction has started"));
    modalinfo->setStandardButtons(QMessageBox::Abort);

    //place inside QMessageBox();
    QGridLayout *modalinfoLayout = (QGridLayout*) modalinfo->layout();
    QVBoxLayout *vl = new QVBoxLayout();
    QHBoxLayout *hlprocess = new QHBoxLayout();
    EXEprogressProgressBar = new QProgressBar(modalinfo);
    EXEprogressProgressBar->setObjectName(QStringLiteral("progressBar"));
    EXEprogressProgressBar->setAlignment(Qt::AlignCenter);
    EXEprogressProgressBar->setValue(0);
    EXEprogressProgressBar->setToolTip("% OF EXTRACTED FRAMES");
    hlprocess->addWidget(EXEprogressProgressBar);
    vl->addLayout(hlprocess);
    QHBoxLayout *hlfps = new QHBoxLayout();
    EXEprogressFpsBar = new QProgressBar(modalinfo);
    EXEprogressFpsBar->setObjectName(QStringLiteral("fpsBar"));
    EXEprogressFpsBar->setAlignment(Qt::AlignCenter);
    EXEprogressFpsBar->setFormat(QString("%v ").append(tr("fps")).append(" (").append(tr("max").append(" %m)") ));
    EXEprogressFpsBar->setValue(0);
    EXEprogressFpsBar->setMaximum(1);
    EXEprogressFpsBar->setToolTip(tr("Frames per second, speed of exctraction"));
    hlfps->addWidget(EXEprogressFpsBar);
    vl->addLayout(hlfps);
    modalinfoLayout->addLayout(vl,modalinfoLayout->rowCount()-2,1,1,modalinfoLayout->columnCount(),Qt::AlignCenter);
    //add custom width of QMessageBox
    QSpacerItem* horizontalSpacer = new QSpacerItem(300, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    modalinfoLayout->addItem(horizontalSpacer,modalinfoLayout->rowCount(),0,1,modalinfoLayout->columnCount());

    QTimer::singleShot(500, this, SLOT(updateEXEprogress()));

    int result=modalinfo->exec();
    if (result==QMessageBox::Abort){
        builder->kill();
        QMessageBox::warning(this, tr("Extract"),
                             tr("Extraction was terminated by user"),
                             QMessageBox::Ok);
    }
}

void ImgSeqExtractControl::updateEXEprogress()
{
    if (modalinfo) {
        if (!modalinfo->isHidden() && EXEprogressProgressBar && EXEprogressFpsBar) {
            detectFFMpegOutputFrames();
            QTimer::singleShot(500, this, SLOT(updateEXEprogress()));
        }
    }
}

void ImgSeqExtractControl::detectFFMpegOutputFrames()
{
    QString outtext = QString(builder->readAll());
    outtext.truncate(40);
    QString outtextfps=QString(outtext);
    if (outtext.startsWith("frame=")) {
        int frame=outtext.replace(QRegExp("^frame=\\s*(\\d+).*$"),"\\1").toInt();
        if (frame) updateProgressBar(frame);
        int fps=outtextfps.replace(QRegExp("^frame=.*\\sfps=\\s*(\\d+).*$"),"\\1").toInt();
        if (fps) updateFpsBar(fps);
    }
}

void ImgSeqExtractControl::updateProgressBar(int frame)
{
    if (frame) {
        int tf=getTotalFrames();
        int perc=((frame)/(qreal)tf)*100;
        if (perc) EXEprogressProgressBar->setValue(perc);
    }
}
void ImgSeqExtractControl::updateFpsBar(int val)
{
    if (val) {
        if (val>EXEprogressFpsBar->maximum()) EXEprogressFpsBar->setMaximum(val);
        EXEprogressFpsBar->setValue(val);
    }
}

void ImgSeqExtractControl::customStyles()
{
    setStyleSheet(" \
                  QSpinBox::up-arrow { \
                      image: url(:/theme/up_arrow-bw.png); \
                      width: 7px; \
                      height: 5px; \
                  } \
                  QSpinBox::down-arrow { \
                      image: url(:/theme/down_arrow-bw.png); \
                      width: 7px; \
                      height: 5px; \
                  } \
                  ");

}
void ImgSeqExtractControl::setOutputDimension(QSize size){
    if (size.isValid()){
        outputDimension=size;
    }
}
