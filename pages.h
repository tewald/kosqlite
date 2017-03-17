#ifndef PAGES_H
#define PAGES_H

#include <QWidget>
#include <QtWidgets>

class LanguagePage : public QWidget
{
    Q_OBJECT

public:
    LanguagePage(QWidget *parent = 0);
};

class QueryEditorPage : public QWidget
{
    Q_OBJECT

public:
    QueryEditorPage(QWidget *parent = 0);
};

class EnviromentPage : public QWidget
{
    Q_OBJECT

public:
    EnviromentPage(QWidget *parent = 0);
};

#endif // PAGES_H
