#include "settingslayer.h"
#include <QtGui/QApplication>
#include "libqutim_p.h"

namespace qutim_sdk_0_3
{
	SettingsItem::SettingsItem() : m_type(SettingsInvalid), m_text(0)
	{
	}

	SettingsItem::~SettingsItem()
	{
		clearWidget();
	}

	SettingsType SettingsItem::type() const
	{
		if(m_type < SettingsGeneral || m_type > SettingsProtocol)
			return SettingsInvalid;
		return m_type;
	}

	QIcon SettingsItem::icon() const
	{
		// If icon is null choose it by type
		return m_icon;
	}

	QString SettingsItem::text() const
	{
		return qApp->translate("Settings", m_text, 0, QApplication::UnicodeUTF8);
	}

	QWidget *SettingsItem::widget() const
	{
		if(!m_widget)
			m_widget = generateWidget();
		return m_widget;
	}

	void SettingsItem::clearWidget()
	{
		if(!m_widget.isNull())
			delete m_widget.data();
	}

	QPointer<SettingsLayer> SettingsLayer::instance = 0;

	SettingsLayer::SettingsLayer()
	{
	}

	SettingsLayer::~SettingsLayer()
	{
	}

}
