#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0);

    ~Downloader();
    /**
     * @brief Downloader::downloadedDataFromUrl --čitanje podatka koji je preuzet sa neta
     * @return
     */
    QByteArray downloadedDataFromUrl() const;
    /**
     * @brief Downloader::checkUpdate --provera da li ima updajeta, i obrada greške ukoliko postoji
     */
    void checkUpdate();

signals:
    void downloaded(); //emituje se kada se preuzimanje završi
    void errorMessage(QString error); //emituje se kada nastane greška

private slots:
    /**
     * @brief Downloader::dataDownloaded preuzimanje podatka
     * @param reply
     */
    void dataDownloaded(QNetworkReply *reply);
    /**
     * @brief Downloader::networkError --obrada greške nastale na mreži
     * @param nError --nastala greška
     */
    void networkError(QNetworkReply::NetworkError nError);

private:
    QNetworkAccessManager networkAccessManager;
    /**
     * @brief downloadedData --preuzet podatak
     */
    QByteArray downloadedData;
    /**
     * @brief linkToData --link odakle se skida podatak
     */
    QUrl linkToData;
};

#endif // DOWNLOADER_H
