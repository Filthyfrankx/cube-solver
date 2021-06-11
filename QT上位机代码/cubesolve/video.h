#ifndef VIDEO_H
#define VIDEO_H

#include <QWidget>
#include <QVideoWidget>
#include <QKeyEvent>
class video : public QVideoWidget
{
    Q_OBJECT
public:
    explicit video(QWidget *parent = nullptr);

signals:

public slots:
    void keyPressEvent(QKeyEvent *event)
    {
        if(event->key()==Qt::Key_Escape)
        {
            this->setFullScreen(false);
        }
    }
};

#endif // VIDEO_H
