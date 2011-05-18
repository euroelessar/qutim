#include "abstractdatawidget.h"

namespace Core {

AbstractDataWidget::AbstractDataWidget(const qutim_sdk_0_3::DataItem &item, DefaultDataForm *dataForm) :
	m_dataForm(dataForm)
{
	m_item = item;
	if (!m_item.isNull()) {
		// We need only item properties, right?
		m_item.setSubitems(QList<qutim_sdk_0_3::DataItem>());
	}
}

AbstractDataWidget::~AbstractDataWidget()
{
}

void AbstractDataWidget::setData(const QVariant &data)
{
	Q_UNUSED(data);
}

} // namespace Core
