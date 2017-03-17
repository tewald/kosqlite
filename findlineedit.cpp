/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#include "findlineedit.h"


FindLineEdit::FindLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    searchButton = new QToolButton(this);
    QPixmap pixmap(":/icons/icons/searchoptions.png");
    searchButton->setIcon(QIcon(pixmap));
    searchButton->setIconSize(pixmap.size());
    searchButton->setCursor(Qt::ArrowCursor);
    searchButton->setStyleSheet("QToolButton {padding: 0px; }");
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setStyleSheet(QString("QLineEdit { padding-left: %1px; } ").arg(searchButton->sizeHint().width() + frameWidth ));
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), searchButton->sizeHint().height() + frameWidth * 2 ),
                   qMax(msz.height(), searchButton->sizeHint().height() + frameWidth * 2 ));

    connect(searchButton,SIGNAL(clicked()),this,SLOT(searchButton_clicked()));
}

QToolButton *FindLineEdit::searchButtonContextMenu()
{
    return searchButton;
}

void FindLineEdit::searchButton_clicked()
{
    searchButton->showMenu();
}

void FindLineEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = searchButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    searchButton->move(rect().left() - frameWidth+1, (rect().bottom()+1 - sz.height())/2);
}
