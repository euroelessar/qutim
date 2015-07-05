#include "completionmodel.h"

namespace QuickChat {

CompletionModel::CompletionModel(QObject *parent) :
	QObject(parent), m_model(nullptr)
{
}

bool CompletionModel::isValid() const
{
	return m_model && m_model->rowCount() > 0;
}

QObject *CompletionModel::model() const
{
	return m_model;
}

void CompletionModel::setModel(QObject *model)
{
	if (m_model == model)
		return;

	m_model = qobject_cast<QAbstractItemModel *>(model);
	emit modelChanged(m_model);
}

static int calculateCommonPrefix(const QString &first, const QString &second)
{
	int length = std::min(first.size(), second.size());

	for (int index = 0; index < length; ++index) {
		if (first.at(index).toLower() != second.at(index).toLower())
			return index;
	}

	return length;
}

QStringList CompletionModel::suggestions(const QString &prefix)
{
	if (!m_model)
		return QStringList();

	QStringList result;

	int length = 0;
	for (int i = 0, count = m_model->rowCount(); i < count; ++i) {
		const QModelIndex index = m_model->index(i, 0);
		const QString text = m_model->data(index).toString();
		const int commonPrefixLength = calculateCommonPrefix(text, prefix);

		if (length < commonPrefixLength) {
			length = commonPrefixLength;
			result.clear();
		}

		if (length == commonPrefixLength)
			result << text;
	}

	return result;
}

} // namespace QuickChat
