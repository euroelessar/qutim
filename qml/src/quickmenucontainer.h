#ifndef QUTIM_SDK_0_3_QUICKMENUCONTAINER_H
#define QUTIM_SDK_0_3_QUICKMENUCONTAINER_H

#include "quickactioncontainerbase.h"

namespace qutim_sdk_0_3 {

class QuickMenuContainer : public QuickActionContainerBase
{
	Q_OBJECT
public:
	explicit QuickMenuContainer(QObject *parent = 0);
	~QuickMenuContainer();

	void handleActionAdded(int index, QAction *action) override;
	void handleActionRemoved(int index) override;
	void handleActionsCleared() override;

signals:
	void actionAdded(int index, QuickAction *action);
	void actionRemoved(int index);

private:
	struct Entry
	{
		QuickAction *action;
		int type;
		bool separator;
	};

	int mapIndex(int originalIndex);

	void validateAllSeparators();
	void insertSeparator(int index);
	void insertAction(int index, const Entry &entry);
	void removeAction(int index);

	QList<int> m_types;
	QList<QuickAction *> m_originalActions;
	QList<QuickAction *> m_actions;
	QVector<Entry> m_entries;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKMENUCONTAINER_H
