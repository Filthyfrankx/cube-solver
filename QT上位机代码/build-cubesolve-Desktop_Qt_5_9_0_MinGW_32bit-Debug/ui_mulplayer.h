/********************************************************************************
** Form generated from reading UI file 'mulplayer.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MULPLAYER_H
#define UI_MULPLAYER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <playerslider.h>

QT_BEGIN_NAMESPACE

class Ui_MulPlayer
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    PlayerSlider *slider;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *openFileBtn;
    QPushButton *playBtn;
    QPushButton *stopBtn;

    void setupUi(QDialog *MulPlayer)
    {
        if (MulPlayer->objectName().isEmpty())
            MulPlayer->setObjectName(QStringLiteral("MulPlayer"));
        MulPlayer->resize(776, 546);
        verticalLayout_2 = new QVBoxLayout(MulPlayer);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

        verticalLayout_2->addLayout(horizontalLayout);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        slider = new PlayerSlider(MulPlayer);
        slider->setObjectName(QStringLiteral("slider"));
        slider->setMinimumSize(QSize(0, 30));
        slider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(slider);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        openFileBtn = new QPushButton(MulPlayer);
        openFileBtn->setObjectName(QStringLiteral("openFileBtn"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(openFileBtn->sizePolicy().hasHeightForWidth());
        openFileBtn->setSizePolicy(sizePolicy);
        openFileBtn->setMinimumSize(QSize(0, 45));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        openFileBtn->setFont(font);

        horizontalLayout_2->addWidget(openFileBtn);

        playBtn = new QPushButton(MulPlayer);
        playBtn->setObjectName(QStringLiteral("playBtn"));
        playBtn->setMinimumSize(QSize(0, 45));
        playBtn->setFont(font);

        horizontalLayout_2->addWidget(playBtn);

        stopBtn = new QPushButton(MulPlayer);
        stopBtn->setObjectName(QStringLiteral("stopBtn"));
        stopBtn->setMinimumSize(QSize(0, 45));
        stopBtn->setFont(font);

        horizontalLayout_2->addWidget(stopBtn);


        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_2->addLayout(verticalLayout);

        verticalLayout_2->setStretch(0, 5);
        verticalLayout_2->setStretch(1, 1);

        retranslateUi(MulPlayer);

        QMetaObject::connectSlotsByName(MulPlayer);
    } // setupUi

    void retranslateUi(QDialog *MulPlayer)
    {
        MulPlayer->setWindowTitle(QApplication::translate("MulPlayer", "\350\247\206\351\242\221\346\274\224\347\244\272", Q_NULLPTR));
        openFileBtn->setText(QApplication::translate("MulPlayer", "\351\200\211\346\213\251", Q_NULLPTR));
        playBtn->setText(QApplication::translate("MulPlayer", "\346\222\255\346\224\276", Q_NULLPTR));
        stopBtn->setText(QApplication::translate("MulPlayer", "\346\232\202\345\201\234", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MulPlayer: public Ui_MulPlayer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MULPLAYER_H
