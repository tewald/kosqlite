#include "sqlcompleterloader.h"

SqlCompleterLoader::SqlCompleterLoader()
{
    model = new QStandardItemModel();
    iKeyword = QIcon(":/icons/icons/keyword.png");
    iFunction = QIcon(":/icons/icons/function.png");
    iPragma = QIcon(":/icons/icons/pragmas.png");

    loadKeywords();
    loadFunctions();
    loadPragmas();
}

SqlCompleterLoader::~SqlCompleterLoader()
{
    delete model;
}

QStandardItemModel *SqlCompleterLoader::completerModel()
{
    model->sort(0, Qt::AscendingOrder);
    return model;
}

void SqlCompleterLoader::loadKeywords()
{
    QFile keywordFile(":/api/api/keywords.api");
    if (keywordFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&keywordFile);
        while ( !in.atEnd() )
        {
            QStringList lista = in.readLine().split(";;;");
            addToModel(lista.first(), lista.last(), iKeyword);
        }
        keywordFile.close();
    }
}

void SqlCompleterLoader::loadFunctions()
{
    QFile functionFile(":/api/api/functions.api");
    if (functionFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&functionFile);
        while ( !in.atEnd() )
        {
            QStringList lista = in.readLine().split(";;;");
            addToModel(lista.first(),lista.last(), iFunction);
        }
        functionFile.close();
    }
}

void SqlCompleterLoader::loadPragmas()
{
    QFile pragmaFile(":/api/api/pragmas.api");
    if (pragmaFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&pragmaFile);
        while ( !in.atEnd() )
        {
            QStringList lista = in.readLine().split(";;;");
            addToModel(lista.first(),lista.last(), iPragma);
        }
        pragmaFile.close();
    }
}

void SqlCompleterLoader::addToModel(QString completion, QString toolTip, QIcon icon)
{
    QStandardItem *item = new QStandardItem();
    item->setText(completion);
    //<p></p> su tu da bi se tekst prebacio u RichText i wrapovao tooltip
    item->setToolTip("<p>" + toolTip + "</p>");
    item->setIcon(icon);

    model->appendRow(item);
}
