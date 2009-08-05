#ifndef SOUNDENGINESETTINGS_H
#define SOUNDENGINESETTINGS_H

#include <QtGui/QWidget>

namespace Ui {
    class SoundEngineSettings;
}

class SoundEngineSettings : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(SoundEngineSettings)
public:
    explicit SoundEngineSettings(const QString &profile_name, QWidget *parent = 0);
    virtual ~SoundEngineSettings();
    void loadSettings();
    void saveSettings();

public slots:
    void widgetStateChanged() { emit settingsChanged(); }
    void on_commandButton_clicked();

signals:
    void settingsChanged();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::SoundEngineSettings *m_ui;
    QString m_profile_name;
    bool m_changed;
};

#endif // SOUNDENGINESETTINGS_H
