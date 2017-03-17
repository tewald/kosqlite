/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef FINDLINEEDIT_H
#define FINDLINEEDIT_H

#include <QLineEdit>
#include <QToolButton>
#include <QStyle>

class FindLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    FindLineEdit(QWidget *parent = 0);
    QToolButton *searchButtonContextMenu();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void searchButton_clicked();

private:
    QToolButton *searchButton; //da mogu da mu postavim context menu, iz druge klase

};

#endif // FINDLIENEDIT_H
