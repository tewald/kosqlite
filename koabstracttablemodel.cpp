#include "koabstracttablemodel.h"

koAbstractTableModel::koAbstractTableModel(QStringList heders, QList<QStringList> fields, QObject *parent) :
    QAbstractTableModel(parent)
{
    //učitavanje podataka u tabelu, tabela read-only i fajront

    this->heders = heders;

    rCount = 0;
    cCount = this->heders.count();


    for (int row = 0; row < fields.count(); row++, rCount++)
    {
        for (int column = 0; column < fields.at(row).count(); column++)
        {
            QModelIndex index = this->createIndex(row, column, this);

            if (fields.at(row).at(column).isEmpty() || fields.at(row).at(column) == "NULL")
            {
                writeTableData(index, Qt::DisplayRole, QVariant("NULL"));
                writeTableData(index, Qt::FontRole, QVariant(fields.at(row).at(column)));
            }
            else
                writeTableData(index, Qt::DisplayRole, QVariant(fields.at(row).at(column)));
        }        
    }
}

int koAbstractTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : rCount;
}

int koAbstractTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : cCount;
}

QVariant koAbstractTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return readTableData(index, role);
    }
    else if (role == Qt::FontRole)
        return readTableData(index, role);

    return QVariant();
}

QVariant koAbstractTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section < this->heders.count())
            return heders.at(section);
    }

    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return section + 1;

    return QVariant();
}

Qt::ItemFlags koAbstractTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

bool koAbstractTableModel::writeTableData(QModelIndex index, int role, QVariant value)
{
    QString ind = QString("%1-%2").arg(index.row()).arg(index.column());

    if (tableData.contains(ind))
    {
        QMap<int, QVariant> cell = tableData.value(ind);

        if (role == Qt::FontRole)
        {
            QFont font;
            font.setItalic(true);
            cell.insert(role, font);
        }
        else
            cell.insert(role, value);

        tableData.insert(ind, cell);

        cell.clear();
    }
    else
    {
        QMap<int, QVariant> cell;

        if (role == Qt::FontRole)
        {
            QFont font;
            font.setItalic(true);
            cell.insert(role, font);
        }
        else
            cell.insert(role, value);

        tableData.insert(ind, cell);

        cell.clear();
    }

    return true;
}

QVariant koAbstractTableModel::readTableData(const QModelIndex index, int role) const
{
    QString ind = QString("%1-%2").arg(index.row()).arg(index.column());

    if (tableData.contains(ind))
        return tableData.value(ind).value(role);

    return QVariant();
}
