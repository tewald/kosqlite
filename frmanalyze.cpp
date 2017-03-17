#include "frmanalyze.h"
#include "ui_frmanalyze.h"

FrmAnalyze::FrmAnalyze(QString connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmAnalyze)
{
    ui->setupUi(this);
    this->connection = connection;

    ui->tblTabele->setColumnCount(1);
    ui->tblTabele->setHorizontalHeaderLabels(QStringList() << tr("Tables"));
    ui->tblTabele->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblTabele->verticalHeader()->setDefaultSectionSize(20);

    QSqlDatabase db = QSqlDatabase::database(connection);

    if (db.isOpen())
    {
        QSqlQuery upit(db);

        upit.prepare("select name from sqlite_master where type = 'table' order by name;");
        upit.exec();

        while (upit.next())
        {
            int row = ui->tblTabele->rowCount();
            ui->tblTabele->insertRow(row);
            ui->tblTabele->setItem(row, 0, new QTableWidgetItem(upit.value(0).toString()));
            ui->tblTabele->item(row, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }       
    }
}

FrmAnalyze::~FrmAnalyze()
{
    delete ui;
}

void FrmAnalyze::closeEvent(QCloseEvent *event)
{
    emit finished(true);

    QDialog::closeEvent(event);
}

void FrmAnalyze::on_btnAnalyze_clicked()
{
    QModelIndexList selektovano = ui->tblTabele->selectionModel()->selectedRows();

    qStableSort(selektovano.begin(), selektovano.end(), qLess<QModelIndex>());

    QStringList upitList;

    foreach(QModelIndex ind, selektovano)
        upitList << QString("analyze %1;").arg(ui->tblTabele->item(ind.row(), 0)->text());

    emit executeSqlQuery(upitList, connection);
}

void FrmAnalyze::on_btnAnalyzeAll_clicked()
{
    emit executeSqlQuery(QStringList() << "analyze;", connection);
}

void FrmAnalyze::on_btnClose_clicked()
{
    this->close();
}
