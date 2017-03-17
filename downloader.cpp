#include "downloader.h"

Downloader::Downloader(QObject *parent) :
    QObject(parent)
{
    connect(&networkAccessManager,SIGNAL(finished(QNetworkReply*)), this, SLOT(dataDownloaded(QNetworkReply*)));
    linkToData = QUrl("https://master-dl.sourceforge.net/project/kosqlite/latestversion.txt");
}

Downloader::~Downloader()
{

}

QByteArray Downloader::downloadedDataFromUrl() const
{
    return downloadedData;
}

void Downloader::checkUpdate()
{
    QNetworkRequest request(linkToData);
    connect(networkAccessManager.get(request),SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(networkError(QNetworkReply::NetworkError)));
}

void Downloader::dataDownloaded(QNetworkReply *reply)
{
    downloadedData = reply->readAll();
    reply->deleteLater();
    emit downloaded();//podatak je preuzet, emituje se signal da je gotovo
}

void Downloader::networkError(QNetworkReply::NetworkError nError)
{
    switch(nError)
    {
    case QNetworkReply::ConnectionRefusedError:
        emit errorMessage(tr("The update server refused the connection."));
        break;
    case QNetworkReply::HostNotFoundError:
        emit errorMessage(tr("The update host was not found."));
        break;
    case QNetworkReply::ContentNotFoundError:
        emit errorMessage(tr("The update information was not found."));
        break;
    default:
        emit errorMessage(tr("Error %1. Please report the error number to developer.").arg(nError));
        break;
    }
}
