#ifndef FRMPRAGMAS_H
#define FRMPRAGMAS_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class FrmPragmas;
}

class FrmPragmas : public QDialog
{
    Q_OBJECT

public:
    explicit FrmPragmas(QString connection, QWidget *parent = 0);
    ~FrmPragmas();

signals:
    void finished();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnReload_clicked();

    void on_btnClose_clicked();

private:
    Ui::FrmPragmas *ui;
    QString connection;

    void loadPragmaStatements();
};

#endif // FRMPRAGMAS_H
