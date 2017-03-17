#ifndef FRMUPDATE_H
#define FRMUPDATE_H

#include <QDialog>
#include <QtCore>
#include <QtGui>

namespace Ui {
class FrmUpdate;
}

class FrmUpdate : public QDialog
{
    Q_OBJECT

public:
    explicit FrmUpdate(bool isUpdate, QWidget *parent = 0);
    ~FrmUpdate();

private slots:
    /**
     * @brief FrmUpdate::on_btnClose_clicked --zatvaranje updejta
     */
    void on_btnClose_clicked();
    /**
     * @brief FrmUpdate::on_btnDownload_clicked --preusmeravanje na updajte
     */
    void on_btnDownload_clicked();

private:
    Ui::FrmUpdate *ui;
    /**
     * @brief isUpdate --Ako je u pitanje updajte
     */
    bool isUpdate;

};

#endif // FRMUPDATE_H
