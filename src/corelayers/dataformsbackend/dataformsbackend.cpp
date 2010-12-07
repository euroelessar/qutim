#include "dataformsbackend.h"
#include "modifiablewidget.h"
#include "datalayout.h"
#include "widgetgenerator.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QKeyEvent>

#include <libqutim/icon.h>

Q_DECLARE_METATYPE(QList<QIcon>);

namespace Core
{

DefaultDataForm::DefaultDataForm(const DataItem &item, StandardButtons standartButtons,  const Buttons &buttons) :
	m_widget(0), m_isChanged(false), m_incompleteWidgets(0)
{
	DataLayout *dataLayout = 0;
	QVBoxLayout *layout = 0;
	setObjectName(item.name());
	setWindowTitle(item.title());
	if (item.isAllowedModifySubitems()) {
		layout = new QVBoxLayout(this);
		ModifiableWidget *w = new ModifiableWidget(item, this, this);
		m_widget = w;
		layout->addWidget(w);
		if (!w->isExpandable()) {
			QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
			layout->addItem(spacer);
		}
	} else {
		dataLayout = new DataLayout(this, this);
		m_widget = dataLayout;
		if (item.hasSubitems())
			dataLayout->addItems(item.subitems());
		else
			dataLayout->addItem(item);
		if (!dataLayout->isExpandable())
			dataLayout->addSpacer();
	}
	if (standartButtons != NoButton || !buttons.isEmpty()) {
		QDialogButtonBox *buttonsBox = new QDialogButtonBox(
				static_cast<QDialogButtonBox::StandardButton>(static_cast<int>(standartButtons)), Qt::Horizontal, this);
		foreach (const Button &button, buttons) {
			QPushButton *btn = buttonsBox->addButton(button.name, static_cast<QDialogButtonBox::ButtonRole>(button.role));
			btn->setObjectName(button.name.original());
		}
		connect(buttonsBox, SIGNAL(accepted()), SLOT(accept()));
		connect(buttonsBox, SIGNAL(rejected()), SLOT(reject()));
		connect(buttonsBox, SIGNAL(helpRequested()), SIGNAL(helpRequested()));
		connect(buttonsBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onButtonClicked(QAbstractButton*)));
		connect(this, SIGNAL(accepted()), SLOT(close()));
		connect(this, SIGNAL(rejected()), SLOT(close()));
		if (dataLayout)
			dataLayout->addRow(buttonsBox);
		else
			layout->addWidget(buttonsBox);
	}
}

DataItem DefaultDataForm::item() const
{
	DataItem item;
	if (m_widget)
		item = m_widget->item();
	item.setName(objectName());
	item.setTitle(windowTitle());
	return item;
}

bool DefaultDataForm::isChanged() const
{
	return m_isChanged;
}

bool DefaultDataForm::isComplete() const
{
	return m_incompleteWidgets == 0;
}

void DefaultDataForm::clearState()
{
	m_isChanged = false;
}

void DefaultDataForm::setData(const QString &name, const QVariant &data)
{
	foreach (AbstractDataWidget *widget, m_widgets.values(name))
		widget->setData(data);
}

void DefaultDataForm::dataChanged()
{
	if (!m_isChanged) {
		emit changed();
		m_isChanged = true;
	}
}

void DefaultDataForm::completeChanged(bool complete)
{
	if (complete) {
		--m_incompleteWidgets;
		Q_ASSERT(m_incompleteWidgets < 0);
		if (m_incompleteWidgets == 0)
			emit completeChanged(true);
	} else {
		if (m_incompleteWidgets == 0)
			emit completeChanged(false);
		++m_incompleteWidgets;
	}
}

void DefaultDataForm::onButtonClicked(QAbstractButton *button)
{
	emit clicked(button->objectName());
}

void DefaultDataForm::keyPressEvent(QKeyEvent *e)
{
#ifdef Q_WS_MAC
	if(e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Period) {
		e->accept();
		reject();
	} else
#endif
	if (!e->modifiers() || (e->modifiers() & Qt::KeypadModifier && e->key() == Qt::Key_Enter)) {
		switch (e->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return: {
			foreach (QPushButton *btn, qFindChildren<QPushButton*>(this)) {
				if (btn->isDefault() && btn->isVisible()) {
					if (btn->isEnabled())
						btn->click();
					e->accept();
					break;
				}
			}
			return;
		}
		case Qt::Key_Escape:
			e->accept();
			reject();
			break;
		}
	}
	AbstractDataForm::keyPressEvent(e);
}

DefaultDataFormsBackend::DefaultDataFormsBackend()
{
}

AbstractDataForm *DefaultDataFormsBackend::get(const DataItem &item,
											   AbstractDataForm::StandardButtons standartButtons,
											   const AbstractDataForm::Buttons &buttons)
{
	if (item.isNull())
		return 0;
	return new DefaultDataForm(item, standartButtons, buttons);
}

}
