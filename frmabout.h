#ifndef FRMABOUT_H
#define FRMABOUT_H

#include <QDialog>
#include <QtCore>
#include <QtGui>

namespace Ui {
class FrmAbout;
}

class FrmAbout : public QDialog
{
    Q_OBJECT

public:
    explicit FrmAbout(QWidget *parent = 0);
    ~FrmAbout();

private slots:
    void on_btnOK_clicked();

private:
    Ui::FrmAbout *ui;
};

#endif // FRMABOUT_H
