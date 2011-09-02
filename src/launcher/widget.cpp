#include "widget.h"
#include <QPainter>

Widget::Widget(QWidget *parent) :
	QGLWidget(parent)
{
	setAttribute(Qt::WA_AcceptTouchEvents);
}

void Widget::paintEvent(QPaintEvent *)
{
	QPainter painter;
	painter.begin(this);
	painter.drawImage(QRectF(67.0f, 0.0f, 720.0f, 480.0f), m->frame(), QRectF(0.0f, 0.0f, 240.0f, 160.0f));
	painter.end();
	QMetaObject::invokeMethod(m, "invalidateFrame", Qt::DirectConnection);
}

void Widget::setMachine(IMachine *m) {
	this->m = m;
}
