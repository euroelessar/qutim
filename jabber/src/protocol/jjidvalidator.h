#ifndef JJIDVALIDATOR_H
#define JJIDVALIDATOR_H

#include <QValidator>
#include <string.h>

namespace Jabber
{
class JJidValidatorPrivate;
class JJidValidator : public QValidator
{
	Q_DECLARE_PRIVATE(JJidValidator)
	Q_OBJECT
public:
	JJidValidator(const QString &server = QString(), QObject *parent = 0);
	~JJidValidator();
	QString server() const;
	virtual State validate(QString &, int &) const;
	virtual void fixup(QString &) const;
protected:
	QScopedPointer<JJidValidatorPrivate> d_ptr;
};
}

#endif // JJIDVALIDATOR_H
