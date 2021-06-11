#include "videoplayer.h"
#include "ui_videoplayer.h"
#include "qmessagebox.h"
#include <QTextCodec>

QTextCodec *codeb = QTextCodec::codecForName("GB18030");
videoplayer::videoplayer(QWidget *parent):QDialog(parent),
    ui(new Ui::videoplayer)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    play = new QMediaPlayer();
    playlist = new QMediaPlaylist();
    play->setPlaylist(playlist);
    play->setVideoOutput(ui->videoOutPut);
}

videoplayer::~videoplayer()
{
    delete ui;
}

void videoplayer::on_pushButton_clicked(bool checked)  //����Ƶ
{
     play->pause();
     currentMediaFilename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "D:\xch_cubesolve\cubesolve\video",
                                                    tr("Media (*.mp4 *.mkv *.*)"));
     playlist->addMedia(QUrl::fromLocalFile(currentMediaFilename));
     playlist->next();
     play->play();
}

void videoplayer::on_pushButton_2_clicked(bool checked)
{
    play->play();
}

void videoplayer::on_pushButton_3_clicked(bool checked)
{
    play->pause();
}

void videoplayer::on_pushButton_4_pressed()
{
    ui->videoOutPut->setFullScreen(true);
}

void videoplayer::closeEvent(QCloseEvent *event)
{
    int b=QMessageBox::question(this,QString(codeb->toUnicode("��ʾ")),QString(codeb->toUnicode("�Ƿ�ر���Ƶ����")),
                                QMessageBox::Yes |QMessageBox::No);
    if(b==QMessageBox::No)
    {
        event->ignore();
        return;
    }
    else
    {
        play->pause();
        event->accept();
    }
}
