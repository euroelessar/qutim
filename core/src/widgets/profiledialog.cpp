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

#include "profiledialog.h"
#include "profilelistwidget.h"
#include "ui_profiledialog.h"
#include <qutim/systeminfo.h>
#include <qutim/json.h>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include "profilecreationwizard.h"
#include <qutim/objectgenerator.h>
#include <qutim/jsonfile.h>
#include <qutim/cryptoservice.h>
#include <qutim/config.h>
#include <qutim/icon.h>
#include <qutim/profile.h>
#include <QCryptographicHash>
#include <QTimer>
#include <QScrollBar>

namespace Core
{
ProfileDialog::ProfileDialog(Config &config, ModuleManager *parent) :
    ui(new Ui::ProfileDialog)
{
	m_manager = parent;
	ui->setupUi(this);

	connect(ui->profileList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(currentItemChanged(QListWidgetItem *, QListWidgetItem *)));

	QString current = config.value("current", QString());
	int size = config.beginArray("list");
	int itemHeight = 70;

	ui->profileList->setGridSize(QSize(0, itemHeight));
	ui->profileList->setFrameStyle(QFrame::NoFrame);
	ui->profileList->setMinimumSize(ui->profileList->minimumSize().width(), itemHeight);
	
	if (size == 0) {
		// TODO
	} else {
		int cid = 0;
		for (int i = 0; i < size; i++) {
			Config group = config.arrayElement(i);

			QListWidgetItem *item = new QListWidgetItem(ui->profileList);
			item->setSizeHint(QSize(0, itemHeight));
			item->setData(Qt::UserRole + 1, qVariantFromValue(group));

			QString id = group.value("id", QString());
			if (id == current)
				cid = ui->profileList->count() - 1;
			ProfileListWidget *w = new ProfileListWidget(id, group.value("configDir", QString()));
			connect(w, SIGNAL(submit(const QString &)), this, SLOT(login(const QString &)));
			w->setMinimumSize(w->minimumSize().width(), itemHeight);
			ui->profileList->setItemWidget(item, w);
		}
		ui->profileList->setCurrentRow(cid);
	}
	config.endArray();

	// Temporary
	ui->profilesButton->setText("Add profile");
}

ProfileDialog::~ProfileDialog()
{
    delete ui;
}

Config ProfileDialog::profilesInfo()
{
	return Profile::instance()->config();
}

QString ProfileDialog::profilesConfigPath(bool *isSystem)
{
	return Profile::instance()->configPath(isSystem);
}

bool ProfileDialog::acceptProfileInfo(const Config &config, const QString &password, bool checkHash)
{
	QString errors;
	bool result = Profile::instance()->acceptData(config.rootValue().toMap(), password, checkHash, &errors);
	if (!result)
		QMessageBox::critical(QApplication::activeWindow(), tr("Error while loading"), errors);
	return result;
}

void ProfileDialog::login(const QString &password)
{
	QVariant variant = ui->profileList->currentItem()->data(Qt::UserRole + 1);
	Config config = variant.value<Config>();
	JsonFile file(profilesConfigPath());
	QVariant var;
	QVariantMap varMap;
	file.load(var);
	varMap = var.toMap();
	Config(&varMap).setValue("current", config.value("id", QString()));
	file.save(varMap);
	if (acceptProfileInfo(config, password)) {
		QTimer::singleShot(0, m_manager, SLOT(initExtensions()));
		deleteLater();
	}
}

void ProfileDialog::on_profilesButton_clicked()
{
	QString name = QInputDialog::getText(this, tr("Enter name"), tr("Profile name:"), QLineEdit::Normal);
	if (name.isEmpty()) {
		QMessageBox::critical(this, tr("Invalid name"), tr("Name can not be empty!"));
		return;
	}

	QString pass = QInputDialog::getText(this, tr("Enter password"), tr("Password:"), QLineEdit::Password);
	QString passr = QInputDialog::getText(this, tr("Repeat password"), tr("Repeat password:"), QLineEdit::Password);

	if (pass != passr) {
		QMessageBox::critical(this, tr("Incorrect password"), tr("Passwords don't match each other"));
		return;
	}

	if (pass.isEmpty()) {
		QMessageBox::critical(this, tr("Incorrect password"), tr("Password can not be empty!"));
		return;
	}

	QWizard *wizard = new ProfileCreationWizard(m_manager, name, pass);
#if	defined(QUTIM_MOBILE_UI)
	wizard->showMaximized();
#else
	wizard->show();
#endif
	connect(wizard, SIGNAL(accepted()), this, SLOT(deleteLater()));
	connect(wizard, SIGNAL(rejected()), this, SLOT(show()));
	hide();
}

void ProfileDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ProfileDialog::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (previous)
		((ProfileListWidget*)ui->profileList->itemWidget(previous))->activate(false);
	if (current)
		((ProfileListWidget*)ui->profileList->itemWidget(current))->activate(true);
}
}

