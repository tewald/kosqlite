#ifndef FRMMAINWINDOW_H
#define FRMMAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtSql>
#include <QtPrintSupport>

//
#include "labelvalue.h"
#include "sqleditor.h"
#include "querysyntaxhighlighter.h"
#include "sqlfile.h"
#include "queryexecutionobject.h"
#include "queryvalidator.h"
#include "sqltabwidget.h"
#include "downloader.h"
//
#include "frmunsaveddocuments.h"
#include "frmoptions.h"
#include "frmupdate.h"
#include "frmabout.h"

#include "frmrenametable.h"
#include "frmvacuum.h"
#include "frmanalyze.h"
#include "frmimportdata.h"
#include "frmeditdata.h"
#include "frmattachdatabase.h"
#include "frmpragmas.h"
#include "frmblobdetails.h"
#include "frmfindandreplace.h"
#include "kosqlquerymodel.h"

namespace Ui {
class FrmMainWindow;
}

class FrmMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FrmMainWindow(QWidget *parent = 0);
    ~FrmMainWindow();

signals:
    void changeColors(QList<QColor> colors);
    void findReplace(QString wordUnderCursor);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    ///[1] --ovo se nalazi u meniju
    void on_actionNew_Database_triggered();

    void on_actionOpen_Database_triggered();

    void on_actionNew_Query_Tab_triggered();

    void on_actionClose_All_Querys_Except_This_triggered();

    void on_actionOpen_SQL_Script_triggered();

    void on_actionSave_SQL_Script_triggered();

    void on_actionSave_SQL_Script_As_triggered();

    void on_actionPrint_triggered();

    void on_actionExit_triggered();

    void on_actionSelect_All_triggered();

    void on_actionFind_triggered();

    void on_actionFind_And_Replace_triggered();

    void on_actionExecute_Query_triggered();

    void on_actionExecute_Query_Under_Cursor_triggered();

    void on_actionExecute_Explain_Query_Plan_triggered();

    void on_actionStop_Query_triggered();

    void on_actionExport_Schema_triggered();

    void on_actionDump_Database_triggered();

    void on_actionAnalyze_Statistics_triggered();

    void on_actionVacuum_triggered();

    void on_actionAttach_Database_triggered();

    void on_actionPragma_Statements_triggered();

    void on_actionOptions_triggered();

    void on_actionReset_Layout_triggered();

    void on_actionHelp_triggered();

    void on_actionCheck_For_Updates_triggered();

    void on_actionAbout_KoSQLite_triggered();
    ///[1]

    ///[2] --ovo se nalazi u toolBaru
    void on_actionReload_triggered();
    ///[2]

    ///[3] zatvaranje taba
    void on_tabDatabase_tabCloseRequested(int index);
    ///[3]

    ///[6] --pretraga i zamena
    void btnPrevious_clicked(QString find, QTextDocument::FindFlags findFlags);

    void btnNext_clicked(QString find, QTextDocument::FindFlags findFlags);

    void btnReplace_clicked(QString find, QString replace, QTextDocument::FindFlags findFlags);

    void btnReplaceAll_clicked(QString find, QString replace, QTextDocument::FindFlags findFlags);
    ///[6]

    void on_tabResult_tabCloseRequested(int index);

