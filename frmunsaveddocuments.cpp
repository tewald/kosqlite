#include "frmunsaveddocuments.h"
#include "ui_frmunsaveddocuments.h"

FrmUnsavedDocuments::FrmUnsavedDocuments(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmUnsavedDocuments)
{
    ui->setupUi(this);

    ui->listDocuments->setFocusPolicy(Qt::NoFocus);

    connect(ui->btnYes,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->btnNo,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(close()));
}

FrmUnsavedDocuments::~FrmUnsavedDocuments()
{
    delete ui;
}

void FrmUnsavedDocuments::setUnsavedDocuments(QStringList documents)
{
    for (int i = 0; i < documents.count(); i++)
    {
        ui->listDocuments->insertItem(i,documents.at(i));
        ui->listDocuments->item(i)->setIcon(QIcon(":/icons/icons/document.png"));
        ui->listDocuments->item(i)->setFlags(Qt::ItemIsEnabled);
    }
}

void FrmUnsavedDocuments::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    this->setResult(QDialog::Accepted);
}
