#ifndef GENERATORWINDOW_H
#define GENERATORWINDOW_H

#include <QtGui/QWizard>
#include "plugpackage.h"

#define qGen m_parent

class GeneratorWindow : public QWizard
{
    Q_OBJECT

public:
    GeneratorWindow(QWidget *parent = 0);
    ~GeneratorWindow();
	enum State { ChooseType, ChoosePath, Config, Save };
	enum Type { Art, Core, Lib, Plugin };
	inline void setType( Type type ) { m_type = type; }
	inline Type type() const { return m_type; }
private:
	Type m_type;
};

#endif // GENERATORWINDOW_H