private slots: //custom slotovi
    /**
     * @brief openDatabase --otvaranje, dodavanje nove baze sa kojom se radi
     * @param database --putanja ka bazi koja se otvara
     */
    void openDatabase(QString database);

    /**
     * @brief closeDatabase --zatvara se konekcija, briše se iz listi konekcija
     * @param database --baza koja treba da se zatvori
     */
    void closeDatabase(QString connection);

    /**
     * @brief treeDatabase_itemClicked --kada se klikne na field u drvetu, onda se čitaju podaci o tom polji i prikazuju se u field information
     * @param item --index na koji je kliknut
     * @param column --kolona je uvek 0
     */
    void treeDatabase_itemClicked(QTreeWidgetItem *item,int column);

    /// --dvoklik na labelu čisto da bi se tekst selektovao
    void lblFieldValue_doubleClicked();
    void lblTypeValue_doubleClicked();
    void lblNullValue_doubleClicked();
    void lblDefaultValue_doubleClicked();
    ///

    ///[4]
    void clipbord_dataChanged();

    void activeCut();

    void activeCopy();

    void activePaste();

    void activeDelete();

    void activeUndo();

    void activeRedo();
    ///[4]

    ///[5] --pretraga i zamena
    /**
    * @brief txtFindLineEdit_textChanged --pretraga sqlEditor-a kako korisnik kuca
    * @param searchString --otkucan tekst, koji se traži
    */
   void txtFindLineEdit_textChanged(QString searchString, QTextDocument::FindFlags findFlags);
   ///[5]
   ///

   /**
   * @brief sqlEditor_modificationChanged --ukoliko je sqlEditor modifikan dodaje se * na kraj naziva taba, što treba da aludira da nije snimljeno, i suprotno, ako je sve snimljeno * se briše
   * @param state --stanje dokumenta, da li je snimljeno ili ne
   */
  void sqlEditor_modificationChanged(bool state);

  void queryOutput(QString vreme, QString elapsedTime, QString lastQuery, QString databaseText);

  void queryResult(QSqlQuery result);

  void tabQuery_currentChanged(int index);

  void tabQuery_tabCloseRequested(int index);

  void tabQuery_middleClicked(int tabIndex);

  void tabQuery_customContextMenuRequested(const QPoint pos);

  void tblOutput_customContextMenuRequested(const QPoint pos);

  void sqlEditor_customContextMenuRequested(const QPoint);

  void treeDatabase_customContextMenuRequested(const QPoint pos);

  void tblResult_customContextMenuRequested(const QPoint pos);

  void on_tabResult_customContextMenuRequested(const QPoint pos);

  void actionCloneTab_triggered();

  void actionTOCopyMessage_triggered();

  void actionTOCopyAction_triggered();

  void actionTOCopy_triggered();

  void actionTOClearOutput_triggered();

  void actionTRCopyRow_triggered();

  void actionTRCopyField_triggered();

  void actionTRCopy_triggered();

  void actionTRBlob_triggered();

  void actionCloseResultsOnRight_triggered();

  void actionCloseAllResultsExceptThis_triggered();

  void actionCloseAllResults_triggered();

  void updateCheck();

  void errorMessage(const QString &error);

  void optionsIzmena();

  void openRecentDatabase();
  //desni klik meni na treeDatabase
  void actionCreateTable_triggered();
  void actionDescribeTable_triggered();
  void actionAlterTable_triggered();
  void actionRenameTable_triggered();
  void actionDropTable_triggered();
  void actionImportData_triggered();
  void actionEditData_triggered();
  void actionCreateView_triggered();
  void actionDescribeView_triggered();
  void actionAlterView_triggered();
  void actionDropView_triggered();
  void actionCreateTrigger_triggered();
  void actionDescribeTrigger_triggered();
  void actionAlterTrigger_triggered();
  void actionDropTrigger_triggered();
  void actionCreateIndex_triggered();
  void actionDescribeIndex_triggered();
  void actionReIndex_triggered();
  void actionDropIndex_triggered();
  void actionDetachDatabase_triggered();

  void executeSqlQuery(QStringList upitList, QString connection);

  void update_timeout();

  void setPath(QString path);

  void setFindAndReplaceActivity(bool activity);


  void on_actionClose_All_Querys_triggered();

