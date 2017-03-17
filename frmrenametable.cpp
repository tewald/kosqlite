#include "frmrenametable.h"
#include "ui_frmrenametable.h"

FrmRenameTable::FrmRenameTable(QString currentName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmRenameTable)
{
    ui->setupUi(this);
    this->currentName = currentName;

    ui->txtNewName->setText(currentName);
    ui->txtNewName->setFocus();
    ui->txtNewName->selectAll();
}

FrmRenameTable::~FrmRenameTable()
{
    delete ui;
}

QString FrmRenameTable::tableName()
{
    return ui->txtNewName->text();
}

void FrmRenameTable::on_btnRename_clicked()
{
    this->close();
    if (currentName != ui->txtNewName->text()) //korisnik je uneo novo ime
        this->setResult(QDialog::Accepted);
    else
        this->setResult(QDialog::Rejected); //ime je isto, preimenovanje nije potrebno
}

void FrmRenameTable::on_btnCancel_clicked()
{
    this->close();
    this->setResult(QDialog::Rejected);
}
