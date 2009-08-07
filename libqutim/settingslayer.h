#ifndef SETTINGSLAYER_H
#define SETTINGSLAYER_H

#include "libqutim_global.h"
#include <QPointer>
#include <QWidget>
#include <QIcon>

namespace qutim_sdk_0_3
{
	enum SettingsType
	{
		SettingsInvalid,
		SettingsGeneral,
		SettingsAppearance,
		SettingsPlugin,
		SettingsProtocol
	};

	class LIBQUTIM_EXPORT SettingsItem
	{
		Q_DISABLE_COPY(SettingsItem);
	public:
		SettingsItem();
		virtual ~SettingsItem();
		SettingsType type() const;
		QIcon icon() const;
		QString text() const;
		QWidget *widget() const;
		void clearWidget();
	protected:
		virtual QWidget *generateWidget() const = 0;
		SettingsType m_type;
		QIcon m_icon;
		const char *m_text; // should be inserted by QT_TRANSLATE_NOOP_UTF8("Settings", "Contact list")
		mutable QPointer<QWidget> m_widget;
	};

	class LIBQUTIM_EXPORT SettingsLayer : public QObject
	{
		Q_OBJECT
	public:
	protected:
		SettingsLayer();
		virtual ~SettingsLayer();
	private:
		static QPointer<SettingsLayer> instance;
	};
}

#endif // SETTINGSLAYER_H
