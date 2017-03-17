#include "frmattachdatabase.h"
#include "ui_frmattachdatabase.h"

FrmAttachDatabase::FrmAttachDatabase(QString path, QString connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmAttachDatabase)
{
    ui->setupUi(this);
    this->path = path;
    this->connection = connection;
}

FrmAttachDatabase::~FrmAttachDatabase()
{
    delete ui;
}

void FrmAttachDatabase::closeEvent(QCloseEvent *event)
{
    emit finished(true);

    QDialog::closeEvent(event);
}

void FrmAttachDatabase::on_tbtnBrowse_clicked()
{
    QString database = QFileDialog::getOpenFileName(this, tr("Attach Database"), path, tr("All Files(*)"));

    if (!database.isEmpty())
    {
        ui->txtPath->setText(database);

        ui->txtAlias->setText(QFileInfo(database).completeBaseName());

        emit setPath(QFileInfo(database).path());
    }
}

void FrmAttachDatabase::on_btnOK_clicked()
{
    if (ui->txtPath->text().isEmpty())
        return;

    if (ui->txtAlias->text().isEmpty())
        return;

    emit executeSqlQuery(QStringList() << QString("attach database \"%1\" as %2").arg(ui->txtPath->text()).arg(ui->txtAlias->text()), connection);
}

void FrmAttachDatabase::on_btnCancel_clicked()
{
    this->close();
}
