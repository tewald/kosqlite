#ifndef LABELVALUE_H
#define LABELVALUE_H

#include <QLabel>
#include <QMouseEvent>

class LabelValue : public QLabel
{
    Q_OBJECT
public:
    explicit LabelValue(QWidget *parent = 0);

signals:
    void mouseDoubleClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:

};

#endif // LABELVALUE_H
