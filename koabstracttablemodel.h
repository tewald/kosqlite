#ifndef KOABSTRACTTABLEMODEL_H
#define KOABSTRACTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QFile>
#include <QDebug>
#include <QFont>

class koAbstractTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit koAbstractTableModel(QStringList heders, QList<QStringList> fields, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;

    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

signals:

public slots:

protected:

private:
    QMap<QString, QMap<int, QVariant> > tableData;

    bool writeTableData(QModelIndex index, int role, QVariant value);
    QVariant readTableData(const QModelIndex index, int role) const;

    QStringList heders;

    int rCount;
    int cCount;

};

#endif // KOABSTRACTTABLEMODEL_H
