#include "ttscfgdlg.h"
#include "ui_TextToSpeech.h"
#include "globalvar.h"
#include <QTextCodec>
#include <QMessageBox>
#include <QLoggingCategory>

using namespace Qt::StringLiterals;

ttscfgDlg::ttscfgDlg(GlobalVar *pGlobalVar,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TTSDlg)
{
    bInited=true;
    ui->setupUi(this);
    m_pGlobalVar=pGlobalVar;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QLoggingCategory::setFilterRules(u"qt.speech.tts=true \n qt.speech.tts.*=true"_s);

    // Populate engine selection list
    ui->engine->addItem(u"Default"_s, u"default"_s);
    const auto engines = QTextToSpeech::availableEngines();
    for (const QString &engine : engines)
        ui->engine->addItem(engine, engine);
    ui->engine->setCurrentIndex(0);
    engineSelected(0);
    ui->rate->setValue( m_pGlobalVar->speechrate*100.0f);

    connect(ui->pitch, &QSlider::valueChanged, this, &ttscfgDlg::setPitch);
    connect(ui->rate, &QSlider::valueChanged, this, &ttscfgDlg::setRate);
    connect(ui->volume, &QSlider::valueChanged, this, &ttscfgDlg::setVolume);
    connect(ui->engine, &QComboBox::currentIndexChanged, this, &ttscfgDlg::engineSelected);
    connect(ui->language, &QComboBox::currentIndexChanged, this, &ttscfgDlg::languageSelected);
    connect(ui->voice, &QComboBox::currentIndexChanged, this, &ttscfgDlg::voiceSelected);
    bInited=false;
}

void ttscfgDlg::setRate(int rate)
{
    if (bInited) return;
    m_pGlobalVar->speechrate=rate / 100.0f;

    m_speech->setRate(m_pGlobalVar->speechrate);
        
    emit RateChanged( m_pGlobalVar->speechrate);
}

void ttscfgDlg::setPitch(int pitch)
{
    if (bInited) return;
    float value=pitch / 100.0f;
    m_speech->setPitch(value);
    
    emit PitchChanged(value);    
}

void ttscfgDlg::setVolume(int volume)
{
    if (bInited) return;

    float Volvalue = volume / 100.0f;
    
    m_speech->setVolume(Volvalue );
    
    emit VolumeChanged(Volvalue );
}

//! [stateChanged]
void ttscfgDlg::stateChanged(QTextToSpeech::State state)
{
    ui->pauseButton->setEnabled(state == QTextToSpeech::Speaking);
    ui->resumeButton->setEnabled(state == QTextToSpeech::Paused);
    ui->stopButton->setEnabled(state == QTextToSpeech::Speaking || state == QTextToSpeech::Paused);
}
//! [stateChanged]

void ttscfgDlg::engineSelected(int index)
{
    QString engineName = ui->engine->itemData(index).toString();
    delete m_speech;
    m_speech = engineName == u"default"
                   ? new QTextToSpeech(this)
                   : new QTextToSpeech(engineName, this);

    // Block signals of the languages combobox while populating
    QSignalBlocker blocker(ui->language);

    ui->language->clear();
    const QList<QLocale> locales = m_speech->availableLocales();
    QLocale current = m_speech->locale();
    for (const QLocale &locale : locales) {
        QString name(u"%1 (%2)"_s
                         //.arg(QLocale::languageToString(locale.language()),
                             .arg(locale.nativeLanguageName(),
                              QLocale::territoryToString(locale.territory())));
        QVariant localeVariant(locale);
        ui->language->addItem(name, localeVariant);
        if (locale.name() == current.name())
            current = locale;
    }
    setRate(ui->rate->value());
    setPitch(ui->pitch->value());
    setVolume(ui->volume->value());
    //! [say]
    connect(ui->speakButton, &QPushButton::clicked, m_speech, [this]{
        m_speech->say(ui->plainTextEdit->toPlainText());
    });
    //! [say]
    //! [stop]
    connect(ui->stopButton, &QPushButton::clicked, m_speech, [this]{
        m_speech->stop();
    });
    //! [stop]
    //! [pause]
    connect(ui->pauseButton, &QPushButton::clicked, m_speech, [this]{
        m_speech->pause();
    });
    //! [pause]
    //! [resume]
    connect(ui->resumeButton, &QPushButton::clicked, m_speech, &QTextToSpeech::resume);
    //! [resume]

    connect(m_speech, &QTextToSpeech::stateChanged, this, &ttscfgDlg::stateChanged);
    connect(m_speech, &QTextToSpeech::localeChanged, this, &ttscfgDlg::localeChanged);

    blocker.unblock();

    localeChanged(current);
}

void ttscfgDlg::languageSelected(int language)
{
    QLocale locale = ui->language->itemData(language).toLocale();
    m_speech->setLocale(locale);
}

void ttscfgDlg::voiceSelected(int index)
{
    m_speech->setVoice(m_voices.at(index));
}

void ttscfgDlg::localeChanged(const QLocale &locale)
{
    QVariant localeVariant(locale);
    ui->language->setCurrentIndex(ui->language->findData(localeVariant));

    QSignalBlocker blocker(ui->voice);

    ui->voice->clear();

    m_voices = m_speech->availableVoices();
    QVoice currentVoice = m_speech->voice();
    for (const QVoice &voice : std::as_const(m_voices)) {
        ui->voice->addItem(u"%1 - %2 - %3"_s
                              .arg(voice.name(), QVoice::genderName(voice.gender()),
                                   QVoice::ageName(voice.age())));
        if (voice.name() == currentVoice.name())
            ui->voice->setCurrentIndex(ui->voice->count() - 1);
    }
}

ttscfgDlg::~ttscfgDlg()
{  
    delete ui;
}
