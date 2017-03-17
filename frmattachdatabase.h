#ifndef FRMATTACHDATABASE_H
#define FRMATTACHDATABASE_H

#include <QDialog>
#include <QtSql>
#include <QtWidgets>

namespace Ui {
class FrmAttachDatabase;
}

class FrmAttachDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit FrmAttachDatabase(QString path, QString connection, QWidget *parent = 0);
    ~FrmAttachDatabase();

signals:
    void executeSqlQuery(QStringList upitList, QString connection);
    void finished(bool isFinished);
    void setPath(QString path);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_tbtnBrowse_clicked();

    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    Ui::FrmAttachDatabase *ui;
    QString connection;
    QString path;
};

#endif // FRMATTACHDATABASE_H
