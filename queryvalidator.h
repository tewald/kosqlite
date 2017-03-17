#ifndef QUERYVALIDATOR_H
#define QUERYVALIDATOR_H

#include <QtCore>


class QueryValidator
{
public:
    QueryValidator();

    QString removeUnneededContent(QString query);

    QStringList parseToQuerys(QString query, bool explain);

private:
};

#endif // QUERYVALIDATOR_H
