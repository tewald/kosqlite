#ifndef FRMFINDHANDREPLACE_H
#define FRMFINDHANDREPLACE_H

#include <QDialog>
#include "findlineedit.h"
#include <QtWidgets>

namespace Ui {
class FrmFindAndReplace;
}

class FrmFindAndReplace : public QDialog
{
    Q_OBJECT

public:
    explicit FrmFindAndReplace(QWidget *parent = 0);
    ~FrmFindAndReplace();

public slots:
    void findReplace(QString wordUnderCursor);

signals:
    void btnPrevious(QString find, QTextDocument::FindFlags findFlags);
    void btnNext(QString find, QTextDocument::FindFlags findFlags);
    void btnReplace(QString find, QString replace, QTextDocument::FindFlags findFlags);
    void btnReplaceAll(QString find, QString replace, QTextDocument::FindFlags findFlags);
    void txtFind(QString find, QTextDocument::FindFlags findFlags);
    void finished(bool);
    void finished();


protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_btnPrevious_clicked();

    void on_btnNext_clicked();

    void on_btnReplace_clicked();

    void on_btnReplaceAll_clicked();

    void txtFindLineEdit_textChanged(const QString &arg1);

    void actionWholeWordsOnly_triggered();

    void actionCaseSensitive_triggered();

    void textChanged();

    void on_btnClose_clicked();

private:
    Ui::FrmFindAndReplace *ui;

    /**
     * @brief txtFindLineEdit --pretraga za opcijama
     */
    FindLineEdit *txtFindLineEdit;
    /**
     * @brief searchOptions --meni koji se poziva klikom na searchOptions dugme koje se nalazi u lineEditu, i pozivaju se opcije za pretragu
     */
    QMenu *menuSearchOptions;
    /**
     * @brief actionCaseSensitive --Opcija za pretragu case sensitive
     */
    QAction *actionCaseSensitive;
    /**
     * @brief actionWholeWordsOnly --Opcija za pretragu samo cele reči
     */
    QAction *actionWholeWordsOnly;
};

#endif // FRMFINDHANDREPLACE_H
