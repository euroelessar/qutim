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
#ifndef JACCOUNTWIZARD_H
#define JACCOUNTWIZARD_H

#include <qutim/protocol.h>
#include <qutim/extensioninfo.h>

namespace Jabber {

	class JAccountWizardPage;
	class JProtocol;

	enum JAccountType {
		AccountTypeJabber,
		AccountTypeLivejournal,
		AccountTypeGoogletalk,
		AccountTypeQip,
		AccountTypeYandex
	};

	class JAccountWizard : public qutim_sdk_0_3::AccountCreationWizard
	{
		Q_OBJECT
		Q_CLASSINFO("DependsOn", "Jabber::JProtocol")
		public:
			JAccountWizard();
			~JAccountWizard();
			QList<QWizardPage *> createPages(QWidget *parent);
			void createAccount();
		protected:
			JAccountType type;
		private:
			JAccountWizardPage *page;
			JProtocol *protocol;
	};

	class LJAccountWizard : public JAccountWizard
	{
		Q_OBJECT
		public:
			LJAccountWizard();
			~LJAccountWizard();
	};

	class GTAccountWizard : public JAccountWizard
	{
		Q_OBJECT
		public:
			GTAccountWizard();
			~GTAccountWizard();
	};

	class YAccountWizard : public JAccountWizard
	{
		Q_OBJECT
		public:
			YAccountWizard();
			~YAccountWizard();
	};

	class QIPAccountWizard : public JAccountWizard
	{
		Q_OBJECT
		public:
			QIPAccountWizard();
			~QIPAccountWizard();
	};
}

#endif // JACCOUNTWIZARD_H

