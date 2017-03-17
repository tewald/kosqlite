#ifndef FRMANALYZE_H
#define FRMANALYZE_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class FrmAnalyze;
}

class FrmAnalyze : public QDialog
{
    Q_OBJECT

public:
    explicit FrmAnalyze(QString connection, QWidget *parent = 0);
    ~FrmAnalyze();

signals:
    void executeSqlQuery(QStringList upitList, QString connection);
    void finished(bool isFinished);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnAnalyze_clicked();

    void on_btnAnalyzeAll_clicked();

    void on_btnClose_clicked();

private:
    Ui::FrmAnalyze *ui;
    QString connection;
};

#endif // FRMANALYZE_H
