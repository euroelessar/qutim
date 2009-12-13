#include "jdataform.h"

namespace Jabber
{
	struct JDataFormPrivate
	{
		JDataFormPrivate()
		{
			form = 0;
		}
		~JDataFormPrivate()
		{
			delete form;
		}
		DataForm *form;
		QList<JDataFormElement *> fields;
	};

	JDataForm::JDataForm(const DataForm *form, bool twocolumn, QWidget *parent)
			: QWidget(parent), p(new JDataFormPrivate)
	{
		p->form = new DataForm(*form);
		p->form->setType(TypeSubmit);
		QList<DataFormField*> fields = QList<DataFormField*>::fromStdList(form->fields());
		QGridLayout *layout = new QGridLayout();
		this->setLayout(layout);
		bool skip = false;
		int fieldCount = fields.count();
		for (int num = 0; num < fieldCount; num++) {
			DataFormField *field = fields[num];
			QLabel *label = new QLabel();
			JDataFormElement *df = 0;
			switch (field->type()) {
			case DataFormField::TypeTextPrivate:
				df = new JDFSingleText();
				qobject_cast<JDFSingleText *>(df->instance())->setEchoMode(QLineEdit::Password);
				break;
			case DataFormField::TypeTextSingle:
				df = new JDFSingleText();
				//widget = line_edits.last();
				break;
			case DataFormField::TypeTextMulti:
				df = new JDFMultiText();
				//widget = text_edits.last();
				break;
			case DataFormField::TypeBoolean:
				df = new JDFBoolean();
				qobject_cast<JDFBoolean *>(df->instance())->setText(QString::fromStdString(field->label()));
				break;
			case DataFormField::TypeHidden:
				df = new JDFHidden();
				break;
			case DataFormField::TypeListSingle:
				df = new JDFSingleList();
				{
					StringMultiMap options = field->options();
					StringMultiMap::const_iterator it2 = options.begin();
					for( ; it2 != options.end(); ++it2 )
						qobject_cast<JDFSingleList *>(df->instance())->addItem(
								QString::fromStdString((*it2).first),
								QString::fromStdString((*it2).second));
				}
				break;
			case DataFormField::TypeListMulti:
				df = new JDFMultiList();
				break;
			case DataFormField::TypeJidSingle:
				df = new JDFSingleJID();
				break;
			case DataFormField::TypeJidMulti:
				df = new JDFMultiJID();
				break;
			}
			if (field->type() != DataFormField::TypeFixed) {
				df->dfSetName(QString::fromStdString(field->name()));
				df->dfSetValue(QString::fromStdString(field->value()));
				p->fields.append(df);
			} else if (field->type() != DataFormField::TypeBoolean) {
				label->setText(QString::fromStdString(field->value()));
				label->setWordWrap(true);
			}
			if (field->type() == DataFormField::TypeHidden)
				continue;

			if (!twocolumn ||
					(num < fieldCount-1 && fields[num+1]->type() == DataFormField::TypeFixed
					|| num && fields[num-1]->type() == DataFormField::TypeFixed
					|| fields[num]->type() == DataFormField::TypeFixed)) {
				skip = false;
			}
			if (df) {
				QWidget *widget = qobject_cast<QWidget *>(df->instance());
				if (skip) {
					if (label->text().isEmpty()) {
						layout->addWidget(widget, layout->rowCount()-1, 2, 1, 2);
					} else {
						layout->addWidget(label, layout->rowCount()-1, 2);
						layout->addWidget(widget, layout->rowCount()-1, 3);
					}
				} else {
					if (label->text().isEmpty()) {
						layout->addWidget(widget, layout->rowCount(), 0, 1, 2);
					} else {
						layout->addWidget(label, layout->rowCount(), 0);
						layout->addWidget(widget, layout->rowCount()-1, 1);
					}
				}
			} else {
				layout->addWidget(label, layout->rowCount(), 0, 1, 2);
			}
			skip = !skip;
		}
	}

	JDataForm::~JDataForm()
	{
	}

	DataForm *JDataForm::getDataForm()
	{
		foreach (JDataFormElement *dataField, p->fields)
		{
			std::string name = dataField->dfName().toStdString();
			std::string value = dataField->dfValue().toStdString();
			p->form->field(name)->setValue(value);
		}
		return new DataForm(*p->form);
	}
}
