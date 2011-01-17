#ifndef ABSTRACTDATAWIDGET_H
#define ABSTRACTDATAWIDGET_H

#include <qutim/dataforms.h>

namespace Core {

class DefaultDataForm;

class AbstractDataWidget
{
public:
	AbstractDataWidget(const qutim_sdk_0_3::DataItem &item, DefaultDataForm *dataForm);
	virtual ~AbstractDataWidget();
	virtual qutim_sdk_0_3::DataItem item() const = 0;
	virtual void setData(const QVariant &data);
	DefaultDataForm *dataForm() { return m_dataForm; }
protected:
	qutim_sdk_0_3::DataItem m_item;
private:
	DefaultDataForm *m_dataForm;
};

} // namespace Core

Q_DECLARE_INTERFACE(Core::AbstractDataWidget, "org.qutim.core.AbstractDataWidget");

#endif // ABSTRACTDATAWIDGET_H
