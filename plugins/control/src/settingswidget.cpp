/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "settingswidget.h"
#include "ui_settingswidget.h"
#include <qutim/config.h>
#include <qutim/icon.h>
#include <QPushButton>

Q_DECLARE_METATYPE(Control::AnswerLine::Guard)

namespace Control {

using namespace qutim_sdk_0_3;

AnswerLine::AnswerLine(const QString &text, QListWidget *list)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	m_textEdit = new QPlainTextEdit(this);
	
	QPushButton *button = new QPushButton(tr("Remove"), this);
	button->setIcon(qutim_sdk_0_3::Icon(QLatin1String("list-remove")));
	layout->addWidget(m_textEdit);
	layout->addWidget(button);
	connect(button, SIGNAL(clicked()), SLOT(onButtonClicked()));
	connect(m_textEdit, SIGNAL(textChanged()), SIGNAL(modified()));
	
	QSizePolicy policy = m_textEdit->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	m_textEdit->setSizePolicy(policy);
	m_textEdit->setMaximumHeight(100);
	
	m_item = new QListWidgetItem(list);
	QObject *guard = new QObject();
	connect(guard, SIGNAL(destroyed()), SLOT(deleteLater()));
	m_item->setData(Qt::UserRole, qVariantFromValue(Guard(guard)));
	m_item->setData(Qt::SizeHintRole, sizeHint());
	m_textEdit->setPlainText(text);
	list->setItemWidget(m_item, this);
}

QString AnswerLine::text() const
{
	return m_textEdit->toPlainText();
}

void AnswerLine::onButtonClicked()
{
	emit modified();
	delete m_item;
}

SettingsWidget::SettingsWidget()
    : ui(new Ui::SettingsWidget)
{
	ui->setupUi(this);
	QSizePolicy policy = ui->answersList->sizePolicy();
	policy.setVerticalPolicy(QSizePolicy::MinimumExpanding);
	ui->answersList->setSizePolicy(policy);
	listenChildrenStates(QWidgetList() << ui->answersList);
}

SettingsWidget::~SettingsWidget()
{
	delete ui;
}

void SettingsWidget::loadImpl()
{
	Config config("control");
	config.beginGroup("general");
	ui->usernameEdit->setText(config.value("username", QString()));
	ui->passwordEdit->setText(config.value("password", QString(), Config::Crypted));
	ui->urlEdit->setText(config.value("url", QString()));
	ui->requestUrlEdit->setText(config.value("requestUrl", QString()));
	ui->answersList->clear();
	const QStringList answers = config.value("answers", QStringList());
	foreach (const QString &text, answers)
		addAnswer(text);
}

void SettingsWidget::saveImpl()
{
	Config config("control");
	config.beginGroup("general");
	config.setValue("username", ui->usernameEdit->text());
	config.setValue("password", ui->passwordEdit->text(), Config::Crypted);
	config.setValue("url", ui->urlEdit->text());
	config.setValue("requestUrl", ui->requestUrlEdit->text());
	QStringList answers;
	for (int i = 0; i < ui->answersList->count(); ++i) {
		QListWidgetItem *item = ui->answersList->item(i);
		QWidget *widget = ui->answersList->itemWidget(item);
		AnswerLine *line = qobject_cast<AnswerLine*>(widget);
		answers << line->text();
	}
	config.setValue("answers", answers);
}

void SettingsWidget::cancelImpl()
{
	loadImpl();
}

void SettingsWidget::addAnswer(const QString &text)
{
	AnswerLine *line = new AnswerLine(text, ui->answersList);
	connect(line, SIGNAL(modified()), SLOT(onModified()));
}

void SettingsWidget::onModified()
{
	setModified(true);
}

void SettingsWidget::on_addButton_clicked()
{
	addAnswer();
	setModified(true);
}

} // namespace Control
