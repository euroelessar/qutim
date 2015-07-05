#ifndef MACSEARCHFIELD_H
#define MACSEARCHFIELD_H

#import <Cocoa/Cocoa.h>
#include <QString>

namespace Core { namespace SimpleContactList {
class MacWidget;
} }

@interface CoreMacSearchField : NSSearchField
{
@public
	Core::SimpleContactList::MacWidget *widget;
	NSString *itemIdentifier;
}
@end

//class MacSearchFieldWidget;

//class MacSearchField : public QWidget
//{
//	Q_OBJECT
//	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
//public:
//	MacSearchField(QWidget *parent = 0);
//	~MacSearchField();

//	QString text() const;
//	void setText(const QString &text);

//	QSize sizeHint() const;

//signals:
//	void textChanged(const QString &text);

//protected:
//	void resizeEvent(QResizeEvent *);

//private:
//	friend class MacSearchFieldWidget;
//	MacSearchFieldWidget *m_widget;
//	QString m_text;
//};

#endif // MACSEARCHFIELD_H
