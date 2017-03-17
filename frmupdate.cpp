#include "frmupdate.h"
#include "ui_frmupdate.h"

FrmUpdate::FrmUpdate(bool isUpdate, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmUpdate), isUpdate(isUpdate)
{
    ui->setupUi(this);

    ui->fSlika->setStyleSheet("background-image: url(:/icons/icons/update.png)");
    ui->fSlika->setFixedHeight(128);
    ui->fSlika->setFixedWidth(128);
    this->setFixedSize(500,177);

    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("update");

    if (isUpdate == true)
    {
        ui->lblUpdate->setText(tr("New version is available. Do you want to download it?"));
        ui->chcShow->setText(tr("Don't show this message again."));
        ui->chcShow->setChecked(settingsConfig.value("showUpdateInfo", false).toBool() );
    }
    else
    {
        ui->lblUpdate->setText(tr("You have the most recent version."));
        ui->chcShow->setText(tr("Show notification if newer version is available."));
        ui->chcShow->setChecked(!settingsConfig.value("showUpdateInfo", true).toBool() );
        ui->btnDownload->setEnabled(false);
    }

    settingsConfig.endGroup();
}

FrmUpdate::~FrmUpdate()
{
    delete ui;
}

void FrmUpdate::on_btnClose_clicked()
{
    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    settingsConfig.beginGroup("update");

    if (isUpdate == true)
    {
        if (ui->chcShow->isChecked() == true)
            settingsConfig.setValue("showUpdateInfo", true);
        else
            settingsConfig.setValue("showUpdateInfo", false);
    }
    else
    {
        if (ui->chcShow->isChecked() == true)
            settingsConfig.setValue("showUpdateInfo", false);
        else
            settingsConfig.setValue("showUpdateInfo", true);
    }

    settingsConfig.endGroup();

    this->close();
}

void FrmUpdate::on_btnDownload_clicked()
{
    QDesktopServices::openUrl(QUrl("http://karoljkocmaros.blogspot.com/p/ko.html"));
    this->close();
}
