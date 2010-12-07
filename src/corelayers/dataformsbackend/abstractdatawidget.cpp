#include "abstractdatawidget.h"

namespace Core {

AbstractDataWidget::AbstractDataWidget()
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
