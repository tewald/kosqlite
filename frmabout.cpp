#include "frmabout.h"
#include "ui_frmabout.h"

FrmAbout::FrmAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmAbout)
{
    ui->setupUi(this);

    ui->textEdit->setReadOnly(true);
    ui->fSlika->setStyleSheet("background-image: url(:/icons/icons/kosqlite.png)");
    ui->fSlika->setFixedHeight(128);
    ui->fSlika->setFixedWidth(128);
    ui->lblName->setText(QApplication::applicationName());
    ui->lblVersion->setText(ui->lblVersion->text() + " " + QApplication::applicationVersion());
    ui->lblDevelopedby->setText(ui->lblDevelopedby->text() + " Karolj Kočmaroš" );
    ui->lblCopyright->setText(tr("Copyright © ") + QDate::currentDate().toString("yyyy") + " Karolj Kočmaroš");
}

FrmAbout::~FrmAbout()
{
    delete ui;
}

void FrmAbout::on_btnOK_clicked()
{
    this->close();
}
