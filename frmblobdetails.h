#ifndef FRMBLOBDETAILS_H
#define FRMBLOBDETAILS_H

#include <QDialog>
#include <QtWidgets>

namespace Ui {
class FrmBlobDetails;
}

class FrmBlobDetails : public QDialog
{
    Q_OBJECT

public:
    explicit FrmBlobDetails(QByteArray blob, QString path, QWidget *parent = 0);
    ~FrmBlobDetails();

signals:
    void finished();
    void setPath(QString path);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnSave_clicked();

    void on_btnClose_clicked();

private:
    Ui::FrmBlobDetails *ui;
    QByteArray blob;
    QString path;
};

#endif // FRMBLOBDETAILS_H
