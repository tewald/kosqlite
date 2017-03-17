#ifndef SQLTABWIDGET_H
#define SQLTABWIDGET_H

#include <QTabWidget>
#include <QtCore>
#include <QtWidgets>

class SqlTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit SqlTabWidget(QWidget *parent = 0);

signals:
    void middleClicked(int tabIndex);

protected:
    void mousePressEvent(QMouseEvent *event);

public slots:

};

#endif // SQLTABWIDGET_H
