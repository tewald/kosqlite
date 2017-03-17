#include "sqltabwidget.h"

SqlTabWidget::SqlTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    this->setTabsClosable(true);
    this->setMovable(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
}

void SqlTabWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton)
    {
        int tabIndex = this->tabBar()->tabAt(event->pos());
        if (tabIndex != -1 )
        {
            emit middleClicked(tabIndex);
            event->accept();
        }
    }

    QTabWidget::mousePressEvent(event);
}
