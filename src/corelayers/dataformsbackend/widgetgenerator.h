#ifndef WIDGETGENERATOR_H
#define WIDGETGENERATOR_H

#include <qutim/dataforms.h>

namespace Core {

QPixmap variantToPixmap(const QVariant &data, const QSize &size);
QStringList variantToStringList(const QVariant &data);

QWidget *getReadOnlyTitle(const qutim_sdk_0_3::DataItem &item, const Qt::Alignment &alignment, QWidget *parent = 0);
QWidget *getReadOnlyWidget(const qutim_sdk_0_3::DataItem &item, QWidget *parent = 0, bool *twoColumn = 0);

QWidget *getTitle(const qutim_sdk_0_3::DataItem &item, const Qt::Alignment &alignment, QWidget *parent = 0);
QWidget *getWidget(const qutim_sdk_0_3::DataItem &item, QWidget *parent = 0, bool *twoColumn = 0);
qutim_sdk_0_3::DataItem getDataItem(QWidget *title, QWidget *data);

} // namespace Core

#endif // WIDGETGENERATOR_H
