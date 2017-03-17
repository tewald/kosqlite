#ifndef FRMRENAMETABLE_H
#define FRMRENAMETABLE_H

#include <QDialog>

namespace Ui {
class FrmRenameTable;
}

class FrmRenameTable : public QDialog
{
    Q_OBJECT

public:
    explicit FrmRenameTable(QString currentName, QWidget *parent = 0);
    ~FrmRenameTable();
    QString tableName();

private slots:
    void on_btnRename_clicked();

    void on_btnCancel_clicked();

private:
    Ui::FrmRenameTable *ui;
    QString currentName;
};

#endif // FRMRENAMETABLE_H
