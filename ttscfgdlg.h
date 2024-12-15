#ifndef TTSCFGDLG_H
#define TTSCFGDLG_H
#include <QtTextToSpeech>
#include <QDialog>
class GlobalVar;

namespace Ui {
class TTSDlg;
}

class ttscfgDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ttscfgDlg(GlobalVar *pGlobalVar,QWidget *parent = nullptr);
    ~ttscfgDlg();

private:
    Ui::TTSDlg *ui;
    bool bInited;
    QTextToSpeech *m_speech = nullptr;
    QList<QVoice> m_voices;
    GlobalVar *m_pGlobalVar;
public slots:
    void setRate(int);
    void setPitch(int);
    void setVolume(int volume);

    void stateChanged(QTextToSpeech::State state);
    void engineSelected(int index);
    void languageSelected(int language);
    void voiceSelected(int index);

    void localeChanged(const QLocale &locale);
    
signals:
    void RateChanged(float value);
    void VolumeChanged(float value);
    void PitchChanged(float value);    

};

#endif // TTSCFGDLG_H
