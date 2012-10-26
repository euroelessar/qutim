#include "declarativeview.h"
#include "icon.h"
#include <QPointer>
#include <QDeclarativeItem>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QLayout>
#include <QDeclarativeContext>
#include <QGraphicsWidget>
#include <QDeclarativeImageProvider>

namespace qutim_sdk_0_3 {

class IconImageProvider: public QDeclarativeImageProvider
{
public:
    IconImageProvider()
        : QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap)
    {
    }

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
    {
        Q_UNUSED(size);
        int pos = id.lastIndexOf('/');
        QString iconName = id.right(id.length() - pos);
        Icon icon = Icon(iconName);
        if (size)
            *size = QSize(128, 128);

        int width = requestedSize.width() > 0 ? requestedSize.width() : size->width();
        return icon.pixmap(width);
    }
};

static QPointer<QDeclarativeEngine> m_engine;

class DeclarativeView;
class DeclarativeViewPrivate
{
	Q_DECLARE_PUBLIC(DeclarativeView)
public:
	DeclarativeViewPrivate(DeclarativeView *q) : q_ptr(q),
		rootContext(new QDeclarativeContext(q->engine(), q))
	{}
	DeclarativeView *q_ptr;
	QDeclarativeContext *rootContext;

	QPointer<QGraphicsObject> rootObject;
	QPointer<QDeclarativeItem> rootItem;
	QPointer<QGraphicsWidget> graphicsWidgetRoot;
	QPointer<QDeclarativeComponent> component;

	QUrl source;
	QSize initialSize;
	DeclarativeView::ResizeMode resizeMode;

	void initResize();
	inline QSize rootObjectSize() const;
	void execute();
	void _q_updateView();
	void _q_continueExecute();
};

namespace  {

void initEngine(QDeclarativeEngine *engine)
{
    engine->addImageProvider("qutim", new IconImageProvider);
}

} //namespace

void DeclarativeViewPrivate::initResize()
{
	Q_Q(DeclarativeView);
	if (rootItem) {
		//QDeclarativeItemPrivate *p =
		//    static_cast<QDeclarativeItemPrivate *>(QGraphicsItemPrivate::get(declarativeItemRoot));
		//p->addItemChangeListener(this, QDeclarativeItemPrivate::Geometry);
	} else if (graphicsWidgetRoot) {
		graphicsWidgetRoot->installEventFilter(q);
	}
	_q_updateView();
}

QSize DeclarativeViewPrivate::rootObjectSize() const
{
	QSize rootObjectSize(0,0);
	int widthCandidate = -1;
	int heightCandidate = -1;
	if (rootObject) {
		QSizeF size = rootObject->boundingRect().size();
		widthCandidate = size.width();
		heightCandidate = size.height();
	}
	if (widthCandidate > 0) {
		rootObjectSize.setWidth(widthCandidate);
	}
	if (heightCandidate > 0) {
		rootObjectSize.setHeight(heightCandidate);
	}
	return rootObjectSize;
}

void DeclarativeViewPrivate::execute()
{
	Q_Q(DeclarativeView);
	if (rootObject && rootObject->parent() == q)
		rootObject->deleteLater();
	if (component)
		component->deleteLater();
	if (!source.isEmpty()) {
		component = new QDeclarativeComponent(m_engine, source, q);
		if (!component->isLoading()) {
			_q_continueExecute();
		} else {
			QObject::connect(component.data(), SIGNAL(statusChanged(QDeclarativeComponent::Status)), q, SLOT(_q_continueExecute()));
		}
	}
}

void DeclarativeViewPrivate::_q_updateView()
{
	Q_Q(DeclarativeView);
	if (rootItem) {
		if (resizeMode == DeclarativeView::SizeViewToRootObject) {
			QSize newSize = QSize(rootItem->width(), rootItem->height());
			if (newSize.isValid() && newSize != q->size()) {
				q->resize(newSize);
			}
		} else {
			if (!qFuzzyCompare(q->width(), rootItem->width()))
				rootItem->setWidth(q->width());
			if (!qFuzzyCompare(q->height(), rootItem->height()))
				rootItem->setHeight(q->height());
			q->scene()->setSceneRect(0, 0, q->width(), q->height());
		}
	} else if (graphicsWidgetRoot) {
		if (resizeMode == DeclarativeView::SizeViewToRootObject){
			QSize newSize = QSize(graphicsWidgetRoot->size().width(), graphicsWidgetRoot->size().height());
			if (newSize.isValid() && newSize != q->size()) {
				q->resize(newSize);
			}
		} else {
			QSizeF newSize = QSize(q->size().width(), q->size().height());
			if (newSize.isValid() && newSize != graphicsWidgetRoot->size()) {
				graphicsWidgetRoot->resize(newSize);
			}
		}
	}
	q->updateGeometry();
}

