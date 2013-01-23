/********************************************************************************
** Form generated from reading UI file 'screenshoter.ui'
**
** Created: Tue Jan 22 19:29:39 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCREENSHOTER_H
#define UI_SCREENSHOTER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Screenshoter
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer;
    QSpinBox *spinBox;
    QComboBox *comboBox_2;
    QPushButton *ShotButton;
    QPushButton *Button_send;
    QPushButton *push_save;
    QPushButton *Button_cancel;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Screenshoter)
    {
        if (Screenshoter->objectName().isEmpty())
            Screenshoter->setObjectName(QString::fromUtf8("Screenshoter"));
        Screenshoter->resize(553, 394);
        QFont font;
        font.setFamily(QString::fromUtf8("Segoe UI"));
        Screenshoter->setFont(font);
        Screenshoter->setAcceptDrops(true);
        Screenshoter->setAutoFillBackground(false);
        centralWidget = new QWidget(Screenshoter);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setMaximumSize(QSize(10000, 10000));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(-1, -1, -1, 1);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);
        label_2->setMinimumSize(QSize(461, 311));
        label_2->setBaseSize(QSize(461, 291));
        label_2->setAutoFillBackground(false);
        label_2->setFrameShape(QFrame::NoFrame);
        label_2->setScaledContents(false);
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        comboBox = new QComboBox(centralWidget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        horizontalLayout->addWidget(comboBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        spinBox = new QSpinBox(centralWidget);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        spinBox->setMinimum(1);
        spinBox->setMaximum(20);
        spinBox->setValue(3);

        horizontalLayout->addWidget(spinBox);

        comboBox_2 = new QComboBox(centralWidget);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

        horizontalLayout->addWidget(comboBox_2);

        ShotButton = new QPushButton(centralWidget);
        ShotButton->setObjectName(QString::fromUtf8("ShotButton"));

        horizontalLayout->addWidget(ShotButton);

        Button_send = new QPushButton(centralWidget);
        Button_send->setObjectName(QString::fromUtf8("Button_send"));

        horizontalLayout->addWidget(Button_send);

        push_save = new QPushButton(centralWidget);
        push_save->setObjectName(QString::fromUtf8("push_save"));

        horizontalLayout->addWidget(push_save);

        Button_cancel = new QPushButton(centralWidget);
        Button_cancel->setObjectName(QString::fromUtf8("Button_cancel"));

        horizontalLayout->addWidget(Button_cancel);


        verticalLayout->addLayout(horizontalLayout);

        Screenshoter->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(Screenshoter);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        Screenshoter->setStatusBar(statusBar);

        retranslateUi(Screenshoter);

        QMetaObject::connectSlotsByName(Screenshoter);
    } // setupUi

    void retranslateUi(QMainWindow *Screenshoter)
    {
        Screenshoter->setWindowTitle(QApplication::translate("Screenshoter", "Screenshoter", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
        ShotButton->setText(QApplication::translate("Screenshoter", "Shot", 0, QApplication::UnicodeUTF8));
        Button_send->setText(QApplication::translate("Screenshoter", "Send", 0, QApplication::UnicodeUTF8));
        push_save->setText(QApplication::translate("Screenshoter", "Save as...", 0, QApplication::UnicodeUTF8));
        Button_cancel->setText(QApplication::translate("Screenshoter", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Screenshoter: public Ui_Screenshoter {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCREENSHOTER_H
