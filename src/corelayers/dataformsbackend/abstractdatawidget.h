#ifndef ABSTRACTDATAWIDGET_H
#define ABSTRACTDATAWIDGET_H

#include <qutim/dataforms.h>

namespace Core {

	class AbstractDataWidget
	{
	public:
		AbstractDataWidget();
		virtual ~AbstractDataWidget();
		virtual qutim_sdk_0_3::DataItem item() const = 0;
	};

} // namespace Core

Q_DECLARE_INTERFACE(Core::AbstractDataWidget, "org.qutim.core.AbstractDataWidget");

#endif // ABSTRACTDATAWIDGET_H
