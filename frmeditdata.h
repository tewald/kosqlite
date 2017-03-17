#ifndef FRMEDITDATA_H
#define FRMEDITDATA_H

#include <QDialog>
#include <QtSql>
#include <QtWidgets>

#include "kosqltablemodel.h"

#include "frmblobdetails.h"

namespace Ui {
class FrmEditData;
}

class FrmEditData : public QDialog
{
    Q_OBJECT

public:
    explicit FrmEditData(QString connection, QString table, QString path, QWidget *parent = 0);
    ~FrmEditData();

signals:
    void finished(bool isFinished);
    void finished();
    void setPath(QString path);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnClose_clicked();

    void actionAddRow_triggered();

    void actionDeleteRow_triggered();

    void actionInsertBlob_triggered();

    void actionDetailsBlob_triggered();

    void actionClear_triggered();

    void actionNull_triggered();

    void actionSubmit_triggered();

    void actionRevert_triggered();

    void setPathBLOB(QString path);

private:
    Ui::FrmEditData *ui;    
    QString table;
    QString connection;
    QString path;

    QAction *actionAddRow;
    QAction *actionDeleteRow;
    QAction *actionInsertBlob;
    QAction *actionDetailsBlob;
    QAction *actionClear;
    QAction *actionNull;
    QAction *actionSubmit;
    QAction *actionRevert;

    QModelIndexList editedCells;

    void inicijalizacijaToolBara();
};

#endif // FRMEDITDATA_H
