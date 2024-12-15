/********************************************************************************
** Form generated from reading UI file 'StartUpDlg.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STARTUPDLG_H
#define UI_STARTUPDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_StartUpDlg
{
public:
    QDialogButtonBox *btnBox;
    QComboBox *cmbExServer;
    QComboBox *cmbHqServer;
    QTextEdit *RegEdt;
    QCheckBox *AutoChk;
    QCheckBox *exChk;
    QLabel *lblExServer;
    QLabel *lblHqServer;
    QLabel *lblReg;
    QLabel *lblWarn;
    QProgressBar *loadProgress;

    void setupUi(QDialog *StartUpDlg)
    {
        if (StartUpDlg->objectName().isEmpty())
            StartUpDlg->setObjectName("StartUpDlg");
        StartUpDlg->setWindowModality(Qt::WindowModal);
        StartUpDlg->resize(387, 225);
        QFont font;
        font.setPointSize(12);
        StartUpDlg->setFont(font);
        btnBox = new QDialogButtonBox(StartUpDlg);
        btnBox->setObjectName("btnBox");
        btnBox->setGeometry(QRect(0, 180, 371, 41));
        btnBox->setOrientation(Qt::Horizontal);
        btnBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        cmbExServer = new QComboBox(StartUpDlg);
        cmbExServer->setObjectName("cmbExServer");
        cmbExServer->setGeometry(QRect(170, 20, 201, 23));
        QFont font1;
        font1.setPointSize(11);
        cmbExServer->setFont(font1);
        cmbHqServer = new QComboBox(StartUpDlg);
        cmbHqServer->setObjectName("cmbHqServer");
        cmbHqServer->setGeometry(QRect(171, 50, 201, 23));
        cmbHqServer->setFont(font1);
        RegEdt = new QTextEdit(StartUpDlg);
        RegEdt->setObjectName("RegEdt");
        RegEdt->setGeometry(QRect(23, 130, 341, 31));
        RegEdt->setInputMethodHints(Qt::ImhNone);
        RegEdt->setAcceptRichText(false);
        AutoChk = new QCheckBox(StartUpDlg);
        AutoChk->setObjectName("AutoChk");
        AutoChk->setGeometry(QRect(160, 80, 191, 21));
        QFont font2;
        font2.setPointSize(12);
        font2.setBold(true);
        AutoChk->setFont(font2);
        AutoChk->setStyleSheet(QString::fromUtf8("color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 0,0, 255));"));
        exChk = new QCheckBox(StartUpDlg);
        exChk->setObjectName("exChk");
        exChk->setGeometry(QRect(150, 20, 16, 21));
        lblExServer = new QLabel(StartUpDlg);
        lblExServer->setObjectName("lblExServer");
        lblExServer->setGeometry(QRect(10, 20, 141, 16));
        lblExServer->setFont(font1);
        lblHqServer = new QLabel(StartUpDlg);
        lblHqServer->setObjectName("lblHqServer");
        lblHqServer->setGeometry(QRect(90, 50, 81, 20));
        lblHqServer->setFont(font1);
        lblReg = new QLabel(StartUpDlg);
        lblReg->setObjectName("lblReg");
        lblReg->setGeometry(QRect(30, 100, 331, 20));
        lblWarn = new QLabel(StartUpDlg);
        lblWarn->setObjectName("lblWarn");
        lblWarn->setGeometry(QRect(40, 160, 331, 20));
        lblWarn->setFont(font2);
        lblWarn->setStyleSheet(QString::fromUtf8("color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 0,0, 255));"));
        lblWarn->setMidLineWidth(4);
        lblWarn->setScaledContents(true);
        loadProgress = new QProgressBar(StartUpDlg);
        loadProgress->setObjectName("loadProgress");
        loadProgress->setGeometry(QRect(20, 130, 341, 23));
        loadProgress->setValue(24);
        loadProgress->raise();
        cmbExServer->raise();
        cmbHqServer->raise();
        RegEdt->raise();
        AutoChk->raise();
        exChk->raise();
        lblExServer->raise();
        lblHqServer->raise();
        lblReg->raise();
        lblWarn->raise();
        btnBox->raise();
        QWidget::setTabOrder(exChk, cmbExServer);
        QWidget::setTabOrder(cmbExServer, cmbHqServer);
        QWidget::setTabOrder(cmbHqServer, AutoChk);
        QWidget::setTabOrder(AutoChk, RegEdt);

        retranslateUi(StartUpDlg);
        QObject::connect(btnBox, &QDialogButtonBox::accepted, StartUpDlg, qOverload<>(&QDialog::accept));
        QObject::connect(btnBox, &QDialogButtonBox::rejected, StartUpDlg, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(StartUpDlg);
    } // setupUi

    void retranslateUi(QDialog *StartUpDlg)
    {
        StartUpDlg->setWindowTitle(QCoreApplication::translate("StartUpDlg", "\351\200\211\346\213\251\346\234\215\345\212\241\345\231\250", nullptr));
        AutoChk->setText(QCoreApplication::translate("StartUpDlg", "\350\207\252\345\212\250\351\200\211\346\213\251\350\277\236\346\216\245\346\234\215\345\212\241\345\231\250", nullptr));
        exChk->setText(QString());
        lblExServer->setText(QCoreApplication::translate("StartUpDlg", "\346\234\237\350\264\247,\346\270\257\350\202\241\350\241\214\346\203\205\346\234\215\345\212\241\345\231\250", nullptr));
        lblHqServer->setText(QCoreApplication::translate("StartUpDlg", "\350\241\214\346\203\205\346\234\215\345\212\241\345\231\250", nullptr));
        lblReg->setText(QCoreApplication::translate("StartUpDlg", "\346\263\250\345\206\214\347\211\271\345\276\201\347\240\201", nullptr));
        lblWarn->setText(QCoreApplication::translate("StartUpDlg", "\350\202\241\345\270\202\346\234\211\351\243\216\351\231\251\357\274\214\346\212\225\350\265\204\351\241\273\350\260\250\346\205\216!", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StartUpDlg: public Ui_StartUpDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STARTUPDLG_H
