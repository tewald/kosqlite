#ifndef QUERYEXECUTIONOBJECT_H
#define QUERYEXECUTIONOBJECT_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtSql>

class QueryExecutionObject : public QObject
{
    Q_OBJECT
public:
    explicit QueryExecutionObject(QObject *parent = 0);
    /**
     * @brief setQuery --postavka lista upita koje trebaju da se izvrše
     * @param listaUpita --lista upita
     */
    void setQuery(QStringList upitList);

    void setConnection(QString connection);

signals:
    /**
     * @brief executeFinished --izvršavanje upita je gotovo
     */
    void finished();

    void enable(bool enable);

    /**
     * @brief queryOutput --postavka poruke o izvršenom upitu, bilo da je uspešno ili neuspešno izvršen
     * @param vreme --vreme pozivanja upita
     * @param elapsedTime --koliko je upit trajao da se izvrši
     * @param lastQuery --upit koji je izvršen
     * @param databaseText --poruka baze, (error, affected rows, itd.)
     */
    void queryOutput(QString vreme, QString elapsedTime, QString lastQuery, QString databaseText);

    /**
     * @brief queryResult --rezultat upit
     * @param result
     */
    void queryResult(QSqlQuery result);

public slots:
    /**
     * @brief executeQuery --izvrši upit
     */
    void executeQuery();
    /**
     * @brief executeStopRequest --zahtev za zaustavljanje izvršenja upita
     */
    void executeStopRequest();


private:
    /**
     * @brief stop --da li je zahteve za zaustavljanje izvršenja upita zatražen
     */
    bool stop;
    /**
     * @brief listaUpita --lista upita koji čekaju na izvršenje
     */
    QStringList upitList;

    QString connection;

};

#endif // QUERYEXECUTIONOBJECT_H
