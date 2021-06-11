#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QDialog>
#include <QFileDialog>
#include <QString>
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class videoplayer;
}

class videoplayer : public QDialog
{
    Q_OBJECT

public:
    explicit videoplayer(QWidget *parent = 0);
    ~videoplayer();
    void closeEvent(QCloseEvent *);

private slots:
    void on_pushButton_clicked(bool checked);//打开视频
    void on_pushButton_2_clicked(bool checked);//播放视频
    void on_pushButton_3_clicked(bool checked);//暂停视频
    void on_pushButton_4_pressed();//全屏

private:
    Ui::videoplayer *ui;
    QString currentMediaFilename;
    QMediaPlayer *play;
    QMediaPlaylist  *playlist;
};

#endif // VIDEOPLAYER_H
