#include "frmeditdata.h"
#include "ui_frmeditdata.h"

FrmEditData::FrmEditData(QString connection, QString table, QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmEditData)
{
    ui->setupUi(this);
    this->connection = connection;
    this->table = table;
    this->path = path;

    this->setWindowTitle(this->windowTitle() + " \"" + table + "\"");

    QSqlDatabase db = QSqlDatabase::database(connection);

    KoSqlTableModel *model = new KoSqlTableModel(this, db);
    model->setObjectName("model");
    model->setTable(table);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    ui->tblEditData->setModel(model);

    inicijalizacijaToolBara();
}

FrmEditData::~FrmEditData()
{
    delete ui;
}

void FrmEditData::closeEvent(QCloseEvent *event)
{
    emit finished(true);
    emit finished();

    QDialog::closeEvent(event);
}

void FrmEditData::on_btnClose_clicked()
{
    this->close();
}

void FrmEditData::actionAddRow_triggered()
{
    KoSqlTableModel *model = this->findChild<KoSqlTableModel*>("model");
    model->insertRow(model->rowCount());
}

void FrmEditData::actionDeleteRow_triggered()
{
    QModelIndexList selected = ui->tblEditData->selectionModel()->selectedIndexes();

    qStableSort(selected.begin(), selected.end(), qLess<QModelIndex>());

    QList<int> rows;

    foreach (QModelIndex ind, selected)
    {
        if (!rows.contains(ind.row()))
            rows.append(ind.row());
    }

    KoSqlTableModel *model = this->findChild<KoSqlTableModel*>("model");

    for (int i = model->rowCount() - 1; i >= 0; i--)
    {
        if (rows.contains(i))
            model->removeRow(i);
    }
}

void FrmEditData::actionInsertBlob_triggered()
{
    if (ui->tblEditData->currentIndex().isValid())
    {
        QString openFile = QFileDialog::getOpenFileName(this,tr("Insert BLOB"), path, tr("All Files(*)"));

        if (!openFile.isEmpty())
        {
            QFile file(openFile);

            if (!file.open(QIODevice::ReadOnly))
            {
                QMessageBox::warning(this,tr("Message"),tr("File can not be loaded."), QMessageBox::Ok);
                return;
            }

            ui->tblEditData->model()->setData(ui->tblEditData->currentIndex(), file.readAll(), Qt::EditRole);

            file.close();

            emit setPath(QFileInfo(openFile).path());
        }
    }
}

void FrmEditData::actionDetailsBlob_triggered()
{
    if (ui->tblEditData->currentIndex().isValid())
    {
        if (ui->tblEditData->model()->data(ui->tblEditData->currentIndex(), Qt::EditRole).type() == QVariant::ByteArray)
        {
            FrmBlobDetails *frm = new FrmBlobDetails(ui->tblEditData->model()->data(ui->tblEditData->currentIndex(), Qt::EditRole).toByteArray(), path, this);
            connect(frm,SIGNAL(finished()),frm,SLOT(deleteLater()));
            connect(frm,SIGNAL(setPath(QString)),this,SLOT(setPathBLOB(QString)));
            frm->show();
        }
    }
}

void FrmEditData::actionClear_triggered()
{
    QModelIndexList selected = ui->tblEditData->selectionModel()->selectedIndexes();

    foreach (QModelIndex ind, selected)
        ui->tblEditData->model()->setData(ind, "", Qt::EditRole);
}

void FrmEditData::actionNull_triggered()
{
    QModelIndexList selected = ui->tblEditData->selectionModel()->selectedIndexes();

    foreach (QModelIndex ind, selected)
        ui->tblEditData->model()->setData(ind, "NULL", Qt::EditRole);
}

void FrmEditData::actionSubmit_triggered()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    KoSqlTableModel *model = this->findChild<KoSqlTableModel*>("model");

    if (!model->submitAll())
    {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        QMessageBox::warning(this,tr("Message"), model->lastError().databaseText(), QMessageBox::Ok);
    }

    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void FrmEditData::actionRevert_triggered()
{
    KoSqlTableModel *model = this->findChild<KoSqlTableModel*>("model");
    model->revertAll();
}

void FrmEditData::setPathBLOB(QString path)
{
    emit setPath(path);
}

void FrmEditData::inicijalizacijaToolBara()
{
    QToolBar *tb = new QToolBar(this);
    tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui->verticalLayout->insertWidget(0,tb);

    actionAddRow = new QAction(QIcon(":/icons/icons/insertrow.png"), tr("Add Row"),this);
    actionDeleteRow = new QAction(QIcon(":/icons/icons/deleterow.png"), tr("Delete Row"),this);
    actionInsertBlob = new QAction(QIcon(":/icons/icons/add.png"), tr("Insert"),this);
    actionDetailsBlob = new QAction(QIcon(":/icons/icons/preview.png"), tr("Details"),this);
    actionClear = new QAction(QIcon(":/icons/icons/clear.png"), tr("Clear"),this);
    actionNull = new QAction(QIcon(":/icons/icons/null.png"), tr("NULL"),this);
    actionSubmit = new QAction(QIcon(":/icons/icons/submit.png"), tr("Submit"), this);
    actionRevert = new QAction(QIcon(":/icons/icons/revert.png"), tr("Revert"), this);

    actionAddRow->setToolTip(tr("Add row at end"));
    actionDeleteRow->setToolTip(tr("Delete selected rows"));
    actionInsertBlob->setToolTip(tr("Insert BLOB in selected cell"));
    actionDetailsBlob->setToolTip(tr("Details BLOB in selected cell"));
    actionClear->setToolTip(tr("Clear values for selected cells. Set empty cell \"\""));
    actionNull->setToolTip(tr("Set NULL value for selected cells"));
    actionSubmit->setToolTip(tr("Submit all pending changes"));
    actionRevert->setToolTip(tr("Revert all pending changes"));

    connect(actionAddRow,SIGNAL(triggered()),this,SLOT(actionAddRow_triggered()));
    connect(actionDeleteRow,SIGNAL(triggered()),this,SLOT(actionDeleteRow_triggered()));
    connect(actionInsertBlob,SIGNAL(triggered()),this,SLOT(actionInsertBlob_triggered()));
    connect(actionDetailsBlob,SIGNAL(triggered()),this,SLOT(actionDetailsBlob_triggered()));
    connect(actionClear,SIGNAL(triggered()),this,SLOT(actionClear_triggered()));
    connect(actionNull,SIGNAL(triggered()),this,SLOT(actionNull_triggered()));
    connect(actionSubmit,SIGNAL(triggered()),this,SLOT(actionSubmit_triggered()));
    connect(actionRevert,SIGNAL(triggered()),this,SLOT(actionRevert_triggered()));

    tb->addAction(actionAddRow);
    tb->addAction(actionDeleteRow);
    tb->addSeparator();
    tb->addAction(actionInsertBlob);
    tb->addAction(actionDetailsBlob);
    tb->addSeparator();
    tb->addAction(actionClear);
    tb->addAction(actionNull);
    tb->addSeparator();
    tb->addAction(actionSubmit);
    tb->addAction(actionRevert);
}
