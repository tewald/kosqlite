#ifndef KOLISTVIEW_H
#define KOLISTVIEW_H

#include <QListView>
#include <QtWidgets>

class KoListView : public QListView
{
    Q_OBJECT
public:
    explicit KoListView(QWidget *parent = 0);

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    //void keyPressEvent(QKeyEvent *event);

signals:
    void activated(QString current);

public slots:

};

#endif // KOLISTVIEW_H
