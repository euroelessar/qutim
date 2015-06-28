#ifndef QUTIM_SDK_0_3_QUICKACTIONCONTAINER_H
#define QUTIM_SDK_0_3_QUICKACTIONCONTAINER_H

#include "quickactioncontainerbase.h"

namespace qutim_sdk_0_3 {

class QuickActionContainer : public QuickActionContainerBase
{
	Q_OBJECT
	Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
	explicit QuickActionContainer(QObject *parent = 0);
	~QuickActionContainer();

	void handleActionAdded(int index, QAction *action) override;
	void handleActionRemoved(int index) override;
	void handleActionsCleared() override;

	int count() const;

public slots:
	QuickAction *item(int index);

signals:
	void countChanged(int count);
	void actionAdded(int index, QuickAction *action);
	void actionRemoved(int index);


private:
	QList<QuickAction *> m_actions;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKACTIONCONTAINER_H
