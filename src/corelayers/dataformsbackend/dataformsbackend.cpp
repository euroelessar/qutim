#include "dataformsbackend.h"
#include "readonlydatalayout.h"
#include "editabledatalayout.h"
#include "src/modulemanagerimpl.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QKeyEvent>

namespace Core
{

static Core::CoreModuleHelper<DefaultDataFormsBackend> data_forms_static(
		QT_TRANSLATE_NOOP("Plugin", "Data forms backend"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM implementation of the data forms")
);

DefaultDataForm::DefaultDataForm(const DataItem &item, StandardButtons standartButtons,  const Buttons &buttons)
{
	setFrameStyle(NoFrame);
	setObjectName(item.name());
	setWindowTitle(item.title());
	if (item.isReadOnly()) {
		m_layout = new ReadOnlyDataLayout(this);
	} else {
		m_layout = new EditableDataLayout(this);
	}
	bool addSpacer;
	if (item.hasSubitems())
		addSpacer = !m_layout->addItems(item.subitems());
	else
		addSpacer = !m_layout->addItem(item);
	if (addSpacer)
		m_layout->addSpacer();
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
		m_layout->addRow(buttonsBox);
	}
}

DataItem DefaultDataForm::item() const
{
	DataItem item;
	EditableDataLayout *editableDataLayout = qobject_cast<EditableDataLayout*>(m_layout);
	if (editableDataLayout)
		item = editableDataLayout->item();
	item.setName(objectName());
	item.setTitle(windowTitle());
	return item;
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

QWidget *DefaultDataFormsBackend::get(const DataItem &item, AbstractDataForm::StandardButtons standartButtons, const AbstractDataForm::Buttons &buttons)
{
	if (item.isNull())
		return 0;
	if (!item.hasSubitems() && standartButtons == AbstractDataForm::NoButton && buttons.isEmpty())
		return EditableDataLayout::getWidget(item);
	else
		return new DefaultDataForm(item, standartButtons, buttons);
}

}
