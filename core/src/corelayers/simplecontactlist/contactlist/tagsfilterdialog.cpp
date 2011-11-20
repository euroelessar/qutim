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
#include "tagsfilterdialog.h"
#include "ui_tagsfilterdialog.h"
#include <qutim/icon.h>

namespace Core
{
namespace SimpleContactList
{
using namespace qutim_sdk_0_3;

TagsFilterDialog::TagsFilterDialog(const QStringList &tags,QWidget *parent) :
	QDialog(parent),
	ui(new Ui::TagsFilterDialog)
{
	ui->setupUi(this);
	setWindowIcon(Icon("feed-subscribe"));
	foreach (QString tag,tags) {
		QListWidgetItem *item = new QListWidgetItem (tag,ui->listWidget);
		item->setCheckState(Qt::Unchecked);
		ui->listWidget->addItem(item);
		m_items.insert(tag,item);
	}
}

TagsFilterDialog::~TagsFilterDialog()
{
	delete ui;
}

void TagsFilterDialog::changeEvent(QEvent *e)
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

QStringList TagsFilterDialog::selectedTags() const
{
	QStringList selected_tags;
	for (int i = 0;i!=ui->listWidget->count();i++) {
		QListWidgetItem *item = ui->listWidget->item(i);
		if (item->checkState() == Qt::Checked)
			selected_tags.append(item->text());
	}
	return selected_tags;
}

void TagsFilterDialog::setSelectedTags(const QStringList &tags)
{
	foreach (QString tag,tags) {
		m_items[tag]->setCheckState(Qt::Checked);
	}
}

}
}

