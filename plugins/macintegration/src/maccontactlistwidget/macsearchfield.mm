#include "macsearchfield.h"
#include <QMenu>
#include <QResizeEvent>
#import <Cocoa/Cocoa.h>
#include <QDebug>
#include <QApplication>

class MacSearchFieldWidget;

@interface MacSearchFieldImpl : NSSearchField
{
@public
	MacSearchFieldWidget *widget;
}
@end

static QString mac_NSString_to_QString(NSString *string)
{
	NSRange range = NSMakeRange(0, [string length]);
	QScopedArrayPointer<unichar> chars(new unichar[range.length + 1]);
	[string getCharacters: chars.data() range: range];
	return QString::fromUtf16(chars.data(), range.length);
}

class MacSearchFieldWidget : public QMacCocoaViewContainer
{
public:
	MacSearchFieldWidget(QWidget *parent)
		: QMacCocoaViewContainer(0, parent)
	{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		m_field = [[MacSearchFieldImpl alloc] init];
		m_field->widget = this;
		setCocoaView(m_field);

		retranslateUi();

		[m_field release];
		[pool release];
	}

	void changeEvent(QEvent *e)
	{
		QMacCocoaViewContainer::changeEvent(e);
		switch (e->type())
		{
		case QEvent::LanguageChange:
			retranslateUi();
			break;
		default:
			break;
		}
	}

	void retranslateUi()
	{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		QString title = QCoreApplication::translate("MacWidget", "Filter");
		NSString *nsTitle = [[NSString alloc] initWithUTF8String: title.toUtf8().constData()];
		[[m_field cell] setPlaceholderString: nsTitle];
		[pool release];
	}

	QSize sizeHint() const
	{
		return QSize(100, 25);
	}

	MacSearchFieldImpl *field() const
	{
		return m_field;
	}

	void updateText(const QString &text)
	{
		MacSearchField *q = static_cast<MacSearchField*>(parent());
		q->m_text = text;
		emit q->textChanged(text);
	}

	void focusOutEvent(QFocusEvent *event)
	{
		NSView *focused = NULL;
		if (qApp->focusWidget())
			focused = reinterpret_cast<NSView*>(qApp->focusWidget()->effectiveWinId());
		[[m_field window] makeFirstResponder: focused];
		QMacCocoaViewContainer::focusOutEvent(event);
	}

private:
	MacSearchFieldImpl *m_field;
};


@implementation MacSearchFieldImpl

- (void)dealloc
{
	[super dealloc];
}

- (void)textDidChange:(NSNotification *)obj
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *nsText = [self stringValue];
	QString result = mac_NSString_to_QString(nsText);
	widget->updateText(result);
	[pool release];
	[super textDidChange: obj];
}

- (BOOL)becomeFirstResponder
{
	BOOL result = [super becomeFirstResponder];
	if (!widget->hasFocus())
		widget->setFocus(Qt::OtherFocusReason);
	return result;
}

@end

MacSearchField::MacSearchField(QWidget *parent)
	: QWidget(parent)
{
	m_widget = new MacSearchFieldWidget(this);
	setFocusProxy(m_widget);
	setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));
}

MacSearchField::~MacSearchField()
{
}

QString MacSearchField::text() const
{
	return m_text;
}

void MacSearchField::setText(const QString &text)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	MacSearchFieldImpl *control = m_widget->field();
	NSString *nsOldText = [control stringValue];
	NSString *nsText = [[NSString alloc] initWithUTF8String: text.toUtf8().constData()];
	if ([nsText compare: nsOldText] == NSOrderedSame) {
		[pool release];
		return;
	}
	m_text = text;
	[control setStringValue: nsText];
	NSRange range = [[control currentEditor] selectedRange];
	range.location += range.length;
	range.length = 0;
	[[control currentEditor] setSelectedRange: range];
	[pool release];
	emit textChanged(m_text);
}

QSize MacSearchField::sizeHint() const
{
	return m_widget->sizeHint();
}


void MacSearchField::resizeEvent(QResizeEvent *event)
{
	m_widget->resize(event->size());
}
