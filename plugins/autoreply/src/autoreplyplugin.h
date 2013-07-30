#ifndef AUTOREPLYPLUGIN_H
#define AUTOREPLYPLUGIN_H

#include "autoreplymessagehandler.h"
#include <qutim/plugin.h>
#include <qutim/servicemanager.h>
#include <qutim/actiongenerator.h>
#include <qutim/settingslayer.h>
#include <QScopedPointer>
#include <QDateTime>

class AutoReplyPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
	Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
	Q_PROPERTY(QString replyText READ replyText WRITE setReplyText NOTIFY replyTextChanged)
	Q_PROPERTY(QDateTime backTime READ backTime WRITE setBackTime NOTIFY backTimeChanged)
	Q_PROPERTY(int deltaTime READ deltaTime NOTIFY deltaTimeChanged)
public:
	explicit AutoReplyPlugin();

	void init();
	bool load();
	bool unload();

	bool isActive() const;
	QString replyText() const;
	QDateTime backTime() const;
	int deltaTime() const;

public slots:
	void setActive(bool active);
	void setReplyText(const QString &replyText);
	void setBackTime(const QDateTime &backTime);

signals:
	void activeChanged(bool active);
	void replyTextChanged(const QString &replyText);
	void backTimeChanged(const QDateTime &backTime);
	void deltaTimeChanged(int deltaTime);

private slots:
	void onSettingsChanged();
	void onActionToggled(bool active);
	void onSecondsIdle(int idle);
	void onServiceChanged(const QByteArray &name);

private:
	QScopedPointer<AutoReplyMessageHandler> m_handler;
	QScopedPointer<qutim_sdk_0_3::SettingsItem> m_settingsItem;
	qutim_sdk_0_3::ServicePointer<QObject> m_idleManager;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_button;
	QString m_replyText;
	QDateTime m_backTime;
	bool m_active;
	int m_deltaTime;
	int m_idleTimeOut;
};

#endif // AUTOREPLYPLUGIN_H
