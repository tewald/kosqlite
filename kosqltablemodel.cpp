#include "kosqltablemodel.h"

KoSqlTableModel::KoSqlTableModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
{

}

KoSqlTableModel::~KoSqlTableModel()
{

}

QVariant KoSqlTableModel::data(const QModelIndex &idx, int role) const
{
    if (QSqlTableModel::data(idx, Qt::DisplayRole).isNull())
    {
        if (role == Qt::DisplayRole)
            return QVariant("NULL");
        else if (role == Qt::FontRole)
        {
            QFont font;
            font.setItalic(true);
            return QVariant::fromValue(font);
        }

        return QSqlTableModel::data(idx, role).toString();
    }
    else
    {
        switch (QSqlTableModel::data(idx, Qt::DisplayRole).type())
        {
        case QMetaType::QByteArray:
        {
            if (role == Qt::DisplayRole)
                return QVariant("BLOB");
            else if (role == Qt::EditRole)
                return QSqlTableModel::data(idx, role);
            if (role == Qt::FontRole)
            {
                QFont font;
                font.setItalic(true);
                return QVariant::fromValue(font);
            }

            return QSqlTableModel::data(idx, role).toString();
        }
            break;

        default:
            return QSqlTableModel::data(idx, role).toString();
            break;
        }
    }

    return QVariant();
}

bool KoSqlTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (value != QSqlTableModel::data(index, Qt::EditRole))
    {
        if (role == Qt::EditRole)
        {
            switch (value.type())
            {
            case QMetaType::QByteArray:
                return QSqlTableModel::setData(index, value, role);
                break;
            default:
                if (value == "NULL")
                    return QSqlTableModel::setData(index, QVariant(), role);

                return QSqlTableModel::setData(index, value, role);
                break;
            }
        }
    }

    return false;
}
