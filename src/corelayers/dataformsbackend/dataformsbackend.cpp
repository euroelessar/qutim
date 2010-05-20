#include "dataformsbackend.h"
#include "readonlydatalayout.h"
#include "editabledatalayout.h"
#include "src/modulemanagerimpl.h"
#include <QDialogButtonBox>
#include <QPushButton>

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
		connect(buttonsBox, SIGNAL(accepted()), SIGNAL(accepted()));
		connect(buttonsBox, SIGNAL(rejected()), SIGNAL(rejected()));
		connect(buttonsBox, SIGNAL(helpRequested()), SIGNAL(helpRequested()));
		connect(buttonsBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onButtonClicked(QAbstractButton*)));
		m_layout->addWidget(buttonsBox);
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

DefaultDataFormsBackend::DefaultDataFormsBackend()
{
}

QWidget *DefaultDataFormsBackend::get(const DataItem &item, AbstractDataForm::StandardButtons standartButtons, const AbstractDataForm::Buttons &buttons)
{
	if (item.isNull())
		return 0;
	if (!item.hasSubitems() && standartButtons == AbstractDataForm::NoButton && buttons.isEmpty())
		return EditableDataLayout::getEditableWidget(item);
	else
		return new DefaultDataForm(item, standartButtons, buttons);
}

}
