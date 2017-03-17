#ifndef KOSQLQUERYMODEL_H
#define KOSQLQUERYMODEL_H

#include <QSqlQueryModel>
#include <QFont>

class KoSqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit KoSqlQueryModel(QObject *parent = 0);

signals:

public slots:

private:
    QVariant data(const QModelIndex &idx, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

};

#endif // KOSQLQUERYMODEL_H
