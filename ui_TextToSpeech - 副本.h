/********************************************************************************
** Form generated from reading UI file 'TextToSpeechTZLOGI.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef TEXTTOSPEECHTZLOGI_H
#define TEXTTOSPEECHTZLOGI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TTSDlg
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *plainTextEdit;
    QGridLayout *gridLayout;
    QLabel *label_5;
    QLabel *label_3;
    QLabel *label_4;
    QSlider *pitch;
    QLabel *label_6;
    QSlider *volume;
    QComboBox *language;
    QComboBox *voice;
    QLabel *label;
    QLabel *label_2;
    QSlider *rate;
    QComboBox *engine;
    QHBoxLayout *horizontalLayout;
    QPushButton *speakButton;
    QPushButton *pauseButton;
    QPushButton *resumeButton;
    QPushButton *stopButton;
    QSpacerItem *verticalSpacer;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *TTSDlg)
    {
        if (TTSDlg->objectName().isEmpty())
            TTSDlg->setObjectName("TTSDlg");
        TTSDlg->setWindowModality(Qt::NonModal);
        TTSDlg->resize(551, 539);
        centralwidget = new QWidget(TTSDlg);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName("plainTextEdit");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(plainTextEdit);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName("label_5");
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_4, 5, 0, 1, 1);

        pitch = new QSlider(centralwidget);
        pitch->setObjectName("pitch");
        pitch->setMinimum(-10);
        pitch->setMaximum(10);
        pitch->setSingleStep(1);

        gridLayout->addWidget(pitch, 3, 2, 1, 1);

        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 6, 0, 1, 1);

        volume = new QSlider(centralwidget);
        volume->setObjectName("volume");
        volume->setMaximum(100);
        volume->setSingleStep(5);
        volume->setPageStep(20);
        volume->setValue(70);

        gridLayout->addWidget(volume, 1, 2, 1, 1);

        language = new QComboBox(centralwidget);
        language->setObjectName("language");
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(language->sizePolicy().hasHeightForWidth());
        language->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(language, 5, 2, 1, 1);

        voice = new QComboBox(centralwidget);
        voice->setObjectName("voice");

        gridLayout->addWidget(voice, 6, 2, 1, 1);

        label = new QLabel(centralwidget);
        label->setObjectName("label");
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        rate = new QSlider(centralwidget);
        rate->setObjectName("rate");
        rate->setMinimum(-10);
        rate->setMaximum(10);

        gridLayout->addWidget(rate, 2, 2, 1, 1);

        engine = new QComboBox(centralwidget);
        engine->setObjectName("engine");
        sizePolicy2.setHeightForWidth(engine->sizePolicy().hasHeightForWidth());
        engine->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(engine, 4, 2, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        speakButton = new QPushButton(centralwidget);
        speakButton->setObjectName("speakButton");

        horizontalLayout->addWidget(speakButton);

        pauseButton = new QPushButton(centralwidget);
        pauseButton->setObjectName("pauseButton");
        pauseButton->setEnabled(false);

        horizontalLayout->addWidget(pauseButton);

        resumeButton = new QPushButton(centralwidget);
        resumeButton->setObjectName("resumeButton");
        resumeButton->setEnabled(false);

        horizontalLayout->addWidget(resumeButton);

        stopButton = new QPushButton(centralwidget);
        stopButton->setObjectName("stopButton");

        horizontalLayout->addWidget(stopButton);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer);

        TTSDlg->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(TTSDlg);
        statusbar->setObjectName("statusbar");
        TTSDlg->setStatusBar(statusbar);
#if QT_CONFIG(shortcut)
        label_4->setBuddy(language);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(plainTextEdit, speakButton);
        QWidget::setTabOrder(speakButton, pauseButton);
        QWidget::setTabOrder(pauseButton, resumeButton);
        QWidget::setTabOrder(resumeButton, stopButton);

        retranslateUi(TTSDlg);

        QMetaObject::connectSlotsByName(TTSDlg);
    } // setupUi

    void retranslateUi(QMainWindow *TTSDlg)
    {
        TTSDlg->setWindowTitle(QCoreApplication::translate("TTSDlg", "MainWindow", nullptr));
        plainTextEdit->setPlainText(QCoreApplication::translate("TTSDlg", "Hello QtTextToSpeech,\n"
"this is an example text in English.\n"
"\n"
"QtSpeech is a library that makes text to speech easy with Qt.\n"
"Done, over and out.", nullptr));
        label_5->setText(QCoreApplication::translate("TTSDlg", "Engine", nullptr));
        label_3->setText(QCoreApplication::translate("TTSDlg", "Pitch:", nullptr));
        label_4->setText(QCoreApplication::translate("TTSDlg", "&Language:", nullptr));
        label_6->setText(QCoreApplication::translate("TTSDlg", "Voice name:", nullptr));
        label->setText(QCoreApplication::translate("TTSDlg", "Rate:", nullptr));
        label_2->setText(QCoreApplication::translate("TTSDlg", "Volume:", nullptr));
        speakButton->setText(QCoreApplication::translate("TTSDlg", "Speak", nullptr));
        pauseButton->setText(QCoreApplication::translate("TTSDlg", "Pause", nullptr));
        resumeButton->setText(QCoreApplication::translate("TTSDlg", "Resume", nullptr));
        stopButton->setText(QCoreApplication::translate("TTSDlg", "Stop", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TTSDlg: public Ui_TTSDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // TEXTTOSPEECHTZLOGI_H
