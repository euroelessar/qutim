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
#include "soundthemeselector.h"
#include "ui_soundthemeselector.h"
#include <qutim/sound.h>
#include <QStandardItemModel>
#include <qutim/localizedstring.h>
#include <qutim/icon.h>
#include <QAbstractItemDelegate>
#include <QMetaEnum>

namespace Core
{
using namespace qutim_sdk_0_3;


SoundThemeSelector::SoundThemeSelector() :
	ui(new Ui::SoundThemeSelector),
	m_model(new QStandardItemModel(this))
{
	ui->setupUi(this);
	connect(ui->themeSelector, SIGNAL(currentIndexChanged(QString)), SLOT(currentIndexChanged(QString)));
	ui->treeView->setModel(m_model);
	connect(ui->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onClicked(QModelIndex)));
}

SoundThemeSelector::~SoundThemeSelector()
{
	delete ui;
}

void SoundThemeSelector::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void SoundThemeSelector::loadImpl()
{
	ui->themeSelector->addItem(QT_TRANSLATE_NOOP("Sound", "No sound"));
	QStringList theme_list = Sound::themeList();
	ui->themeSelector->addItems(theme_list);
	int index = ui->themeSelector->findText(Sound::currentThemeName());
	ui->themeSelector->setCurrentIndex(index == -1 ? 0 : index);
}
void SoundThemeSelector::saveImpl()
{
	QString name = ui->themeSelector->currentIndex() == 0 ?
						QString() :
						ui->themeSelector->currentText();
	Sound::setTheme(name);
}

void SoundThemeSelector::cancelImpl()
{

}

void SoundThemeSelector::currentIndexChanged(const QString& text)
{
	setModified(true);
	m_model->clear();
	fillModel(Sound::theme(text));
	ui->treeView->resizeColumnToContents(1);
}

void SoundThemeSelector::fillModel(const SoundTheme &theme)
{
	QStringList headers;
	headers.append(QT_TRANSLATE_NOOP("Notifications", "Type"));
	headers.append(QT_TRANSLATE_NOOP("SoundTheme", "Preview"));
	m_model->setHorizontalHeaderLabels(headers);

	for (int i = 0; i <= Notification::LastType; ++i) {
		Notification::Type type = static_cast<Notification::Type>(i);
		QList<QStandardItem *> items;

		QStandardItem *item = new QStandardItem(Notification::typeString(type));
		item->setToolTip(QT_TRANSLATE_NOOP("SoundTheme","Type"));
		item->setSelectable(false);
		items << item;

		item = new QStandardItem();
		item->setEditable(false);
		item->setIcon(Icon("media-playback-start"));
		item->setToolTip(QT_TRANSLATE_NOOP("SoundTheme","Play"));
		item->setEnabled(!theme.path(type).isNull());
		item->setSelectable(false);
		items << item;

		m_model->appendRow(items);
	}
	ui->treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void SoundThemeSelector::onClicked(const QModelIndex &index)
{
	if (index.column() != 1 || !(index.flags() & Qt::ItemIsEnabled))
		return;

	SoundTheme theme =  Sound::theme(ui->themeSelector->currentText());
	theme.play(static_cast<Notification::Type>(index.row()));
}

}

