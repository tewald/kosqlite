#ifndef FRMVACUUM_H
#define FRMVACUUM_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class FrmVacuum;
}

class FrmVacuum : public QDialog
{
    Q_OBJECT

public:
    explicit FrmVacuum(QString connection, QWidget *parent = 0);
    ~FrmVacuum();

signals:
    void executeSqlQuery(QStringList upitList, QString connection);
    void finished(bool isFinished);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnVacuum_clicked();

    void on_btnVacuumAll_clicked();

    void on_btnClose_clicked();

private:
    Ui::FrmVacuum *ui;

    QString connection;
};

#endif // FRMVACUUM_H
