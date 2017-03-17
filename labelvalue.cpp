#include "labelvalue.h"

LabelValue::LabelValue(QWidget *parent) :
    QLabel(parent)
{
}

void LabelValue::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit mouseDoubleClicked();

    QLabel::mouseDoubleClickEvent(event);
}
