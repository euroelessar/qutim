#include "abstractdatawidget.h"

namespace Core {

AbstractDataWidget::AbstractDataWidget(DefaultDataForm *dataForm) :
	m_dataForm(dataForm)
{
}

AbstractDataWidget::~AbstractDataWidget()
{
}

void AbstractDataWidget::setData(const QVariant &data)
{
	Q_UNUSED(data);
}

} // namespace Core
