/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef MIGRATIONSTARTPAGE_H
#define MIGRATIONSTARTPAGE_H

#include <QWizardPage>
#include <QDir>

namespace Ui {
    class MigrationStartPage;
}

namespace Core
{
	class MigrationStartPage : public QWizardPage
	{
		Q_OBJECT
	public:
		struct ProtoInfo
		{
			ProtoInfo(QString l, QString n, QString h, QString c) :
					lower(l), name(n), history(h), config(c) {}
			QString lower;
			QString name;
			QString history;
			QString config;
		};

		MigrationStartPage(QWidget *parent = 0);
		~MigrationStartPage();

		virtual void initializePage();
		virtual bool validatePage();

	protected slots:
		void on_profileBox_currentIndexChanged(int index);

	protected:
		void changeEvent(QEvent *e);

	private:
		QList<ProtoInfo> m_protos;
		Ui::MigrationStartPage *ui;
	};
}

#endif // MIGRATIONSTARTPAGE_H

