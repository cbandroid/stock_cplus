/********************************************************************************
** Form generated from reading UI file 'TextToSpeech.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTTOSPEECH_H
#define UI_TEXTTOSPEECH_H
#include <QDialog> 
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "globalvar.h"

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

    void setupUi(QDialog *TTSDlg)
    {
        if (TTSDlg->objectName().isEmpty())
            TTSDlg->setObjectName("TTSDlg");

         TTSDlg->resize(355, 410);
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
        label_5->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);
        label_3->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName("label_4");
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);
        label_4->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_4, 5, 0, 1, 1);

        pitch = new QSlider(centralwidget);
        pitch->setObjectName("pitch");
        pitch->setMinimum(-10);
        pitch->setMaximum(10);
        pitch->setSingleStep(1);
        pitch->setOrientation(Qt::Orientation::Horizontal);

        gridLayout->addWidget(pitch, 3, 2, 1, 1);

        label_6 = new QLabel(centralwidget);
        label_6->setObjectName("label_6");
        label_6->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_6, 6, 0, 1, 1);

        volume = new QSlider(centralwidget);
        volume->setObjectName("volume");
        volume->setMaximum(100);
        volume->setSingleStep(2);
        volume->setPageStep(10);
        volume->setValue(70);
        volume->setOrientation(Qt::Orientation::Horizontal);

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
        label->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);
        label_2->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        rate = new QSlider(centralwidget);
        rate->setObjectName("rate");
        rate->setMinimum(-10);
        rate->setMaximum(10);
        rate->setSingleStep(1);
        rate->setValue( GlobalVar::speechrate*10.0f);
     
        rate->setOrientation(Qt::Orientation::Horizontal);

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

#if QT_CONFIG(shortcut)
        label_4->setBuddy(language);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(plainTextEdit, speakButton);
        QWidget::setTabOrder(speakButton, pauseButton);
        QWidget::setTabOrder(pauseButton, resumeButton);
        QWidget::setTabOrder(resumeButton, stopButton);

        retranslateUi(TTSDlg);

      //  QMetaObject::connectSlotsByName(TTSDlg);
    } // setupUi

    void retranslateUi(QDialog  *TTSDlg)
    {
        TTSDlg->setWindowTitle(QCoreApplication::translate("TTSDlg", "语音设置", nullptr));
        plainTextEdit->setPlainText(QCoreApplication::translate("TTSDlg", "股市有风险，投资需谨慎！缠论量化，是人类历史上第一个数\
学上可以严格证明的技术分析策略，包含严格笔，线段作为最低级别，中枢的构成，级别的扩展，第一二三类买卖点，\
同级别分解的，背驰的判断，区间套。\n\
  不懂缠论，就永远在财务自由的路上，这路能走通不，对于走路的人，是未知的。我这里能提供的只有工具，答案就在所\
有的缠论原文之中，经过回测，已经无数次的验证了其有效性。无论你是几个亿以下的小资金，还是大资金，都有方\
法。请坚定信念！如果你失败了一年，不妨再继续失败两年，当你绝望之际，终会回到原文，逼迫自己，求得真知。\
一亿多次访问量的博客，是不会骗你的。这正如无数人参加过的高考，有几个高中清华北大的。有缘者自会明白。已经\
明白的也不必来这了，只与还在路上，并且经历失望又还是充满信心的人共勉。你们掌握了缠论，的确是会胜利的。\
答案就在那原文当中，就好比高考题的答案都在无数的教辅，教材之中一样。参与市场，解决他人流动性需求，自己\
还能持续赚钱生存。但是那个介入，退出的买卖点，在哪里？有缘人自会找到它的！\n\
   缠论最大的问题是，走势类型无法完成。不能完成，最低级别的终结之点就无法固定，买入的时间就无从知晓。那\
种区间套所存在的情况，极其少见。 解决这个问题的方法有两种。第一种不说，第二种是，多头利用回拉中枢dd的必然\
上涨幅度，来忽略起止点的无法确定所带来的问题。 不足之处是回拉中枢dd，一般只在第二个中枢之后才出现，这在大级别\
上，下跌第二个中枢很少见。而且大级别的中枢级别，容易判断错误，很容易九段升级。 缠论量化，人类历史上第\
一个胜率严格为100%的技术分析策略，包含严格笔，线段作为最低级别，中枢的构成，级别的扩展，第一二三类买卖\
点，同级别分解，背驰的判断，区间套。\n\
  需要注意的问题: 笔和线段的划分是标准化的,意味着不可能在这方面上千人千缠。但是标准化的笔和段，里面有\
很多细节是可以有多种选择的。只要按照相同的标准进行划分的笔和线段，就能够构建最低级别的基本构件，这就是合\
理的。哪怕按照3根K线来构建最小级别中枢，也是合理的。线段，只是增强了稳定性和可操作性。 这多种选择包括：\n\
 笔的划分： 是否次高点成笔，包含关系意义下的5根k线是否成笔，下降笔起始点顶分型第一根K线是否可以低点低于\
后面底分型低点，是否允许修正笔，大幅度跳空是否成笔，等等。\n\
  线段的划分： 跳空成笔的笔当做线段，线段起始点不是最低最高点的标准化，价格相同是否认为是创新高，等等。当然按\
照定义，可以解决大部分问题。\n\
  走势类型的划分： 走势的多义性，扩张中枢次级别的判定，小转大，中枢震荡中的小转大，同级别分解的如何确认当下分解，\
线段类走势的级别，突发事件第二类买卖点的处理，等等。\n\
  需要注意，本策略的前提是假设没有流动性风险，也就是不会存在只有一个人交易,导致没有足够的对手方的情况.那种大资金\
的操作,不适合用本策略.\n\
  凡是认为缠论会因为知道的人多了,就渐渐失效的,不愿意交流,这问题禅师本人早有叙述.万有引力定律,永远不失效!", nullptr));
        label_5->setText(QCoreApplication::translate("TTSDlg", "语音驱动", nullptr));
        label_3->setText(QCoreApplication::translate("TTSDlg", "音高", nullptr));
        label_4->setText(QCoreApplication::translate("TTSDlg", "语言", nullptr));
        label_6->setText(QCoreApplication::translate("TTSDlg", "朗读者", nullptr));
        label->setText(QCoreApplication::translate("TTSDlg", "语速", nullptr));
        label_2->setText(QCoreApplication::translate("TTSDlg", "音量", nullptr));
        speakButton->setText(QCoreApplication::translate("TTSDlg", "朗读", nullptr));
        pauseButton->setText(QCoreApplication::translate("TTSDlg", "暂停", nullptr));
        resumeButton->setText(QCoreApplication::translate("TTSDlg", "继续", nullptr));
        stopButton->setText(QCoreApplication::translate("TTSDlg", "停止", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TTSDlg: public Ui_TTSDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTTOSPEECH_H
