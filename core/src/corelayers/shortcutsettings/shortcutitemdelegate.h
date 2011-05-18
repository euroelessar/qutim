#ifndef SHORTCUTITEMDELEGATE_H
#define SHORTCUTITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include <QPushButton>
#include <QMouseEvent>

namespace Core
{

	class ShortcutItemDelegate : public QAbstractItemDelegate
	{
		Q_OBJECT
	public:
		enum Role {
			GroupRole = Qt::UserRole,
			SequenceRole,
			IdRole,
			GlobalRole
		};
		ShortcutItemDelegate(QObject* parent = 0);
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		virtual void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
		virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
		virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
	private:
		virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);
		int m_vertical_padding;
		int m_horizontal_padding;
	};
	
}
#endif // SHORTCUTITEMDELEGATE_H
