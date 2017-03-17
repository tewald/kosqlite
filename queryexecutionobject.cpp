#include "queryexecutionobject.h"

QueryExecutionObject::QueryExecutionObject(QObject *parent) :
    QObject(parent)
{
    stop = false;
}

void QueryExecutionObject::setQuery(QStringList upitList)
{
    this->upitList = upitList;
}

void QueryExecutionObject::setConnection(QString connection)
{
    this->connection = connection;
}

void QueryExecutionObject::executeQuery()
{
    QSqlDatabase db = QSqlDatabase::database(connection);

    foreach (QString strUpit, this->upitList)
    {
        if (stop == false)
        {
            if (db.isOpen())
            {
                QElapsedTimer timer;
                QSqlQuery upit(db);
                QString vreme = QTime::currentTime().toString("hh:mm:ss");

                timer.start();
                upit.exec(strUpit);

                double elapsedTimer = (double)timer.elapsed() / 1000.0; //konverzija vremena iz milisekundi u sekunde. vreme koje je prošlo za izvršenje upita

                if (upit.isActive())
                {
                    emit queryResult(upit);

                    emit queryOutput(vreme, QString::number(elapsedTimer, 'f', 3), upit.lastQuery(), "Query OK");
                }
                else//upit nije izvršen, došlo je do greške
                {
                    emit queryOutput(vreme, QString::number(elapsedTimer, 'f', 3), upit.lastQuery(), upit.lastError().text());
                    //ukoliko jedan upit nije izvršen ostali se ne izvršavaju i tu izvršavanje puca
                    break;
                }
            }
        }
    }
    emit finished();
    emit enable(true);
}

void QueryExecutionObject::executeStopRequest()
{
    stop = true;
}
