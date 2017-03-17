#include "frmblobdetails.h"
#include "ui_frmblobdetails.h"

FrmBlobDetails::FrmBlobDetails(QByteArray blob, QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmBlobDetails)
{
    ui->setupUi(this);
    this->blob = blob;
    this->path = path;

    QString size;

    //mislim da TB neće niko stavljati u bazu, bilo bi već jako bolesno
    if (blob.size() > 1024 * 1024 * 1024)
        size = QString(" %1 GB").arg(QString::number((double)blob.size() / 1024 / 1024 / 1024, 'd', 2));
    else if (blob.size() > 1024 * 1024)
        size = QString(" %1 MB").arg(QString::number((double)blob.size() / 1024 / 1024, 'd', 2));
    else if (blob.size() > 1024)
        size = QString(" %1 KB").arg(QString::number((double)blob.size() / 1024, 'd', 2));
    else
        size = QString(" %1 B").arg(QString::number((double)blob.size(), 'd', 2));

    ui->lblSize->setText(ui->lblSize->text() + size);

    ui->pteText->setPlainText(blob);

    QPixmap pixmap;
    pixmap.loadFromData(blob);

    if (!pixmap.isNull())
        ui->lblPreview->setPixmap(pixmap.scaled(128,128, Qt::KeepAspectRatio));
    else
        ui->lblPreview->setText(tr("No Preview"));
}

FrmBlobDetails::~FrmBlobDetails()
{
    delete ui;
}

void FrmBlobDetails::closeEvent(QCloseEvent *event)
{
    emit finished();

    QDialog::closeEvent(event);
}

void FrmBlobDetails::on_btnSave_clicked()
{
    QString file = QFileDialog::getSaveFileName(this,tr("Save BLOB"),path, tr("All Files(*)"));

    if (!file.isEmpty())
    {
        QFile out(file);
        if (!out.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this,tr("Message"), tr("File %1 can not be written.").arg(file), QMessageBox::Ok);
            return;
        }

        out.write(this->blob);

        out.close();

        emit setPath(QFileInfo(file).path());
    }
}

void FrmBlobDetails::on_btnClose_clicked()
{
    this->close();
}