private:
    Ui::FrmMainWindow *ui;

    QString path; //putanja koja se koristi kao defaultna kod open file, save file dijaloga
    int autosaveTime;
    bool confirmClosing;
    bool keepContent;
    bool openLastDatabasesOnStart;
    int recentlyUsedDatabases;

    /// --brojači tabova koji su otvoreni
    int brTabovaDatabase;
    int brTabovaQuery;
    int brTabovaResult;
    ///

    /// --objekat i nit na koju se prosleđuje izvršavanje upita
    QueryExecutionObject *queryExecution;
    QPointer<QThread> queryThread;

    /**
     * @brief dbConnections --lista konekcija koje su ostvarene prema bazi podataka
     *                        po default su otvorene, zatvaraju se tek kad korisnik klikne na zatvaranje
     *                        ili prilikom pozivanja destruktora!
     *  QString prvi je za ime objekta
     *  QString drugi je za ime konekcija ka bazi
     */
    QMap<QString, QString> dbConnections;

    /**
     * @brief sqlFiles --lista *.sql fajlova koji su otvoreni ili snimljeni a nalaze se u tabquery
     */
    QMap<QString, SQLFile> sqlFiles;

    /// --informacije o polju na koje je kliknuto, ime polja, tip podatka, da li je null i defoultna vrednost
    LabelValue *lblFieldValue;
    LabelValue *lblTypeValue;
    LabelValue *lblNullValue;
    LabelValue *lblDefaultValue;
    ///

    /**
     * @brief tabQuery --tabWidget u kom se smešteni otvorene *.sql datoteke
     */
    SqlTabWidget *tabQuery;

    //////////////////////////////////////tabSQLeditor menu
    QMenu *menuTabQuery;
    QAction *actionCloneTab;

    int indexQueryClicked;
    //////////////////////////////////////

    //////////////////////////////////////tabResult menu
    QMenu *menuTblOutput;

    QAction *actionTOCopyMessage;
    QAction *actionTOCopyAction;
    QAction *actionTOCopy;
    QAction *actionTOClearOutput;
    //////////////////////////////////////

    //////////////////////////////////////sqlEditor menu

    QMenu *menuSqlEditor;

    //////////////////////////////////////


    //////////////////////////////////////
    QMenu *menuTreeDatabase;
    //ovo važi samo ako se klikne na Tables
    QAction *actionCreateTable;
    //ovo važi samo ako se klikne na tabelu
    QAction *actionDescribeTable;
    QAction *actionAlterTable;//iskače prozor sa opcijama sa izmenu tabele
    QAction *actionRenameTable;
    QAction *actionDropTable;
    QAction *actionImportData;//CSV fajl
    QAction *actionEditData;
    //view, trigger, index
    QAction *actionCreateView;
    QAction *actionCreateTrigger;
    QAction *actionCreateIndex;
    QAction *actionDescribeView;
    QAction *actionAlterView;
    QAction *actionDropView;
    QAction *actionDescribeTrigger;
    QAction *actionAlterTrigger;
    QAction *actionDropTrigger;
    QAction *actionDescribeIndex;
    QAction *actionReIndex;
    QAction *actionDropIndex;
    //ovo važi samo za tabelu, i to ako nije main
    QAction *actionDetachDatabase;

    /**
     * @brief itemRightClicked --item iz treeWidget na koji je kliknuto desnim klikom.
     * ovo mora da se globalizuje jer ako su meniju izabere neki action, znači miš se pomerio
     * pa se samim tim više ne može odrediti sa tačnošću na šta je kliknuto, i dolazi do brlje
     */
    QTreeWidgetItem *itemRightClicked;
    //////////////////////////////////////

    //////////////////////////////////////
    QMenu *menuTblResult;

    QAction *actionTRCopyRow;
    QAction *actionTRCopyField;
    QAction *actionTRCopy;
    QAction *actionTRBlob;
    //////////////////////////////////////

    //////////////////////////////////////
    QMenu *menuTblResultTab;

    QAction *actionCloseResultsOnRight;
    QAction *actionCloseAllResultsExceptThis;
    QAction *actionCloseAllResults;

    int indexResultClicked;
    //////////////////////////////////////

    /**
     * @brief indexContextMenu --ovo mi služi da znam na koji index je kliknuto desnim klikom, tblResult
     */
    QModelIndex indexContextMenu;
    /////////////////////////////////////////////////////

    ///meni za recent databases
    QMenu *menuRecentDatabases;
    //ima dinamčki broj QActions, u zavisnosti podešavanja
    ///


    bool isFindAndReplaceActive;

    /**
     * @brief d --provera updejta aplikacije
     */
    Downloader *d;
    /**
     * @brief loading --da li je u pitanju prvo učitavanje TRUE, ili provera FALSE
     */
    bool loading;

    /**
     * @brief autosave --dodavanje tajmera, ukoliko se menja vreme tajmera tada se tajmer prvo obriše pa se doda nov
     */
    void autosave();

    /**
     * @brief relocateRecentDatabase --ovo se dešava ako se doda nova baza, kreira nova baza ili otvori nova baza, pa i STARA
     * @param database
     */
    void relocateRecentDatabase(QString database);

    /// ---metode za popunjavanje drveta, dodavanje tabele, polja, indeksa systemskih indeksa i okidača
    QTreeWidgetItem *addDatabase(QTreeWidget *parent, QString name);

    QTreeWidgetItem *addTable(QTreeWidgetItem *parent, QString name);

    void addField(QTreeWidgetItem *parent, QString name, int isPK);

    void addIndexTrigger(QTreeWidgetItem *parent, QString name);

    QTreeWidgetItem* addViews(QTreeWidgetItem *parent, QString name);

    void addViewTrigger(QTreeWidgetItem *parent, QString name);

    void addSystemCatalogue(QTreeWidgetItem *parent, QString name);
    ///

    void inicijalizacijaFieldInformation();

    //void inicijalizacijaSearchAndReplace();

    void inicijalizacijaGrupnihAkcija();//cut, copy, paste, undo, redo

    void inicijalizacijaTabQuery();

    void inicijalizacijaTabeleOutput();//poruke o greškama i uspešnosti izvršenja upita

    void inicijalizacijaDesniKlikMenija(); //svi desni klik meniji

    void inicijalizacijaOsnovnihVrednostiIzSettings();

    void inicijalizacijaDokumenataKojiSuBiliOtvoreni();

    //otvara baze koje su u prethodnoj sesiji bili otvorene
    void inicijalizacijaPrethodnoOtovorenihBaza();

    void inicijalizacijaProveraUpdate();

    //ovo puni meni recent databases sa nedavno otvaranim bazama
    void inicijalizacijaRecentDatabases();

    void inicijalizacijaSystemMenija();

    //void inicijalizacijaNewTaba();

    /**
     * @brief activeTreeDatabase --getter za aktivan tab, tj. bazu, OVO MI JE POTREBNO zbog toga što dinamički po potrebi kreiram beskonačan broj drva
     * @param current --trenutan tab`
     * @return --vraća trenutno drvo
     */
    QTreeWidget *activeTreeDatabase(int current);

    SqlEditor *activeSqlEditor(int current);

    QTableView *activeResultTable(int current);

    QueryExecutionObject *activeExecutionObject(QString connection);

    /**
     * @brief databaseKey --vraća trenutnu konekciju
     * @return
     */
    QString activeConnection();
};

#endif // FRMMAINWINDOW_H