void DeclarativeViewPrivate::_q_continueExecute()
{
	Q_Q(DeclarativeView);
	QObject::disconnect(component.data(), SIGNAL(statusChanged(QDeclarativeComponent::Status)), q, SLOT(_q_continueExecute()));

	if (component->isError()) {
		QList<QDeclarativeError> errorList = component->errors();
		foreach (const QDeclarativeError &error, errorList) {
			qWarning() << error;
		}
		return;
	}

	QObject *obj = component->beginCreate(rootContext);
	component->completeCreate();

	if(component->isError()) {
		QList<QDeclarativeError> errorList = component->errors();
		foreach (const QDeclarativeError &error, errorList) {
			qWarning() << error;
		}
		return;
	}

    q->setRootObject(obj);
}

DeclarativeView::DeclarativeView(QWidget *parent) :
	QGraphicsView(parent),
	d_ptr(new DeclarativeViewPrivate(this))
{
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// These seem to give the best performance
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

	viewport()->setFocusPolicy(Qt::NoFocus);
	setFocusPolicy(Qt::StrongFocus);
	setFrameShape(QFrame::NoFrame);

	setScene(new QGraphicsScene(this));
	scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene()->setStickyFocus(true);  //### needed for correct focus handling
}

DeclarativeView::~DeclarativeView()
{
}

void DeclarativeView::setEngine(QDeclarativeEngine *engine)
{
	Q_ASSERT(!m_engine);
	m_engine = engine;
    initEngine(engine);
}

QDeclarativeEngine *DeclarativeView::engine()
{
    if (!m_engine) {
        m_engine = new QDeclarativeEngine();
        initEngine(m_engine.data());
    }
	return m_engine;
}

void DeclarativeView::setSource(const QUrl &source)
{
	Q_D(DeclarativeView);
	d->source = source;
	d->execute();
}

QUrl DeclarativeView::source() const
{
	return d_func()->source;
}

QDeclarativeContext *DeclarativeView::rootContext() const
{
	return d_func()->rootContext;
}

void DeclarativeView::resizeEvent(QResizeEvent *event)
{
	Q_D(DeclarativeView);
	d->_q_updateView();
	return QGraphicsView::resizeEvent(event);
}

void DeclarativeView::setRootObject(QObject *object)
{
	Q_D(DeclarativeView);
	if (d->rootObject == object || !scene())
		return;
	if (QDeclarativeItem *declarativeItem = qobject_cast<QDeclarativeItem*>(object)) {
		scene()->addItem(declarativeItem);
		d->rootObject = declarativeItem;
		d->rootItem = declarativeItem;
		declarativeItem->setFlag(QGraphicsItem::ItemIsMovable, false);
	} else if (QGraphicsObject *graphicsObject = qobject_cast<QGraphicsObject *>(object)) {
		scene()->addItem(graphicsObject);
		d->rootObject = graphicsObject;
		if (graphicsObject->isWidget()) {
			d->graphicsWidgetRoot = static_cast<QGraphicsWidget*>(graphicsObject);
		} else {
			qWarning() << "QDeclarativeView::resizeMode is not honored for components of type QGraphicsObject";
		}
	} else if (object) {
		qWarning() << "QDeclarativeView only supports loading of root objects that derive from QGraphicsObject";
		if (QWidget* widget  = qobject_cast<QWidget *>(object)) {
			window()->setAttribute(Qt::WA_OpaquePaintEvent, false);
			window()->setAttribute(Qt::WA_NoSystemBackground, false);
			if (layout() && layout()->count()) {
				// Hide the QGraphicsView in GV mode.
				QLayoutItem *item = layout()->itemAt(0);
				if (item->widget())
					item->widget()->hide();
			}
			widget->setParent(this);
			if (isVisible()) {
				widget->setVisible(true);
			}
			resize(widget->size());
		}
	}

	if (d->rootObject) {
		d->initialSize = d->rootObjectSize();
		if (d->initialSize != size()) {
			if (!(parentWidget() && parentWidget()->layout())) {
				resize(d->initialSize);
			}
		}
		d->initResize();
	}
}

bool DeclarativeView::eventFilter(QObject *watched, QEvent *e)
{
	Q_D(DeclarativeView);
	if (watched == d->rootObject) {
		if (d->graphicsWidgetRoot) {
			if (e->type() == QEvent::GraphicsSceneResize) {
				d->_q_updateView();
			}
		}
	}
	return QGraphicsView::eventFilter(watched, e);
}

QObject *DeclarativeView::rootObject() const
{
	return d_func()->rootObject;
}

DeclarativeView::Status DeclarativeView::status() const
{
	Q_D(const DeclarativeView);
	if (!d->component)
		return DeclarativeView::Null;

	return DeclarativeView::Status(d->component->status());
}

DeclarativeView::ResizeMode DeclarativeView::resizeMode() const
{
	return d_func()->resizeMode;
}

void DeclarativeView::setResizeMode(DeclarativeView::ResizeMode mode)
{
	Q_D(DeclarativeView);
	if (d->resizeMode != mode) {
		d->resizeMode = mode;
		if (d->rootItem) {
			d->initResize();
		}
	}
}

} // namespace qutim_sdk_0_3

#include "moc_declarativeview.cpp"
