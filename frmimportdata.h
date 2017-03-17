#ifndef FRMIMPORTDATA_H
#define FRMIMPORTDATA_H

#include <QDialog>
#include <QtSql>
#include <QtWidgets>

#include "frmblobdetails.h"
#include "koabstracttablemodel.h"

namespace Ui {
class FrmImportData;
}

class FrmImportData : public QDialog
{
    Q_OBJECT

public:
    explicit FrmImportData(QString connection, QString path, QString table, QWidget *parent = 0);
    ~FrmImportData();

signals:
    void executeSqlQuery(QStringList upitList, QString connection);
    void finished(bool isFinished);
    void finished();
    void setPath(QString path);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnBrowse_clicked();

    void on_btnImport_clicked();

    void on_btnClose_clicked();

    void on_rbTab_toggled(bool checked);

    void on_rbComma_toggled(bool checked);

    void on_rbSemicolon_toggled(bool checked);

    void on_rbSpace_toggled(bool checked);

    void on_rbOther_toggled(bool checked);

    void on_txtOther_textChanged(const QString &arg1);

private:
    Ui::FrmImportData *ui;
    QString connection;
    QString path;
    QString data;
    QString table;
    QStringList heders;

    void readCSVfile(QString data, QString delimiter);
};

#endif // FRMIMPORTDATA_H
