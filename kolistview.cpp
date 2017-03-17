#include "kolistview.h"

KoListView::KoListView(QWidget *parent) :
    QListView(parent)
{
    this->setAlternatingRowColors(true);
}

void KoListView::mouseReleaseEvent(QMouseEvent *event)
{
    //prikazivanje tooltipa ključne reči funkcije
    this->currentIndex();

    QMap<int, QVariant> item = this->model()->itemData(this->currentIndex());

    QToolTip::showText(event->globalPos(), item.value(Qt::ToolTipRole).toString());

    event->accept();
}

void KoListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    //insertovanje completiona
    this->currentIndex();

    QMap<int, QVariant> item = this->model()->itemData(this->currentIndex());

    emit activated(item.value(Qt::DisplayRole).toString());

    event->accept();

    this->hide();
}

//void KoListView::keyPressEvent(QKeyEvent *event)
//{
   /* //! OVO SAD NE RADI
    if (event->isAccepted())
    {
        switch(event->key())
        {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            this->currentIndex();

            QMap<int, QVariant> item = this->model()->itemData(this->currentIndex());

            emit activated(item.value(Qt::DisplayRole).toString());

            qDebug() << "h";

            this->hide();
        }
            break;

        default:
            QListView::keyPressEvent(event);
            break;
        }
    }

    event->accept();*/

    //qDebug() << event->key();
//}

