/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "libqutim_global.h"
#include <QWidget>
#include <QScopedPointer>

namespace qutim_sdk_0_3
{
	struct SettingsWidgetPrivate;
	class AutoSettingsGenerator;
	
//	class LIBQUTIM_EXPORT Settings
//	{
//	public:
//		virtual ~Settings();
//		virtual void doLoad() = 0;
//		virtual void doSave() = 0;
//		virtual void doCancel() = 0;
//	};

	class LIBQUTIM_EXPORT SettingsWidget : public QWidget
	{
		Q_OBJECT
		Q_PROPERTY(bool modified READ isModified NOTIFY modifiedChanged)
	public:
		SettingsWidget(QWidget *parent = 0);
		virtual ~SettingsWidget();
		bool isModified() const;
		virtual void setController(QObject *controller);
	public slots:
		void load();
		void save();
		void cancel();
	signals:
#if !defined(Q_MOC_RUN) && !defined(DOXYGEN_SHOULD_SKIP_THIS) && !defined(IN_IDE_PARSER)
private: // don't tell moc, doxygen or kdevelop, but those signals are in fact private
#endif
		void modifiedChanged(bool have_changes);
		void saved();
	protected:
		virtual void loadImpl() = 0;
		virtual void saveImpl() = 0;
		virtual void cancelImpl() = 0;
		void setModified(bool modified);
		void listenChildrenStates(const QWidgetList &exceptions = QWidgetList());
		const char *lookForWidgetState(QWidget *widget, const char *property = 0, const char *signal = 0);
		virtual void virtual_hook(int id, void *data);
	private slots:
		void onStateChanged(int index);
	private:
		friend class AutoSettingsGenerator;
		QScopedPointer<SettingsWidgetPrivate> p;
	};
}

#endif // SETTINGSWIDGET_H

