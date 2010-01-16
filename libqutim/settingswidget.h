/****************************************************************************
 *  settingswidget.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "libqutim_global.h"
#include <QWidget>
#include <QScopedPointer>

namespace qutim_sdk_0_3
{
	struct SettingsWidgetPrivate;
	class AutoSettingsGenerator;

	class LIBQUTIM_EXPORT SettingsWidget : public QWidget
	{
		Q_OBJECT
		Q_PROPERTY(bool modified READ isModified NOTIFY modifiedChanged)
	public:
		SettingsWidget();
		virtual ~SettingsWidget();
		bool isModified() const;
	public slots:
		void load();
		void save();
		void cancel();
	signals:
		void modifiedChanged(bool have_changes);
		void saved();
	protected:
		virtual void loadImpl() = 0;
		virtual void saveImpl() = 0;
		virtual void cancelImpl() = 0;
		void listenChildrenStates(const QWidgetList &exceptions = QWidgetList());
		const char *lookForWidgetState(QWidget *widget, const char *property = 0, const char *signal = 0);
	private slots:
		void onStateChanged(int index);
	private:
		friend class AutoSettingsGenerator;
		QScopedPointer<SettingsWidgetPrivate> p;
	};
}

#endif // SETTINGSWIDGET_H
