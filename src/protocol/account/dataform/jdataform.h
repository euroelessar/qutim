#ifndef JDATAFORM_H
#define JDATAFORM_H

#include <QWidget>
#include <QGridLayout>
#include <gloox/jid.h>
#include <gloox/dataform.h>
#include <gloox/dataformfield.h>
#include <gloox/dataformfieldcontainer.h>

namespace Jabber
{
	using namespace gloox;

	struct JDataFormPrivate;

	class JDataForm : public QWidget
	{
		Q_OBJECT
		public:
			JDataForm(const DataForm *form, bool twocolumn = false, QWidget *parent = 0);
			~JDataForm();
			DataForm *getDataForm();
		private:
			QScopedPointer<JDataFormPrivate> p;
	};
}

#endif // JDATAFORM_H
