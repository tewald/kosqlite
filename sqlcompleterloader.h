#ifndef SQLCOMPLETERLOADER_H
#define SQLCOMPLETERLOADER_H

#include <QtCore>
#include <QtWidgets>

class SqlCompleterLoader
{
public:
    SqlCompleterLoader();
    ~SqlCompleterLoader();

    QStandardItemModel *completerModel();

private:
    void loadKeywords();
    void loadFunctions();
    void loadPragmas();

    void addToModel(QString completion, QString toolTip, QIcon icon);

    QStandardItemModel *model;
    QIcon iKeyword;
    QIcon iFunction;
    QIcon iPragma;
};

#endif // SQLCOMPLETERLOADER_H
