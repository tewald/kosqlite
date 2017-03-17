#ifndef FRMOPTIONS_H
#define FRMOPTIONS_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "pages.h"

namespace Ui {
class FrmOptions;
}

class FrmOptions : public QDialog
{
    Q_OBJECT

public:
    explicit FrmOptions(QWidget *parent = 0);
    ~FrmOptions();

signals:
    void optionsIzmena();

private slots:
    /**
     * @brief on_listOptions_currentItemChanged --zamena trunutnog widget stack-a
     * @param current
     * @param previous
     */
    void on_listOptions_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    ////////////////////////////////////////
    /**
     * @brief --učitavanje boja
     */
    void btnNormalText_clicked();
    void btnNumber_clicked();
    void btnStringDouble_clicked();
    void btnStringSingle_clicked();
    void btnKeyword_clicked();
    void btnFunction_clicked();
    void btnComment_clicked();

    void btnResetColors_clicked();
    /////////////////////////////////////////

    /**
     * @brief on_btnResetAll_clicked --restart svih podešavanja na osnovne vrednosti
     */
    void on_btnResetAll_clicked();
    /**
     * @brief on_btnOK_clicked --snimanje podešavanja i izlazak
     */
    void on_btnOK_clicked();
    /**
     * @brief on_btnApply_clicked --snimanje podešavanja
     */
    void on_btnApply_clicked();
    /**
     * @brief on_btnCancel_clicked --odustajenje od unetih podešavanja
     */
    void on_btnCancel_clicked();

private:
    /**
     * @brief loadValues --čitanje podešavanja is settings datoteke
     */
    void loadValues();
    /**
     * @brief loadDefaultValues --postavka defoultih podešavanja
     */
    void loadDefaultValues();
    /**
     * @brief saveValues --snimanje podešavanja u settings datoteku
     */
    void saveValues();
    /**
     * @brief createIcon --pravljenje ikonice
     * @param color --boja ikonice
     * @return --vraća se napravljena ikonica
     */
    QIcon createIcon(QColor color);

    Ui::FrmOptions *ui;

    /**
     * @brief pagesWidget --lista grupisanih widgeta, koji se u zavisnosti od aktivnog QListWidgetItema prikazuje
     */
    QStackedWidget *pagesWidget;

    /**
     * @brief listaBoja --lista učitanih boja, potrebno mi je da znam koju da prosledim u qcolordialog
     */
    QList<QColor> listaBoja;
};

#endif // FRMOPTIONS_H
