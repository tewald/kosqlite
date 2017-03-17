#include "frmimportdata.h"
#include "ui_frmimportdata.h"

FrmImportData::FrmImportData(QString connection, QString table, QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmImportData)
{
    ui->setupUi(this);

    this->connection = connection;
    this->path = path;
    this->table = table;

    ui->rbTab->blockSignals(true);
    ui->rbTab->setChecked(true);
    ui->rbTab->blockSignals(false);

    QSqlDatabase db = QSqlDatabase::database(connection);

    if (db.isOpen())
    {
        QSqlQuery upit(db);
        upit.prepare(QString("select * from %1 limit 0").arg(table));
        upit.exec();

        QSqlRecord record = upit.record();

        for (int i = 0; i < record.count(); i++)
            heders << record.fieldName(i);
    }

    ui->tblPreview->resizeColumnsToContents();
    ui->tblPreview->verticalHeader()->setDefaultSectionSize(20);

    //inicijalno učitavanje hedera na tabelu
    readCSVfile("","\t");
}

FrmImportData::~FrmImportData()
{
    delete ui;
}

void FrmImportData::closeEvent(QCloseEvent *event)
{
    emit finished(true);
    emit finished();
    QDialog::closeEvent(event);
}

void FrmImportData::on_btnBrowse_clicked()
{
    QString csvFile = QFileDialog::getOpenFileName(this,tr("Open CSV File"), path, tr("CSV files (*.csv)"));

    if (!csvFile.isEmpty())
    {
        QFile file(csvFile);

        if (!file.open(QIODevice::ReadOnly))
            QMessageBox::warning(this,tr("Message"), tr("File can not be opened."), QMessageBox::Ok);

        QString data = file.readAll();
        file.close();

        this->data = data;

        ui->txtFile->setText(csvFile);

        QString delimiter;

        if (ui->rbTab->isChecked())
            delimiter = "\t";
        else if (ui->rbComma->isChecked())
            delimiter = ",";
        else if (ui->rbSemicolon->isChecked())
            delimiter = ";";
        else if (ui->rbSpace->isChecked())
            delimiter = " ";
        else if (ui->rbOther->isChecked())
            delimiter = ui->txtOther->text();

        readCSVfile(data, delimiter);

        emit setPath(QFileInfo(csvFile).path());
    }
}

void FrmImportData::on_btnImport_clicked()
{
    QStringList upitList;

    for (int i = 0; i < ui->tblPreview->model()->rowCount(); i++)
    {
        QString upit = QString("insert into %1 (%2) values(").arg(table).arg(heders.join(", "));

        for (int j = 0; j < ui->tblPreview->model()->columnCount(); j++)
        {
            if (ui->tblPreview->model()->data(ui->tblPreview->model()->index(i,j)).toString() == "NULL")
                upit.append("NULL,");
            else
            {
                bool ok;
                bool konvertovano = false;

                ui->tblPreview->model()->data(ui->tblPreview->model()->index(i, j)).toInt(&ok);

                //ako je konverzija uspešna dodaj u values
                if (ok)
                {
                    upit.append(ui->tblPreview->model()->data(ui->tblPreview->model()->index(i, j)).toString() + ",");
                    konvertovano = true;
                }

                //ako je konvertovanje neuspešno onda probaj da konvertuješ u double
                if (konvertovano == false)
                {
                    ui->tblPreview->model()->data(ui->tblPreview->model()->index(i, j)).toDouble(&ok);

                    if (ok)//konvertovanje uspelo
                    {
                        upit.append(ui->tblPreview->model()->data(ui->tblPreview->model()->index(i, j)).toString() + ",");
                        konvertovano = true;
                    }
                }

                //ako konvetovanje nije uspelo ni za INT ni za DOUBLE onda je string
                if (konvertovano == false)
                {
                    upit.append("\"" + ui->tblPreview->model()->data(ui->tblPreview->model()->index(i, j)).toString() + "\""  + ",");
                }
            }
        }

        //zadnji zarez je višak
        upit = upit.remove(upit.lastIndexOf(","), upit.count());
        upit.append(");");

        //string se dodaje u upit koji se priprema
        upitList << upit;
    }

    emit executeSqlQuery(upitList, connection);
}

void FrmImportData::on_btnClose_clicked()
{
    this->close();
}

void FrmImportData::on_rbTab_toggled(bool checked)
{
    if (checked)
        readCSVfile(data, "\t");
}

void FrmImportData::on_rbComma_toggled(bool checked)
{
    if (checked)
        readCSVfile(data, ",");
}

void FrmImportData::on_rbSemicolon_toggled(bool checked)
{
    if (checked)
        readCSVfile(data, ";");
}

void FrmImportData::on_rbSpace_toggled(bool checked)
{
    if (checked)
        readCSVfile(data, " ");
}

void FrmImportData::on_rbOther_toggled(bool checked)
{
    if (checked)
        if (!ui->txtOther->text().isEmpty())
            readCSVfile(data, ui->txtOther->text());
}

void FrmImportData::readCSVfile(QString data, QString delimiter)
{
    koAbstractTableModel *m = this->findChild<koAbstractTableModel*>("model");

    //ukoliko već postoji model na tabelu potrebno ga je obrisati iz memorije
    if (m != NULL)
    {
        delete m;
    }

    QList<QStringList> table;

    QStringList split = data.split("\n");
    split.removeLast();

    for (int i = 0; i < split.count(); i++)
        table << split.at(i).split(delimiter);

    koAbstractTableModel *model = new koAbstractTableModel(heders, table, this);
    model->setObjectName("model");
    ui->tblPreview->setModel(model);;

    ui->tblPreview->resizeColumnsToContents();
}

void FrmImportData::on_txtOther_textChanged(const QString &arg1)
{
    if (ui->rbOther->isChecked())
        if (!ui->txtOther->text().isEmpty())
            readCSVfile(data, arg1);
}
