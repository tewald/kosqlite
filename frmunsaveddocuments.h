#ifndef FRMUNSAVEDDOCUMENTS_H
#define FRMUNSAVEDDOCUMENTS_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class FrmUnsavedDocuments;
}

class FrmUnsavedDocuments : public QDialog
{
    Q_OBJECT

public:
    explicit FrmUnsavedDocuments(QWidget *parent = 0);
    ~FrmUnsavedDocuments();

    void setUnsavedDocuments(QStringList documents);

protected:
    void closeEvent(QCloseEvent *event);

private slots:

private:
    Ui::FrmUnsavedDocuments *ui;

    QStringList documents;
};

#endif // FRMUNSAVEDDOCUMENTS_H
