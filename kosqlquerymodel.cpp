#include "kosqlquerymodel.h"

KoSqlQueryModel::KoSqlQueryModel(QObject *parent) :
    QSqlQueryModel(parent)
{

}

QVariant KoSqlQueryModel::data(const QModelIndex &idx, int role) const
{
    if (QSqlQueryModel::data(idx, Qt::DisplayRole).isNull())
    {
        if (role == Qt::DisplayRole)
            return QVariant("NULL");
        else if (role == Qt::FontRole)
        {
            QFont font;
            font.setItalic(true);
            return QVariant::fromValue(font);
        }

        return QSqlQueryModel::data(idx, role);
    }
    else
    {
        switch (QSqlQueryModel::data(idx, Qt::DisplayRole).type())
        {
        case QMetaType::QByteArray:
        {
            if (role == Qt::DisplayRole)
                return QVariant("BLOB");
            else if (role == Qt::EditRole)
                return QSqlQueryModel::data(idx, role);
            if (role == Qt::FontRole)
            {
                QFont font;
                font.setItalic(true);
                return QVariant::fromValue(font);
            }

            return QSqlQueryModel::data(idx, role);
        }
            break;

        default:
            return QSqlQueryModel::data(idx, role);
            break;
        }
    }

    return QVariant();
}

bool KoSqlQueryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (value != QSqlQueryModel::data(index, Qt::EditRole))
    {
        if (role == Qt::EditRole)
        {
            switch (value.type())
            {
            case QMetaType::QByteArray:
                return QSqlQueryModel::setData(index, value, role);
                break;
            default:
                if (value == "NULL")
                    return QSqlQueryModel::setData(index, QVariant(), role);

                return QSqlQueryModel::setData(index, value, role);
                break;
            }
        }
    }

    return false;
}
