#include "soundenginesettings.h"
#include "defaultsoundenginelayer.h"
#include <QFileDialog>
#include "ui_soundenginesettings.h"

SoundEngineSettings::SoundEngineSettings(const QString &profile_name, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::SoundEngineSettings)
{
    m_changed = true;
    m_profile_name = profile_name;
    m_ui->setupUi(this);
#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
    m_ui->qSoundRadioButton->setEnabled(true);
#else
	m_ui->qSoundRadioButton->hide();
#endif
    loadSettings();
    connect(m_ui->noSoundRadioButton, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->qSoundRadioButton, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->commandRadioButton, SIGNAL(toggled(bool)), this, SLOT(widgetStateChanged()));
    connect(m_ui->commandLineEdit, SIGNAL(textChanged(QString)), this, SLOT(widgetStateChanged()));
    m_changed = false;
}

SoundEngineSettings::~SoundEngineSettings()
{
    delete m_ui;
}

void SoundEngineSettings::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SoundEngineSettings::loadSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");

    settings.beginGroup("sounds");
    DefaultSoundEngineLayer::EngineType type = static_cast<DefaultSoundEngineLayer::EngineType>(settings.value("soundengine", 0).toInt());
    m_ui->noSoundRadioButton->setChecked(type==DefaultSoundEngineLayer::NoSound);
    m_ui->qSoundRadioButton->setChecked(type==DefaultSoundEngineLayer::LibSound);
    m_ui->commandRadioButton->setChecked(type==DefaultSoundEngineLayer::UserCommand);
#ifndef Q_OS_WIN32
    m_ui->commandLineEdit->setText(settings.value("command", "play \"%1\"").toString());
#else
    m_ui->commandLineEdit->setText(settings.value("command", "").toString());
#endif
    settings.endGroup();
}

void SoundEngineSettings::saveSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("sounds");
    if(m_ui->noSoundRadioButton->isChecked())
        settings.setValue("soundengine", DefaultSoundEngineLayer::NoSound);
    else if(m_ui->qSoundRadioButton->isChecked())
        settings.setValue("soundengine", DefaultSoundEngineLayer::LibSound);
    else if(m_ui->commandRadioButton->isChecked())
        settings.setValue("soundengine", DefaultSoundEngineLayer::UserCommand);
    settings.setValue("command", m_ui->commandLineEdit->text());
    settings.endGroup();
}

void SoundEngineSettings::on_commandButton_clicked()
{
#ifdef Q_OS_WIN32
    QString bin_name = QFileDialog::getOpenFileName(this,
        tr("Select command path"), QDir::currentPath(),
        tr("Executables (*.exe *.com *.cmd *.bat)\nAll files (*.*)"));
#else
    QString bin_name = QFileDialog::getOpenFileName(this,
        tr("Select command path"), "/bin",
        tr("Executables"));
#endif // WIN32
    if (bin_name.isEmpty())
        return;

#ifndef Q_OS_WIN32
    m_ui->commandLineEdit->setText(bin_name + " \"%1\"");
#else
    m_ui->commandLineEdit->setText(QString("\"%1\" \"%2\"").arg(bin_name).arg("%1"));
#endif // WIN32
}
