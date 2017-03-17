#ifndef KOSQLTABLEMODEL_H
#define KOSQLTABLEMODEL_H

#include <QSqlTableModel>
#include <QtSql>

class KoSqlTableModel : public QSqlTableModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSqlTableModel)

public:
    explicit KoSqlTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~KoSqlTableModel();

signals:
    void sqlError(QString error);

private:
    QVariant data(const QModelIndex &idx, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

private slots:


};

#endif // KOSQLTABLEMODEL_H
