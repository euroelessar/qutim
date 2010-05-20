#ifndef DATAFORMSBACKEND_H
#define DATAFORMSBACKEND_H

#include "libqutim/dataforms.h"
#include "abstractdatalayout.h"

class QAbstractButton;

namespace Core
{

using namespace qutim_sdk_0_3;

class DefaultDataForm : public AbstractDataForm
{
	Q_OBJECT
public:
	DefaultDataForm(const DataItem &item, StandardButtons standartButtons = NoButton,  const Buttons &buttons = Buttons());
	virtual DataItem item() const;
private slots:
	void onButtonClicked(QAbstractButton *button);
private:
	AbstractDataLayout *m_layout;
};

class DefaultDataFormsBackend : public DataFormsBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "DataFormsBackend")
public:
	DefaultDataFormsBackend();
	virtual QWidget *get(const DataItem &item, AbstractDataForm::StandardButtons standartButtons = AbstractDataForm::NoButton,
						  const AbstractDataForm::Buttons &buttons = AbstractDataForm::Buttons());
};

}

#endif // DATAFORMSBACKEND_H
