#include "frmmainwindow.h"
#include "ui_frmmainwindow.h"

FrmMainWindow::FrmMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FrmMainWindow)
{
    ui->setupUi(this);

    this->setWindowState(Qt::WindowMaximized);

    ///inicijalne vrednosti brojača tabova
    brTabovaDatabase = 0;
    brTabovaQuery = 0;
    brTabovaResult = 0;

    setFindAndReplaceActivity(false);

    ///about qt
    connect(ui->actionAbout_Qt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));

    inicijalizacijaFieldInformation();

    inicijalizacijaGrupnihAkcija();

    inicijalizacijaTabQuery();

    inicijalizacijaTabeleOutput();

    ///inicijalizacija splittera
    ui->splitterDatabaseOdFieldInformation->setSizes(QList<int>() << 450 << 180);
    ui->splitterQueryOdTblOdMessage->setSizes(QList<int>() << 250 << 200 << 100);
    ui->splitterLeveOdDesneStrane->setSizes(QList<int>() << 100 << 550);

    inicijalizacijaDesniKlikMenija();

    inicijalizacijaOsnovnihVrednostiIzSettings();

    inicijalizacijaDokumenataKojiSuBiliOtvoreni();

    inicijalizacijaPrethodnoOtovorenihBaza();

    inicijalizacijaProveraUpdate();

    inicijalizacijaRecentDatabases();

    inicijalizacijaSystemMenija();

    //ukoliko je dvokliknuto na bazu podataka, program će je automatski otvoriti
    if (QApplication::arguments().count() > 1)
        if ( QFileInfo( QApplication::arguments().at(1) ).exists() )
            openDatabase(QApplication::arguments().at(1));

    ui->actionHelp->setEnabled(false);
}

FrmMainWindow::~FrmMainWindow()
{
    delete this->findChild<QTimer*>("autosavetimer");

    delete ui;
    //ukoliko ima konekcija prema bazi zatvori ih
    foreach (QString str, QSqlDatabase::connectionNames())
        closeDatabase(str);
}

void FrmMainWindow::closeEvent(QCloseEvent *event)
{
    //ukoliko upita koji se izvršava, u tom slučaju ignoriši zatvaranje,
    //da bi se program zatvorio upit se mora prekunuti na stop ili sačekati da se izvrše upiti
    if (queryThread != NULL)
    {
        event->ignore();
        return;
    }

    if (confirmClosing == true)
    {
        QStringList unsavedList;

        for (int i = 0; i < tabQuery->count(); i++)
        {
            SqlEditor *sqlEditor = activeSqlEditor(i);

            if (sqlEditor->document()->isModified() == true)
            {
                if (sqlFiles.value(sqlEditor->objectName()).getFile().isEmpty())//ukoliko je vrednost pod ključem prazan string
                    unsavedList << sqlEditor->objectName();//onda upiši ključ
                else//u suprotnom vrednost pod ključem upiši
                    unsavedList << sqlFiles.value(sqlEditor->objectName()).getFile();
            }
        }

        if (unsavedList.count() > 0)//ako ima ne snimljenih tabova, pita da li da ih snimiš ili da izađe
        {
            FrmUnsavedDocuments *unsaved = new FrmUnsavedDocuments(this);
            unsaved->setUnsavedDocuments(unsavedList);
            int rez = unsaved->exec();

            switch(rez)
            {
            case QDialog::Accepted: //ima ne snimljenih
                event->ignore();
                break;

            case QDialog::Rejected: //nema ne snimljenih
                event->accept();
                break;
            }
        }
    }
    //snimanje upita koji su ostali u tabovima, bez obzira da li korisnik hoće ili neće da snimi
    QSettings settingsQuery(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName().append("_query"));

    settingsQuery.beginWriteArray("querys");

    int br = 0;

    for(int i = 0; i < tabQuery->count(); i++)
    {
        if (!activeSqlEditor(i)->toPlainText().isEmpty())
        {
            settingsQuery.setArrayIndex(br);
            settingsQuery.setValue("tab", tabQuery->tabText(i));
            settingsQuery.setValue("query", activeSqlEditor(i)->toPlainText());
            br++;
        }
    }
    settingsQuery.endArray();
    //snimanje stanja splittera
    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("splitterState");
    settingsConfig.setValue("splitterDatabaseOdFieldInformation", ui->splitterDatabaseOdFieldInformation->saveState());
    settingsConfig.setValue("splitterLeveOdDesneStrane", ui->splitterLeveOdDesneStrane->saveState());
    settingsConfig.setValue("splitterQueryOdTblOdMessage", ui->splitterQueryOdTblOdMessage->saveState());
    settingsConfig.endGroup();

    //snimanje otvorenih baza podataka
    settingsConfig.beginGroup("databases");

    QStringList lastDbs;
    QMapIterator<QString, QString> iterator(dbConnections);

    while (iterator.hasNext())
    {
        iterator.next();
        lastDbs.append( iterator.value() );
    }

    settingsConfig.setValue("lastDatabases", lastDbs);

    settingsConfig.endGroup();
}

void FrmMainWindow::on_actionNew_Database_triggered()
{
    //Za SQLite nema predefinisane ekstenzije, koriste se *.db, *.db3, *.sqlite3 itd...
    QString database = QFileDialog::getSaveFileName(this, tr("New SQLite Database"), path, tr("All Files(*)"));

    if (!database.isEmpty())
    {
        //ukoliko baza već postoji, a korisnik hoće da je override-uje
        if (QFileInfo(database).exists())
        {
            QString key = dbConnections.key(database);

            //bazu je potrebno ukloniti samo ako je već otvorena
            if (!key.isEmpty())
            {
                int index = ui->tabDatabase->indexOf(ui->tabDatabase->findChild<QWidget*>(key));
                closeDatabase(dbConnections.value(key));
                ui->tabDatabase->widget(index)->deleteLater();
                ui->tabDatabase->removeTab(index);
                //dbConnections.remove(key);
            }

            //samo ako je uspešno obrisano onda se otvara
            QFile file;
            if (file.remove(database))
                openDatabase(database);
        }
        else
            openDatabase(database);

        relocateRecentDatabase(database);

        setPath(QFileInfo(database).path());
    }
}

void FrmMainWindow::on_actionOpen_Database_triggered()
{
    QString database = QFileDialog::getOpenFileName(this, tr("Open SQLite Database"), path, tr("All Files(*)"));

    if (!database.isEmpty())
    {
        openDatabase(database);

        relocateRecentDatabase(database);

        setPath(QFileInfo(database).path());
    }
}

void FrmMainWindow::on_actionNew_Query_Tab_triggered()
{
    tabQuery->blockSignals(true);

    brTabovaQuery++;

    //QTabWidget-ov tab je u stvari jedan QWidget, tako da bi se tab dodao, dodaje se nov widget
    QWidget *widget = new QWidget(tabQuery);
    widget->setObjectName(QString("Query %1").arg(brTabovaQuery));
    tabQuery->insertTab(tabQuery->count(), widget,QIcon(":/icons/icons/document.png"),QString("Query %1").arg(brTabovaQuery));

    //postavljam layout na tom widgetu, zbog poravnanja prilikom razvlačenja
    QVBoxLayout *layout = new QVBoxLayout(tabQuery->widget(tabQuery->count() - 1));
    tabQuery->widget(tabQuery->count() - 1)->setLayout(layout);

    //sql editor dodajem u taj layout
    SqlEditor *sql = new SqlEditor(tabQuery->widget(tabQuery->count() - 1));
    layout->addWidget(sql);
    sql->setObjectName(QString("Query %1").arg(brTabovaQuery));

    //higlighter dodajem na sql editor
    QuerySyntaxHighlighter *highlighter = new QuerySyntaxHighlighter(sql->document());//highlighter postavka
    connect(this,SIGNAL(changeColors(QList<QColor>)),highlighter,SLOT(changeColors(QList<QColor>)));

    //desni klik meni na sql editor
    sql->setContextMenuPolicy(Qt::CustomContextMenu);
    //signali i slotovi koje treba vezati
    connect(sql,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(sqlEditor_customContextMenuRequested(QPoint)));
    connect(sql,SIGNAL(modificationChanged(bool)),this,SLOT(sqlEditor_modificationChanged(bool)));
    connect(sql,SIGNAL(copyAvailable(bool)),ui->actionCopy,SLOT(setEnabled(bool)));
    connect(sql,SIGNAL(copyAvailable(bool)),ui->actionCut,SLOT(setEnabled(bool)));
    connect(sql,SIGNAL(copyAvailable(bool)),ui->actionDelete,SLOT(setEnabled(bool)));//copyAvailable je u suštini da je ima selektovanog teksta
    connect(sql,SIGNAL(undoAvailable(bool)),ui->actionUndo,SLOT(setEnabled(bool)));
    connect(sql,SIGNAL(redoAvailable(bool)),ui->actionRedo,SLOT(setEnabled(bool)));

    ///ukoliko je prozor za pretragu otvoren onda ovo poveži
    if (isFindAndReplaceActive)
        connect(sql->document(),SIGNAL(contentsChanged()),this->findChild<FrmFindAndReplace*>("findReplace"),SLOT(textChanged()));

    SQLFile sqlFile;
    sqlFile.setFileName(QString("Query %1").arg(brTabovaQuery));

    //u mapu openFiles se dodaje broj taba i prazan string kao naziv, jer još nije snimljen na disk
    sqlFiles.insert(QString("Query %1").arg(brTabovaQuery), sqlFile);

    tabQuery->blockSignals(false);
}

void FrmMainWindow::on_actionClose_All_Querys_Except_This_triggered()
{
    if (indexQueryClicked == -1)
        indexQueryClicked = tabQuery->currentIndex();

    for (int i = tabQuery->count()-1; i >= 0; i--)
    {
        if (i != indexQueryClicked)
            tabQuery->tabCloseRequested(i);
    }

    indexQueryClicked = -1;
}

void FrmMainWindow::on_actionClose_All_Querys_triggered()
{
    for (int i = tabQuery->count()-1; i >= 0; i--)
        tabQuery->tabCloseRequested(i);
}

void FrmMainWindow::on_actionOpen_SQL_Script_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open SQL Script"), path, "SQL files (*.sql)");

    if (!filename.isEmpty())
    {
        QFile file(filename);

        if (file.open(QIODevice::ReadOnly))
        {
            on_actionNew_Query_Tab_triggered();
            tabQuery->setCurrentIndex(tabQuery->count() - 1);
            SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

            QTextStream in(&file);
            sqlEditor->setPlainText(in.readAll());
            file.close();

            //ako uspe čitanje iz datoteke
            SQLFile sqlFile(filename);
            sqlFiles.insert(tabQuery->widget(tabQuery->currentIndex())->objectName(), sqlFile); //ubacivanje putanje datoteke u listu otvorenih datoteka
        }
    }
}

void FrmMainWindow::on_actionSave_SQL_Script_triggered()
{
    //ovo treba da pita samo kad se ne zna putanja datoteke, tj. nikada nije snimano
    QString filename = sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName()).getFile();

    if (filename.isEmpty())
        filename = QFileDialog::getSaveFileName(this, tr("Save SQL Script"), path, "SQL files (*.sql)");

    if (!filename.isEmpty())
    {///za save treba samo kod prvog pitanja da pita naziv datoteke, kod drugog samo da override već postojoće

        SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

        if (QFileInfo(filename).suffix() != "sql")
            filename = filename.append(".sql");

        QFile file(filename);
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream out(&file);
            out << sqlEditor->toPlainText();
            file.close();
        }

        SQLFile sqlFile;
        sqlFile = sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName());
        sqlFile.setFile(filename);
        //kod mape nema replace, nego kada se insertuje ukoliko postoji prepiše se
        sqlFiles.insert(tabQuery->widget(tabQuery->currentIndex())->objectName(), sqlFile);

        tabQuery->setTabText(tabQuery->currentIndex(),sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName()).getFileName());
        sqlEditor->document()->setModified(false);

        setPath(sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName()).getFilePath());
    }
}

void FrmMainWindow::on_actionSave_SQL_Script_As_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save SQL Script"), path, "SQL files (*.sql)");

    if (!filename.isEmpty())
    {
        SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

        if (QFileInfo(filename).suffix() != "sql")
            filename = filename.append(".sql");

        QFile file(filename);
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream out(&file);
            out << sqlEditor->toPlainText();
            file.close();
        }

        SQLFile sqlFile;
        sqlFile = sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName());
        sqlFile.setFile(filename);
        //kod mape nema replace, nego kada se insertuje ukoliko postoji prepiše se
        sqlFiles.insert(tabQuery->widget(tabQuery->currentIndex())->objectName(), sqlFile);

        tabQuery->setTabText(tabQuery->currentIndex(),sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName()).getFileName());
        sqlEditor->document()->setModified(false);

        setPath(sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName()).getFilePath());
    }
}

void FrmMainWindow::on_actionPrint_triggered()
{
    QPrinter printer(QPrinterInfo::defaultPrinter());
    QPrintDialog *dialog = new QPrintDialog(&printer, this);

    if (dialog->exec() == QDialog::Accepted)
    {
        SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());
        //ako je dokument u NoWrap modu onda prilikom štampe beži sa strane, ovako ga zavaram
        if (sqlEditor->lineWrapMode() == QPlainTextEdit::NoWrap)
        {
            sqlEditor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
            sqlEditor->print(&printer);
            sqlEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
        }
        else
            sqlEditor->print(&printer);
    }
}

void FrmMainWindow::on_actionExit_triggered()
{
    this->close();
}

void FrmMainWindow::on_actionSelect_All_triggered()
{
    SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

    sqlEditor->selectAll();
}

void FrmMainWindow::on_actionFind_triggered()
{
    //ubacivanje teksta uspod kursora u pretragu
    SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

    QTextCursor cursor = sqlEditor->textCursor();

    if (cursor.selectedText().isEmpty())
        cursor.select(QTextCursor::WordUnderCursor);

    if (isFindAndReplaceActive == false)
    {
        FrmFindAndReplace *frm = new FrmFindAndReplace(this);
        frm->setObjectName("findReplace");
        connect(frm,SIGNAL(btnPrevious(QString, QTextDocument::FindFlags)),this,SLOT(btnPrevious_clicked(QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(btnNext(QString, QTextDocument::FindFlags)),this,SLOT(btnNext_clicked(QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(btnReplace(QString,QString, QTextDocument::FindFlags)),this,SLOT(btnReplace_clicked(QString,QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(btnReplaceAll(QString,QString, QTextDocument::FindFlags)),this,SLOT(btnReplaceAll_clicked(QString,QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(txtFind(QString, QTextDocument::FindFlags)),this,SLOT(txtFindLineEdit_textChanged(QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(finished(bool)),this,SLOT(setFindAndReplaceActivity(bool)));
        connect(frm,SIGNAL(finished()),frm,SLOT(deleteLater()));
        connect(this,SIGNAL(findReplace(QString)),frm,SLOT(findReplace(QString)));

        for (int i = 0; i < tabQuery->count(); i++)
        {
            SqlEditor *sqlEditor = activeSqlEditor(i);
            connect(sqlEditor->document(),SIGNAL(contentsChanged()),frm,SLOT(textChanged()));
        }

        frm->show();
        emit findReplace(cursor.selectedText());

        /// OVO je matematika da prozor za pretragu pošaljem u desni gornji ugao
        int x = this->geometry().topLeft().x() + this->rect().width() - 50 - frm->rect().width();
        int y = this->geometry().topLeft().y() + 160 - frm->rect().height();

        frm->move(QPoint(x, y));

        setFindAndReplaceActivity(true);
    }
    else
        emit findReplace(cursor.selectedText());
}

void FrmMainWindow::on_actionFind_And_Replace_triggered()
{
    //ubacivanje teksta uspod kursora u pretragu
    SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

    QTextCursor cursor = sqlEditor->textCursor();

    if (cursor.selectedText().isEmpty())
        cursor.select(QTextCursor::WordUnderCursor);

    if (isFindAndReplaceActive == false)
    {
        FrmFindAndReplace *frm = new FrmFindAndReplace(this);
        frm->setObjectName("findReplace");
        connect(frm,SIGNAL(btnPrevious(QString, QTextDocument::FindFlags)),this,SLOT(btnPrevious_clicked(QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(btnNext(QString, QTextDocument::FindFlags)),this,SLOT(btnNext_clicked(QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(btnReplace(QString,QString, QTextDocument::FindFlags)),this,SLOT(btnReplace_clicked(QString,QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(btnReplaceAll(QString,QString, QTextDocument::FindFlags)),this,SLOT(btnReplaceAll_clicked(QString,QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(txtFind(QString, QTextDocument::FindFlags)),this,SLOT(txtFindLineEdit_textChanged(QString, QTextDocument::FindFlags)));
        connect(frm,SIGNAL(finished(bool)),this,SLOT(setFindAndReplaceActivity(bool)));
        connect(frm,SIGNAL(finished()),frm,SLOT(deleteLater()));
        connect(this,SIGNAL(findReplace(QString)),frm,SLOT(findReplace(QString)));

        for (int i = 0; i < tabQuery->count(); i++)
        {
            SqlEditor *sqlEditor = activeSqlEditor(i);
            connect(sqlEditor->document(),SIGNAL(contentsChanged()),frm,SLOT(textChanged()));
        }

        frm->show();
        emit findReplace(cursor.selectedText());

        /// OVO je matematika da prozor za pretragu pošaljem u desni gornji ugao
        int x = this->geometry().topLeft().x() + this->rect().width() - 50 - frm->rect().width();
        int y = this->geometry().topLeft().y() + 160 - frm->rect().height();

        frm->move(QPoint(x, y));

        setFindAndReplaceActivity(true);
    }
    else
        emit findReplace(cursor.selectedText());
}

void FrmMainWindow::on_actionExecute_Query_triggered()
{
    SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

    QTextCursor cursor = sqlEditor->textCursor(); //interesuje me samo selektovan tekst

    QString sqlEditorText;

    if (!cursor.selectedText().isEmpty()) //selekcija nije prazna
        sqlEditorText = cursor.selection().toPlainText();
    else
        sqlEditorText = sqlEditor->toPlainText();

    //validacija stringa za izvršavanje upita
    QueryValidator validator;
    sqlEditorText = validator.removeUnneededContent(sqlEditorText);
    QStringList queryList = validator.parseToQuerys(sqlEditorText, false);

    executeSqlQuery(queryList, activeConnection());
}

void FrmMainWindow::on_actionExecute_Query_Under_Cursor_triggered()
{
    SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

    QString text = sqlEditor->toPlainText();

    QTextCursor cursor = sqlEditor->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);

    QueryValidator validator;
    //![0]
    //iz selektovanog teksta čistim komentare da bih mogao tačno da ustanovim koliko znakova
    //';' ima pre upita ispod cursora
    QString textSelected = cursor.selection().toPlainText();
    textSelected = validator.removeUnneededContent(textSelected);
    int queryUnderCursor = textSelected.count(";");
    //![0]

    //![1]
    //sav tekst koji se nalazi u sqlEditoru čistim i parsiram
    text = validator.removeUnneededContent(text);
    QStringList queryList = validator.parseToQuerys(text, false);
    //![1]

    //![2]
    //računam koji je to upit bio ispod kursora, pa ga izvršavam
    QString upitIspodCursora;
    int br = 0;

    for (int i = 0; i < queryList.count(); i++)
    {
        br += queryList.at(i).count(";");
        if (br > queryUnderCursor)
        {
            upitIspodCursora = queryList.at(i);
            break;
        }
    }

    //upiti se ubacuju u listu i nakon toga se poziva metoda za izvršenje upita
    if (!upitIspodCursora.isEmpty())
        executeSqlQuery(QStringList() << upitIspodCursora, activeConnection());
    //![2]
}

void FrmMainWindow::on_actionExecute_Explain_Query_Plan_triggered()
{
    QSqlDatabase db = QSqlDatabase::database(activeConnection());

    if (db.isOpen())
    {
        SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

        QTextCursor cursor = sqlEditor->textCursor(); //interesuje me samo selektovan tekst

        QString sqlEditorText;

        if (!cursor.selectedText().isEmpty()) //selekcija nije prazna
            sqlEditorText = cursor.selection().toPlainText();
        else
            sqlEditorText = sqlEditor->toPlainText();

        //validacija stringa za izvršavanje upita
        QueryValidator validator;
        sqlEditorText = validator.removeUnneededContent(sqlEditorText);
        QStringList queryList = validator.parseToQuerys(sqlEditorText, true);

        executeSqlQuery(queryList, activeConnection());
    }
}

void FrmMainWindow::on_actionStop_Query_triggered()
{
    if (ui->actionExecute_Query->isEnabled() == false)
        queryExecution->executeStopRequest();
}

void FrmMainWindow::on_actionExport_Schema_triggered()
{
    QString exportPath = QFileDialog::getSaveFileName(this, tr("Export Schema"), path, tr("Sql Files(*.sql)"));

    if (!exportPath.isEmpty())
    {
        if (QFileInfo(exportPath).suffix() != "sql")
            exportPath.append(".sql");

        QFile file(exportPath);

        if (!file.open(QFile::WriteOnly| QFile::Text))
            QMessageBox::warning(this,tr("Message"), tr("File can not be saved on selected path."), QMessageBox::Ok);

        QApplication::setOverrideCursor(Qt::WaitCursor);

        QTextStream stream(&file);

        QSqlDatabase db = QSqlDatabase::database(activeConnection());

        if (db.isOpen())
        {
            QSqlQuery upit(db);
            upit.prepare("select sql from sqlite_master where sql not null");
            upit.exec();

            while (upit.next())
                stream << upit.value(0).toString() << ";" << endl;
        }
        file.close();

        setPath(QFileInfo(exportPath).path());

        QApplication::setOverrideCursor(Qt::ArrowCursor);
    }
}

void FrmMainWindow::on_actionDump_Database_triggered()
{
    QString dumpPath = QFileDialog::getSaveFileName(this, tr("Dump Database"), path, tr("Sql Files(*.sql)"));

    if (!dumpPath.isEmpty())
    {
        if (QFileInfo(dumpPath).suffix() != "sql")
            dumpPath.append(".sql");

        QFile file(dumpPath);

        if (!file.open(QFile::WriteOnly| QFile::Text))
            QMessageBox::warning(this,tr("Message"), tr("File can not be saved on selected path."), QMessageBox::Ok);

        QApplication::setOverrideCursor(Qt::WaitCursor);

        QTextStream stream(&file);

        stream << "PRAGMA foreign_keys=OFF;" << endl;

        QSqlDatabase db = QSqlDatabase::database(activeConnection());

        if (db.isOpen())
        {
            //![1] --čitanje tabela iz baze
            QSqlQuery upit(db);
            upit.prepare("select name, sql from sqlite_master where type = 'table'");
            upit.exec();

            QStringList tabels;

            while (upit.next())
            {
                stream << upit.value(1).toString() << ";" << endl;
                tabels.append(upit.value(0).toString());
            }
            //![1]

            //![2] --čitanje views
            upit.prepare("select sql from sqlite_master where type = 'view'");
            upit.exec();

            while (upit.next())
                stream << upit.value(0).toString() << ";" << endl;
            //![2]

            //![3] --čitanje triggers
            upit.prepare("select sql from sqlite_master where type = 'trigger'");
            upit.exec();

            while (upit.next())
                stream << upit.value(0).toString() << ";" << endl;
            //![3]

            //![4] --čitanje triggers
            upit.prepare("select sql from sqlite_master where type = 'index' and sql not null");
            upit.exec();

            while (upit.next())
                stream << upit.value(0).toString() << ";" << endl;
            //![4]

            //![5] --čitanje podataka iz tabela
            foreach (QString str, tabels)
            {
                if (str == "sqlite_sequence") //ova tabela služi za čuvanje podataka o auto increment
                    stream << "DELETE FROM sqlite_sequence";

                upit.prepare(QString("select * from %1").arg(str));
                upit.exec();

                QSqlRecord record = upit.record();

                while(upit.next())
                {
                    stream << QString("INSERT INTO %1 VALUES(").arg(str);

                    for (int count = 0; count < record.count(); count++)
                    {
                        if (count > 0 && count < record.count())
                            stream << ",";

                        if (upit.value(count).isNull())
                        {//vrednost je null
                            stream << "NULL";
                        }
                        else if (upit.value(count).type() == QVariant::ByteArray)
                        {//vrednost je ili broj ili string ili datum
                            stream << "X'";
                            stream << upit.value(count).toByteArray().toHex().toUpper();
                            stream << "'";
                        }
                        else
                        {
                            QString value = upit.value(count).toString();

                            bool ok;
                            bool konvertovano = false;

                            //konvertuj i intiger
                            value.toInt(&ok);

                            //ako je konverzija uspešna dodaj u values
                            if (ok)
                            {
                                stream << value;
                                konvertovano = true;
                            }

                            //ako je konvertovanje neuspešno onda probaj da konvertuješ u double
                            if (konvertovano == false)
                            {
                                value.toDouble(&ok);

                                if (ok)//konvertovanje uspelo
                                {
                                    stream << value;
                                    konvertovano = true;
                                }
                            }

                            //ako konvetovanje nije uspelo ni za INT ni za DOUBLE onda je string
                            if (konvertovano == false)
                                stream << "\"" + value + "\"";
                        }
                    }

                    stream << ");" << endl;
                }
            }
            //![5]
        }

        file.close();

        setPath(QFileInfo(dumpPath).path());

        QApplication::setOverrideCursor(Qt::ArrowCursor);
    }
}

void FrmMainWindow::on_actionAnalyze_Statistics_triggered()
{
    FrmAnalyze *frm = new FrmAnalyze(activeConnection(), this);
    connect(frm,SIGNAL(executeSqlQuery(QStringList,QString)),this,SLOT(executeSqlQuery(QStringList,QString)));
    connect(frm,SIGNAL(finished(bool)),ui->actionAnalyze_Statistics,SLOT(setEnabled(bool)));
    frm->show();
    ui->actionAnalyze_Statistics->setEnabled(false);
}

void FrmMainWindow::on_actionVacuum_triggered()
{
    FrmVacuum *frm = new FrmVacuum(activeConnection(), this);
    connect(frm,SIGNAL(executeSqlQuery(QStringList,QString)),this,SLOT(executeSqlQuery(QStringList,QString)));
    connect(frm,SIGNAL(finished(bool)),ui->actionVacuum,SLOT(setEnabled(bool)));
    frm->show();
    ui->actionVacuum->setEnabled(false);
}

void FrmMainWindow::on_actionAttach_Database_triggered()
{
    FrmAttachDatabase *frm = new FrmAttachDatabase(path, activeConnection(), this);
    connect(frm,SIGNAL(executeSqlQuery(QStringList,QString)),this,SLOT(executeSqlQuery(QStringList,QString)));
    connect(frm,SIGNAL(finished(bool)),ui->actionAttach_Database,SLOT(setEnabled(bool)));
    frm->show();
    ui->actionAttach_Database->setEnabled(false);
}

void FrmMainWindow::on_actionPragma_Statements_triggered()
{
    FrmPragmas *frm = new FrmPragmas(activeConnection(), this);
    connect(frm,SIGNAL(finished()),frm,SLOT(deleteLater()));
    frm->show();
}

void FrmMainWindow::on_actionOptions_triggered()
{
    FrmOptions *frm = new FrmOptions(this);
    connect(frm, SIGNAL(optionsIzmena()),this,SLOT(optionsIzmena()));
    frm->exec();
    frm->deleteLater();
}

void FrmMainWindow::on_actionReset_Layout_triggered()
{
    ui->splitterDatabaseOdFieldInformation->setSizes(QList<int>() << 450 << 180);
    ui->splitterQueryOdTblOdMessage->setSizes(QList<int>() << 250 << 200 << 100);
    ui->splitterLeveOdDesneStrane->setSizes(QList<int>() << 100 << 550);
}

void FrmMainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl(QUrl("http://karoljkocmaros.blogspot.com/KoSQLite"));
}

void FrmMainWindow::on_actionCheck_For_Updates_triggered()
{
    loading = false;
    d->checkUpdate();
    ui->actionCheck_For_Updates->setEnabled(false);
}

void FrmMainWindow::on_actionAbout_KoSQLite_triggered()
{
    FrmAbout *frm = new FrmAbout(this);
    frm->exec();
    frm->deleteLater();
}

void FrmMainWindow::on_actionReload_triggered()
{
    //snimanje trenutno stanja treeWidget šta je expandovano a šta nije
    bool reload = false;

    QTreeWidget *treeDatabase = activeTreeDatabase(ui->tabDatabase->currentIndex());
    QStringList databasesExpanded;
    QStringList tablesExpanded;
    ///[1] --snimanje otvorenih itema
    int mainItem = treeDatabase->topLevelItemCount();

    //snimanje trenutnog stanja treeWidgeta,
    if (ui->tabDatabase->currentIndex() >= 0)
    {
        for (int i = 0; i < mainItem; i++)
        {
            QTreeWidgetItem *topLevelItem = treeDatabase->topLevelItem(i);
            if (topLevelItem->isExpanded())
            {
                databasesExpanded << topLevelItem->text(0);
                for (int j = 0; j < topLevelItem->childCount(); j++)
                {
                    if (topLevelItem->child(j)->isExpanded())
                    {
                        tablesExpanded << topLevelItem->child(j)->text(0) + topLevelItem->text(0);
                        for(int k = 0; k < topLevelItem->child(j)->childCount(); k++)
                        {
                            if (topLevelItem->child(j)->child(k)->isExpanded())
                            {
                                tablesExpanded << topLevelItem->child(j)->child(k)->text(0) + topLevelItem->child(j)->text(0) + topLevelItem->text(0);
                                for (int l = 0; l < topLevelItem->child(j)->child(k)->childCount(); l++)
                                {
                                    if (topLevelItem->child(j)->child(k)->child(l)->isExpanded())
                                        tablesExpanded << topLevelItem->child(j)->child(k)->child(l)->text(0) + topLevelItem->child(j)->child(k)->text(0) + topLevelItem->child(j)->text(0) + topLevelItem->text(0);
                                }
                            }
                        }
                    }
                }
            }
        }
        reload = true;
    }

    //učitavanje treeWidgetItem-a
    QSqlDatabase db = QSqlDatabase::database(activeConnection());

    if (db.isOpen())
    {
        QTreeWidget *treeDatabase = activeTreeDatabase(ui->tabDatabase->currentIndex());
        //pre nego što se dodaju item-i, drvo se očisti
        treeDatabase->clear();

        //ispitujem sve baze koje su nakačene na konekciju, pa ih redom dodajem
        QSqlQuery upit(db);
        upit.prepare("pragma database_list;");
        upit.exec();

        while(upit.next())
        {
            //!ovo je id baze dodat u konekciju, auto increment
            //upit.value(0).toString();
            //!ovo je alias baze, prva baza je uvek main, a ostale moraju biti nešto drugo
            //upit.value(1).toString();
            //!ovo je putanja ka bazi
            //upit.value(2).toString();

            //preskoči temp bazu
            if (upit.value(1).toString() == "temp")
                continue;

            QTreeWidgetItem *itemDatabase = addDatabase(treeDatabase, upit.value(1).toString());

            //![0] --učitavanje tabela, kolona u tabeli, triggera, indexa i sistemskih indexa
            //čitanje tabela iz svake baze
            QSqlQuery upitTables(db);
            upitTables.exec(QString("select tbl_name from %1.sqlite_master where type='table' and (tbl_name != 'sqlite_sequence' and tbl_name != 'sqlite_stat1' and tbl_name != 'sqlite_stat2' and tbl_name != 'sqlite_stat3' and tbl_name !=  'sqlite_stat4') order by tbl_name;").arg(upit.value(1).toString()));
            while (upitTables.next())
            {
                QTreeWidgetItem *itemTable = addTable(itemDatabase->child(0), upitTables.value(0).toString());

                //kolone u tabeli
                QSqlQuery upitTableInfo(db);
                upitTableInfo.exec(QString("pragma table_info('%1');").arg(upitTables.value(0).toString()));
                while (upitTableInfo.next())
                    addField(itemTable->child(0), upitTableInfo.value(1).toString(), upitTableInfo.value(5).toInt());

                //expandujem item Tables, jer će tek sada imati tabele u sebi
                itemTable->parent()->setExpanded(true);

                //indexi i triggeri
                QSqlQuery upitTriggerIndex(db);
                upitTriggerIndex.exec(QString("select * from %1.sqlite_master where (type = 'trigger' or type = 'index') and tbl_name = '%2';").arg(upit.value(1).toString()).arg(upitTables.value(0).toString()));
                while (upitTriggerIndex.next())
                {
                    //da li je trigger u pitanju
                    if (upitTriggerIndex.value(0).toString() == "trigger")
                    {
                        addIndexTrigger(itemTable->child(3), upitTriggerIndex.value(1).toString());
                    }
                    else //onda je index u pitanju
                    {
                        if (upitTriggerIndex.value(4).toString() == "")//onda ide u Indexes nije null
                            addIndexTrigger(itemTable->child(2), upitTriggerIndex.value(1).toString());
                        else
                            addIndexTrigger(itemTable->child(1), upitTriggerIndex.value(1).toString());
                    }
                }
            }
            //![0]

            //![1] --učitavanje pogleda, i triggera na pogled
            QSqlQuery upitViews(db);
            upitViews.exec(QString("select tbl_name from %1.sqlite_master where type='view' order by tbl_name;").arg(upit.value(1).toString()));
            while (upitViews.next())
            {
                QTreeWidgetItem *itemViews = addViews(itemDatabase->child(1), upitViews.value(0).toString());

                QSqlQuery upitViewTrigger(db);
                upitViewTrigger.exec(QString("select * from %1.sqlite_master where type = 'trigger' and tbl_name = '%2';").arg(upit.value(1).toString()).arg(upitViews.value(0).toString()));
                while(upitViewTrigger.next())
                    addViewTrigger(itemViews->child(0), upitViewTrigger.value(1).toString());

            }
            //![1]

            //![2] --učitavanje sistemskih tabela
            addSystemCatalogue(itemDatabase->child(2), "sqlite_master");

            QSqlQuery upitSystemCatalogue(db);
            upitSystemCatalogue.exec(QString("select tbl_name from %1.sqlite_master where tbl_name = 'sqlite_sequence' or tbl_name = 'sqlite_stat1' or tbl_name = 'sqlite_stat2' or tbl_name = 'sqlite_stat3' or tbl_name =  'sqlite_stat4' order by tbl_name;").arg(upit.value(1).toString()));
            while (upitSystemCatalogue.next())
            {
                addSystemCatalogue(itemDatabase->child(2), upitSystemCatalogue.value(0).toString());
            }
            //![2]

            treeDatabase->expandItem(itemDatabase);
        }
    }

    //učitavanje snimljenog stanja treeWidgeta
    if (reload == true)
    {
        mainItem = treeDatabase->topLevelItemCount();

        for (int i = 0; i < mainItem; i++)
        {
            QTreeWidgetItem *item = treeDatabase->topLevelItem(i);

            if (databasesExpanded.contains(item->text(0), Qt::CaseSensitive))
            {
                item->setExpanded(true);
                databasesExpanded.removeOne(item->text(0));

                for (int j = 0; j < item->childCount(); j++)
                {
                    if (tablesExpanded.contains(item->child(j)->text(0) + item->text(0), Qt::CaseSensitive))
                    {
                        item->child(j)->setExpanded(true);
                        tablesExpanded.removeOne(item->child(j)->text(0) + item->text(0));

                        for (int k = 0; k < item->child(j)->childCount(); k++)
                        {
                            if (tablesExpanded.contains(item->child(j)->child(k)->text(0) + item->child(j)->text(0) + item->text(0), Qt::CaseSensitive))
                            {
                                item->child(j)->child(k)->setExpanded(true);
                                tablesExpanded.removeOne(item->child(j)->child(k)->text(0) + item->child(j)->text(0) + item->text(0));

                                for (int l = 0; l < item->child(j)->child(k)->childCount(); l++)
                                {
                                    if (tablesExpanded.contains(item->child(j)->child(k)->child(l)->text(0) + item->child(j)->child(k)->text(0) + item->child(j)->text(0) + item->text(0), Qt::CaseSensitive))
                                    {
                                        item->child(j)->child(k)->child(l)->setExpanded(true);
                                        tablesExpanded.removeOne(item->child(j)->child(k)->child(l)->text(0) + item->child(j)->child(k)->text(0) + item->child(j)->text(0) + item->text(0));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void FrmMainWindow::on_tabDatabase_tabCloseRequested(int index)
{
    //ukoliko se upit izvršava na zasebnoj niti proveriti da li je tab baze koji korisnik hoće da zatvori
    //tab baze nad kojim se izvršava upit. ukoliko jeste ne radi ništa.
    if (queryThread != NULL)
        if (queryThread.data()->objectName() == dbConnections.value(activeTreeDatabase(index)->objectName()))
            return;

    QString key = activeTreeDatabase(index)->objectName();

    ui->tabDatabase->widget(index)->deleteLater();
    ui->tabDatabase->removeTab(index);//živi u memoriji, do sledećeg kruga u loop event

    closeDatabase(dbConnections.value(key));
}

void FrmMainWindow::btnPrevious_clicked(QString find, QTextDocument::FindFlags findFlags)
{
    SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

    if (sqlEditor->find(find, findFlags) == false)
    {
        sqlEditor->moveCursor(QTextCursor::End);
        sqlEditor->find(find, QTextDocument::FindBackward);
    }
}

void FrmMainWindow::btnNext_clicked(QString find, QTextDocument::FindFlags findFlags)
{
    SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

    if (sqlEditor->find(find, findFlags) == false)
    {
        sqlEditor->moveCursor(QTextCursor::Start);
        sqlEditor->find(find);
    }
}

void FrmMainWindow::btnReplace_clicked(QString find, QString replace, QTextDocument::FindFlags findFlags)
{
    SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());

    QTextCursor sqlCursor = sqlEditor->textCursor();

    if (sqlCursor.hasSelection())
        sqlCursor.insertText(replace);

    sqlEditor->moveCursor(QTextCursor::NextWord);

    if (sqlEditor->find(find,findFlags) == false)
    {
        sqlEditor->moveCursor(QTextCursor::Start);
        sqlEditor->moveCursor(QTextCursor::NextWord);
        sqlEditor->find(find,findFlags);
    }
}

void FrmMainWindow::btnReplaceAll_clicked(QString find, QString replace, QTextDocument::FindFlags findFlags)
{
    SqlEditor *sqlEditor = activeSqlEditor(tabQuery->currentIndex());
    sqlEditor->moveCursor(QTextCursor::Start);

    QTextCursor textCursor = sqlEditor->textCursor();

    textCursor.beginEditBlock();//obeležavanje undo-a

    while(sqlEditor->find(find, findFlags))
    {
        QTextCursor sqlCursor = sqlEditor->textCursor();

        if (sqlCursor.hasSelection())
            sqlCursor.insertText(replace);

        if (sqlEditor->textCursor().atEnd())
            break;
    }

    textCursor.endEditBlock();
}

void FrmMainWindow::tabQuery_currentChanged(int index)
{
    SqlEditor *sqlEditor = activeSqlEditor(index);
    sqlEditor->setFocus();

    bool hasSelection = (sqlEditor && (sqlEditor->textCursor().hasSelection()));
    ui->actionCopy->setEnabled(hasSelection);
    ui->actionCut->setEnabled(hasSelection);
    ui->actionDelete->setEnabled(hasSelection);

    ui->actionUndo->setEnabled(sqlEditor->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(sqlEditor->document()->isRedoAvailable());
}

void FrmMainWindow::tabQuery_tabCloseRequested(int index)
{
    SqlEditor *sqlEditor = activeSqlEditor(index);
    QString tab = sqlFiles.value(tabQuery->widget(index)->objectName()).getFileName();

    tabQuery->blockSignals(true);

    if (confirmClosing == true)
    {
        //nije snimljeno da li da se snimi pre no što se zatvori
        if (sqlEditor->document()->isModified())
        {
            switch (QMessageBox::information(this,tr("Message"), tr("Save changes to document %1 before closing?").arg(tab), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel))
            {
            case QMessageBox::Save:
                on_actionSave_SQL_Script_As_triggered();
                sqlFiles.remove(tabQuery->widget(index)->objectName());
                tabQuery->widget(index)->deleteLater();
                tabQuery->removeTab(index);//ovo sklanja widget is tabwidget-a ali još i dalje živi u memoriji
                break;
            case QMessageBox::Discard:
                sqlFiles.remove(tabQuery->widget(index)->objectName());
                tabQuery->widget(index)->deleteLater();
                tabQuery->removeTab(index);//ovo sklanja widget is tabwidget-a ali još i dalje živi u memoriji
                break;
            default:
                //ovo je tu samo da zavara warning
                break;
            }
        }
        else
        {
            sqlFiles.remove(tabQuery->widget(tabQuery->currentIndex())->objectName());
            tabQuery->widget(index)->deleteLater();
            tabQuery->removeTab(index);
        }
    }
    else
    {
        sqlFiles.remove(tabQuery->widget(tabQuery->currentIndex())->objectName());
        tabQuery->widget(index)->deleteLater();
        tabQuery->removeTab(index);
    }

    tabQuery->blockSignals(false);

    if (tabQuery->count() == 0)
        on_actionNew_Query_Tab_triggered();
}

void FrmMainWindow::tabQuery_middleClicked(int tabIndex)
{
    if (tabIndex > -1)
    {
        indexQueryClicked = tabIndex;
        actionCloneTab_triggered();
    }
}

void FrmMainWindow::on_tabResult_tabCloseRequested(int index)
{
    ui->tabResult->widget(index)->deleteLater();
    ui->tabResult->removeTab(index);
}

void FrmMainWindow::openDatabase(QString database)
{
    ui->tabDatabase->blockSignals(true);

    //ukoliko je baza već učitana onda se ona ne može ponovo učitati,
    //doći će preklapanja konektora i stari će se obrisati a nov dodati (pod istim imenom)
    bool isLoaded = false;
    foreach (QString key, dbConnections.keys())
    {
        if (dbConnections.value(key) == database)
        {
            isLoaded = true;
            break;//nema potrebe da pretrčava kroz ostale
        }
    }

    if (isLoaded == false)
    {
        brTabovaDatabase++;

        QWidget *widget = new QWidget(ui->tabDatabase);
        widget->setObjectName(QString("Database %1").arg(brTabovaDatabase));

        ui->tabDatabase->addTab(widget,QIcon(":/icons/icons/database.png"),  QFileInfo(database).fileName());
        ui->tabDatabase->tabBar()->setToolTip(database);

        QVBoxLayout *layout = new QVBoxLayout(widget);
        QTreeWidget *treeDatabase = new QTreeWidget(widget);
        layout->addWidget(treeDatabase);
        treeDatabase->setObjectName(QString("Database %1").arg(brTabovaDatabase));

        treeDatabase->setColumnCount(1);
        treeDatabase->setHeaderLabels(QStringList() << tr("Database"));
        treeDatabase->setContextMenuPolicy(Qt::CustomContextMenu);
        treeDatabase->setItemsExpandable(true);

        connect(treeDatabase,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(treeDatabase_customContextMenuRequested(QPoint)));
        connect(treeDatabase,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(treeDatabase_itemClicked(QTreeWidgetItem*,int)));

        //ime konekcija ka bazi je njena putanja. otvaranje konekcije ka bazi podataka
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", database);
        db.setDatabaseName(database);
        db.open();

        //u ovoj mapi se čuvaju sve konekcije, a kljuc je ime objekta treeDatabase,
        //zbog toga što se tabovi mogu presortirati
        dbConnections.insert(QString("Database %1").arg(brTabovaDatabase), database);

        ui->tabDatabase->setCurrentIndex(ui->tabDatabase->count() - 1);

        on_actionReload_triggered();//učatavanje baze!!!

        inicijalizacijaSystemMenija();
    }
    else
        QMessageBox::warning(this, tr("Message"), tr("Database %1 is already opened").arg(database), QMessageBox::Ok);

    ui->tabDatabase->blockSignals(false);
}

void FrmMainWindow::closeDatabase(QString connection)
{
    {
        QSqlDatabase db = QSqlDatabase::database(connection);
        db.close();
    }

    QSqlDatabase::removeDatabase(connection);

    dbConnections.remove(dbConnections.key(connection));
}

void FrmMainWindow::treeDatabase_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)

    if (item->parent() != false)//database
    {
        if (item->parent()->parent() != false)//table
        {
            if (item->parent()->parent()->parent() != false)
            {
                //proveravam da li je to field, i proveravam da li mu je roditelj na prvom mestu, jer je u tom slučaju Columns
                if (item->parent()->parent()->parent()->parent() != false && item->parent()->parent()->indexOfChild(item->parent()) == 0)
                {
                    QSqlDatabase db = QSqlDatabase::database(activeConnection());
                    if (db.isOpen())
                    {
                        QSqlQuery upit(db);
                        upit.exec(QString("pragma table_info('%1');").arg(item->parent()->parent()->text(0)));

                        while(upit.next())
                        {
                            if (upit.value(1).toString() == item->text(0))
                            {
                                //field , type, null, default
                                lblFieldValue->setText(upit.value(1).toString());
                                lblTypeValue->setText(upit.value(2).toString());
                                lblNullValue->setText(upit.value(3).toInt() == 0 ? "Null" : "Not Null");
                                lblDefaultValue->setText(upit.value(4).toString());
                            }
                        }
                    }
                }
            }
        }
    }
}

void FrmMainWindow::lblFieldValue_doubleClicked()
{
    lblFieldValue->setSelection(0, lblFieldValue->text().length());
}

void FrmMainWindow::lblTypeValue_doubleClicked()
{
    lblTypeValue->setSelection(0, lblTypeValue->text().length());
}

void FrmMainWindow::lblNullValue_doubleClicked()
{
    lblNullValue->setSelection(0, lblNullValue->text().length());
}

void FrmMainWindow::lblDefaultValue_doubleClicked()
{
    lblDefaultValue->setSelection(0, lblDefaultValue->text().length());
}

void FrmMainWindow::clipbord_dataChanged()
{//ovo radi u qt4 kada se u klipper pročisti istorijat a u qt5 ne radi
    ui->actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void FrmMainWindow::activeCut()
{
    activeSqlEditor(tabQuery->currentIndex())->cut();
}

void FrmMainWindow::activeCopy()
{
    activeSqlEditor(tabQuery->currentIndex())->copy();
}

void FrmMainWindow::activePaste()
{
    activeSqlEditor(tabQuery->currentIndex())->paste();
}

void FrmMainWindow::activeDelete()
{
    activeSqlEditor(tabQuery->currentIndex())->textCursor().removeSelectedText();
}

void FrmMainWindow::activeUndo()
{
    activeSqlEditor(tabQuery->currentIndex())->document()->undo();
}

void FrmMainWindow::activeRedo()
{
    activeSqlEditor(tabQuery->currentIndex())->document()->redo();
}

void FrmMainWindow::txtFindLineEdit_textChanged(QString searchString, QTextDocument::FindFlags findFlags)
{
    for (int i = 0; i < tabQuery->count(); i++)
    {
        SqlEditor *sqlEditor = activeSqlEditor(i);

        QTextCursor tc = sqlEditor->textCursor();//ovo je tu da bi na kraju vratio kursor gde je kucano

        sqlEditor->moveCursor(QTextCursor::Start);//od kraja kreće samo zato da bi prvi nađen bio selektovan a ne poslednji

        QList<QTextEdit::ExtraSelection> extraSelections;
        QColor color = QColor(Qt::yellow);

        while(sqlEditor->find(searchString, findFlags))
        {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(color);

            extra.cursor = sqlEditor->textCursor();
            extraSelections.append(extra);
        }

        sqlEditor->setExtraSelections(extraSelections);

        sqlEditor->setTextCursor(tc);
    }
}

void FrmMainWindow::sqlEditor_modificationChanged(bool state)
{
    if (state == true)
    {
        if (!sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName()).getFileName().isEmpty())
            tabQuery->setTabText(tabQuery->currentIndex(),sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName()).getFileName() + "*");
    }
    else if (state == false)
    {
        if (!sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName()).getFileName().isEmpty())
            tabQuery->setTabText(tabQuery->currentIndex(),sqlFiles.value(tabQuery->widget(tabQuery->currentIndex())->objectName()).getFileName());
    }
}

void FrmMainWindow::queryOutput(QString vreme, QString elapsedTime, QString lastQuery, QString databaseText)
{
    int row = ui->tblOutput->rowCount();

    ui->tblOutput->insertRow(row);

    ui->tblOutput->setItem(row, 0, new QTableWidgetItem(QString::number(row+1)));
    ui->tblOutput->setItem(row, 1, new QTableWidgetItem(vreme));
    ui->tblOutput->setItem(row, 2, new QTableWidgetItem(elapsedTime));
    ui->tblOutput->setItem(row, 3, new QTableWidgetItem(lastQuery));
    ui->tblOutput->setItem(row, 4, new QTableWidgetItem(databaseText));

    ui->tblOutput->item(row, 3)->setToolTip(lastQuery);
    ui->tblOutput->item(row, 4)->setToolTip(databaseText);

    ui->tblOutput->scrollToBottom();

    for (int i = 0; i < 5; i++)
        ui->tblOutput->item(row, i)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void FrmMainWindow::queryResult(QSqlQuery result)
{
    if (result.isSelect())
    {
        ui->tabResult->blockSignals(true);

        brTabovaResult++;

        QWidget *widget = new QWidget(ui->tabResult);
        widget->setObjectName(QString("result%1").arg(brTabovaResult));
        ui->tabResult->addTab(widget, QString("Result %1").arg(brTabovaResult));

        QVBoxLayout *layout = new QVBoxLayout(widget);
        ui->tabResult->widget(ui->tabResult->count() - 1)->setLayout(layout);

        QTableView *tblView = new QTableView(widget);
        KoSqlQueryModel *model = new KoSqlQueryModel(widget);
        model->setQuery(result);

        tblView->setObjectName(QString("result%1").arg(brTabovaResult));
        tblView->setContextMenuPolicy(Qt::CustomContextMenu);
        tblView->verticalHeader()->setDefaultSectionSize(20);
        tblView->setAlternatingRowColors(true);
        tblView->setModel(model);

        layout->addWidget(tblView);
        tblView->resizeColumnsToContents();

        connect(tblView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(tblResult_customContextMenuRequested(QPoint)));

        ui->tabResult->blockSignals(false);
    }
}

void FrmMainWindow::tabQuery_customContextMenuRequested(const QPoint pos)
{
    int tabIndex = tabQuery->tabBar()->tabAt(pos);

    if (tabIndex > -1)
    {
        indexQueryClicked = tabIndex;
        menuTabQuery->exec(QCursor::pos());
    }
}

void FrmMainWindow::tblOutput_customContextMenuRequested(const QPoint pos)
{
    if (ui->tblOutput->indexAt(pos).isValid()) //ovo bi bilo dobro, ali ostaje prethodno kliknuti index
    {
        menuTblOutput->exec(QCursor::pos());
    }
}

void FrmMainWindow::sqlEditor_customContextMenuRequested(const QPoint)
{
    menuSqlEditor->exec(QCursor::pos());
}

void FrmMainWindow::treeDatabase_customContextMenuRequested(const QPoint pos)
{
    QTreeWidget *treeDatabase = activeTreeDatabase(ui->tabDatabase->currentIndex());

    if (treeDatabase->itemAt(pos) != NULL)
    {
        QTreeWidgetItem *itemPos = treeDatabase->itemAt(pos);

        itemRightClicked = itemPos;

        if (itemPos->parent() != NULL)
        {
            if (itemPos->parent()->parent() != NULL)
            {
                if (itemPos->parent()->parent()->parent() != NULL)
                {
                    if (itemPos->parent()->parent()->parent()->parent() != NULL)
                    {
                        if (itemPos->parent()->parent()->parent()->parent()->parent() == NULL)
                        {
                            if (itemPos->parent()->text(0) == "Indexes")
                            {
                                actionCreateTable->setVisible(false);
                                actionDescribeTable->setVisible(false);
                                actionAlterTable->setVisible(false);
                                actionRenameTable->setVisible(false);
                                actionDropTable->setVisible(false);
                                actionImportData->setVisible(false);
                                actionEditData->setVisible(false);
                                actionCreateView->setVisible(false);
                                actionCreateIndex->setVisible(false);
                                actionCreateTrigger->setVisible(false);
                                actionDescribeView->setVisible(false);
                                actionAlterView->setVisible(false);
                                actionDropView->setVisible(false);
                                actionDescribeTrigger->setVisible(false);
                                actionAlterTrigger->setVisible(false);
                                actionDropTrigger->setVisible(false);
                                actionDescribeIndex->setVisible(true);
                                actionReIndex->setVisible(true);
                                actionDropIndex->setVisible(true);
                                actionDetachDatabase->setVisible(false);
                            }
                            else if (itemPos->parent()->text(0) == "Triggers")
                            {
                                actionCreateTable->setVisible(false);
                                actionDescribeTable->setVisible(false);
                                actionAlterTable->setVisible(false);
                                actionRenameTable->setVisible(false);
                                actionDropTable->setVisible(false);
                                actionImportData->setVisible(false);
                                actionEditData->setVisible(false);
                                actionCreateView->setVisible(false);
                                actionCreateIndex->setVisible(false);
                                actionCreateTrigger->setVisible(false);
                                actionDescribeView->setVisible(false);
                                actionAlterView->setVisible(false);
                                actionDropView->setVisible(false);
                                actionDescribeTrigger->setVisible(true);
                                actionAlterTrigger->setVisible(true);
                                actionDropTrigger->setVisible(true);
                                actionDescribeIndex->setVisible(false);
                                actionReIndex->setVisible(false);
                                actionDropIndex->setVisible(false);
                                actionDetachDatabase->setVisible(false);
                            }
                            else
                            {
                                actionCreateTable->setVisible(false);
                                actionDescribeTable->setVisible(false);
                                actionAlterTable->setVisible(false);
                                actionRenameTable->setVisible(false);
                                actionDropTable->setVisible(false);
                                actionImportData->setVisible(false);
                                actionEditData->setVisible(false);
                                actionCreateView->setVisible(false);
                                actionCreateIndex->setVisible(false);
                                actionCreateTrigger->setVisible(false);
                                actionDescribeView->setVisible(false);
                                actionAlterView->setVisible(false);
                                actionDropView->setVisible(false);
                                actionDescribeTrigger->setVisible(false);
                                actionAlterTrigger->setVisible(false);
                                actionDropTrigger->setVisible(false);
                                actionDescribeIndex->setVisible(false);
                                actionReIndex->setVisible(false);
                                actionDropIndex->setVisible(false);
                                actionDetachDatabase->setVisible(false);
                            }
                        }
                    }
                    else
                    {
                        //nema roditelja verovatno je Columns, Indexes, System Index, Trigger (na tabelu i na pogled)
                        if (itemPos->text(0) == "Indexes")
                        {
                            actionCreateTable->setVisible(false);
                            actionDescribeTable->setVisible(false);
                            actionAlterTable->setVisible(false);
                            actionRenameTable->setVisible(false);
                            actionDropTable->setVisible(false);
                            actionImportData->setVisible(false);
                            actionEditData->setVisible(false);
                            actionCreateView->setVisible(false);
                            actionCreateIndex->setVisible(true);
                            actionCreateTrigger->setVisible(false);
                            actionDescribeView->setVisible(false);
                            actionAlterView->setVisible(false);
                            actionDropView->setVisible(false);
                            actionDescribeTrigger->setVisible(false);
                            actionAlterTrigger->setVisible(false);
                            actionDropTrigger->setVisible(false);
                            actionDescribeIndex->setVisible(false);
                            actionReIndex->setVisible(false);
                            actionDropIndex->setVisible(false);
                            actionDetachDatabase->setVisible(false);
                        }
                        else if (itemPos->text(0) == "Triggers")
                        {
                            actionCreateTable->setVisible(false);
                            actionDescribeTable->setVisible(false);
                            actionAlterTable->setVisible(false);
                            actionRenameTable->setVisible(false);
                            actionDropTable->setVisible(false);
                            actionImportData->setVisible(false);
                            actionEditData->setVisible(false);
                            actionCreateView->setVisible(false);
                            actionCreateIndex->setVisible(false);
                            actionCreateTrigger->setVisible(true);
                            actionDescribeView->setVisible(false);
                            actionAlterView->setVisible(false);
                            actionDropView->setVisible(false);
                            actionDescribeTrigger->setVisible(false);
                            actionAlterTrigger->setVisible(false);
                            actionDropTrigger->setVisible(false);
                            actionDescribeIndex->setVisible(false);
                            actionReIndex->setVisible(false);
                            actionDropIndex->setVisible(false);
                            actionDetachDatabase->setVisible(false);
                        }
                        else
                        {//columns, system indexes
                            actionCreateTable->setVisible(false);
                            actionDescribeTable->setVisible(false);
                            actionAlterTable->setVisible(false);
                            actionRenameTable->setVisible(false);
                            actionDropTable->setVisible(false);
                            actionImportData->setVisible(false);
                            actionEditData->setVisible(false);
                            actionCreateView->setVisible(false);
                            actionCreateIndex->setVisible(false);
                            actionCreateTrigger->setVisible(false);
                            actionDescribeView->setVisible(false);
                            actionAlterView->setVisible(false);
                            actionDropView->setVisible(false);
                            actionDescribeTrigger->setVisible(false);
                            actionAlterTrigger->setVisible(false);
                            actionDropTrigger->setVisible(false);
                            actionDescribeIndex->setVisible(false);
                            actionReIndex->setVisible(false);
                            actionDropIndex->setVisible(false);
                            actionDetachDatabase->setVisible(false);
                        }
                    }
                }
                else
                {
                    //nema roditelja verovatno je tabela, pogled ili sistemska tabela
                    if (itemPos->parent()->text(0) == "Tables")
                    {
                        actionCreateTable->setVisible(false);
                        actionDescribeTable->setVisible(true);
                        actionAlterTable->setVisible(true);
                        actionRenameTable->setVisible(true);
                        actionDropTable->setVisible(true);
                        actionImportData->setVisible(true);
                        actionEditData->setVisible(true);
                        actionCreateView->setVisible(false);
                        actionCreateIndex->setVisible(false);
                        actionCreateTrigger->setVisible(false);
                        actionDescribeView->setVisible(false);
                        actionAlterView->setVisible(false);
                        actionDropView->setVisible(false);
                        actionDescribeTrigger->setVisible(false);
                        actionAlterTrigger->setVisible(false);
                        actionDropTrigger->setVisible(false);
                        actionDescribeIndex->setVisible(false);
                        actionReIndex->setVisible(false);
                        actionDropIndex->setVisible(false);
                        actionDetachDatabase->setVisible(false);
                    }
                    else if (itemPos->parent()->text(0) == "Views")
                    {
                        actionCreateTable->setVisible(false);
                        actionDescribeTable->setVisible(false);
                        actionAlterTable->setVisible(false);
                        actionRenameTable->setVisible(false);
                        actionDropTable->setVisible(false);
                        actionImportData->setVisible(false);
                        actionEditData->setVisible(false);
                        actionCreateView->setVisible(false);
                        actionCreateIndex->setVisible(false);
                        actionCreateTrigger->setVisible(false);
                        actionDescribeView->setVisible(true);
                        actionAlterView->setVisible(true);
                        actionDropView->setVisible(true);
                        actionDescribeTrigger->setVisible(false);
                        actionAlterTrigger->setVisible(false);
                        actionDropTrigger->setVisible(false);
                        actionDescribeIndex->setVisible(false);
                        actionReIndex->setVisible(false);
                        actionDropIndex->setVisible(false);
                        actionDetachDatabase->setVisible(false);
                    }
                    else if (itemPos->parent()->text(0) == "System Catalogue")
                    {
                        actionCreateTable->setVisible(false);
                        actionDescribeTable->setVisible(false);
                        actionAlterTable->setVisible(false);
                        actionRenameTable->setVisible(false);
                        actionDropTable->setVisible(false);
                        actionImportData->setVisible(false);
                        actionEditData->setVisible(false);
                        actionCreateView->setVisible(false);
                        actionCreateIndex->setVisible(false);
                        actionCreateTrigger->setVisible(false);
                        actionDescribeView->setVisible(false);
                        actionAlterView->setVisible(false);
                        actionDropView->setVisible(false);
                        actionDescribeTrigger->setVisible(false);
                        actionAlterTrigger->setVisible(false);
                        actionDropTrigger->setVisible(false);
                        actionDescribeIndex->setVisible(false);
                        actionReIndex->setVisible(false);
                        actionDropIndex->setVisible(false);
                        actionDetachDatabase->setVisible(false);
                    }
                }
            }
            else
            {//nema roditelja, verovatno je Tables, Views, System Catalogue
                if (itemPos->text(0) == "Tables")
                {
                    actionCreateTable->setVisible(true);
                    actionDescribeTable->setVisible(false);
                    actionAlterTable->setVisible(false);
                    actionRenameTable->setVisible(false);
                    actionDropTable->setVisible(false);
                    actionImportData->setVisible(false);
                    actionEditData->setVisible(false);
                    actionCreateView->setVisible(false);
                    actionCreateIndex->setVisible(false);
                    actionCreateTrigger->setVisible(false);
                    actionDescribeView->setVisible(false);
                    actionAlterView->setVisible(false);
                    actionDropView->setVisible(false);
                    actionDescribeTrigger->setVisible(false);
                    actionAlterTrigger->setVisible(false);
                    actionDropTrigger->setVisible(false);
                    actionDescribeIndex->setVisible(false);
                    actionReIndex->setVisible(false);
                    actionDropIndex->setVisible(false);
                    actionDetachDatabase->setVisible(false);
                }
                else if (itemPos->text(0) == "Views")
                {
                    actionCreateTable->setVisible(false);
                    actionDescribeTable->setVisible(false);
                    actionAlterTable->setVisible(false);
                    actionRenameTable->setVisible(false);
                    actionDropTable->setVisible(false);
                    actionImportData->setVisible(false);
                    actionEditData->setVisible(false);
                    actionCreateView->setVisible(true);
                    actionCreateIndex->setVisible(false);
                    actionCreateTrigger->setVisible(false);
                    actionDescribeView->setVisible(false);
                    actionAlterView->setVisible(false);
                    actionDropView->setVisible(false);
                    actionDescribeTrigger->setVisible(false);
                    actionAlterTrigger->setVisible(false);
                    actionDropTrigger->setVisible(false);
                    actionDescribeIndex->setVisible(false);
                    actionReIndex->setVisible(false);
                    actionDropIndex->setVisible(false);
                    actionDetachDatabase->setVisible(false);
                }
                else if (itemPos->text(0) == "System Catalogue")
                {
                    actionCreateTable->setVisible(false);
                    actionDescribeTable->setVisible(false);
                    actionAlterTable->setVisible(false);
                    actionRenameTable->setVisible(false);
                    actionDropTable->setVisible(false);
                    actionImportData->setVisible(false);
                    actionEditData->setVisible(false);
                    actionCreateView->setVisible(false);
                    actionCreateIndex->setVisible(false);
                    actionCreateTrigger->setVisible(false);
                    actionDescribeView->setVisible(false);
                    actionAlterView->setVisible(false);
                    actionDropView->setVisible(false);
                    actionDescribeTrigger->setVisible(false);
                    actionAlterTrigger->setVisible(false);
                    actionDropTrigger->setVisible(false);
                    actionDescribeIndex->setVisible(false);
                    actionReIndex->setVisible(false);
                    actionDropIndex->setVisible(false);
                    actionDetachDatabase->setVisible(false);
                }
            }
        }
        else
        {//item koji nema roditelja je BAZA

            actionCreateTable->setVisible(false);
            actionDescribeTable->setVisible(false);
            actionAlterTable->setVisible(false);
            actionRenameTable->setVisible(false);
            actionDropTable->setVisible(false);
            actionImportData->setVisible(false);
            actionEditData->setVisible(false);
            actionCreateView->setVisible(false);
            actionCreateIndex->setVisible(false);
            actionCreateTrigger->setVisible(false);
            actionDescribeView->setVisible(false);
            actionAlterView->setVisible(false);
            actionDropView->setVisible(false);
            actionDescribeTrigger->setVisible(false);
            actionAlterTrigger->setVisible(false);
            actionDropTrigger->setVisible(false);
            actionDescribeIndex->setVisible(false);
            actionReIndex->setVisible(false);
            actionDropIndex->setVisible(false);
            actionDetachDatabase->setVisible(itemPos->text(0) == "main" ? false : true);
        }

        menuTreeDatabase->exec(QCursor::pos());
    }
}

void FrmMainWindow::tblResult_customContextMenuRequested(const QPoint pos)
{
    QTableView *tableResult = activeResultTable(ui->tabResult->currentIndex());

    if (tableResult->indexAt(pos).isValid())
    {
        indexContextMenu = tableResult->indexAt(pos);

        //ako je BLOB u pitanju onda prikaži akciju za detalje u suprotnom sakrij
        if (tableResult->model()->data(tableResult->currentIndex(), Qt::EditRole).type() == QVariant::ByteArray)
            actionTRBlob->setVisible(true);
        else
            actionTRBlob->setVisible(false);

        menuTblResult->exec(QCursor::pos());
    }
}

void FrmMainWindow::on_tabResult_customContextMenuRequested(const QPoint pos)
{
    //pošto su tabovi dole, a tabAt peca tabove i dalje gore(zašto ne znam) ova ispod matematika je to prevazišla
    //10+10 je prestor margine u tabu ispod i iznad widget koji se nalazi u tabu
    //ostalo je height widgeta koji je u tabu tj. tabele
    // to se sve oduzme od trenutno pozicije
    if (ui->tabResult->count() > 0)
    {
        int tabIndex = ui->tabResult->tabBar()->tabAt(pos - QPoint(0, 10 + 10 + activeResultTable( ui->tabResult->currentIndex())->geometry().height() ) );

        if (tabIndex > -1)
        {
            //učitan je u globalnu promenljivu da znam na šta sam kliknuo
            indexResultClicked = tabIndex;
            menuTblResultTab->exec(QCursor::pos());
        }
    }
}

void FrmMainWindow::actionCloneTab_triggered()
{
    SqlEditor *sqlEditor = activeSqlEditor(indexQueryClicked);

    QString queryString = sqlEditor->document()->toPlainText();

    on_actionNew_Query_Tab_triggered();

    SqlEditor *sqlEditorClone = activeSqlEditor(tabQuery->count() - 1);

    sqlEditorClone->document()->setPlainText(queryString);
    sqlEditorClone->document()->setModified(false);

    indexQueryClicked = -1;
}

void FrmMainWindow::actionTOCopyMessage_triggered()
{
    QModelIndexList selektovaniRedovi = ui->tblOutput->selectionModel()->selectedRows();

    qStableSort(selektovaniRedovi.begin(), selektovaniRedovi.end());

    QString copy;

    foreach (QModelIndex index, selektovaniRedovi)
        copy += ui->tblOutput->item(index.row(), 4)->text() + "\n";

    copy = copy.remove(copy.lastIndexOf("\n"), copy.length());

    QApplication::clipboard()->setText(copy);
}

void FrmMainWindow::actionTOCopyAction_triggered()
{
    QModelIndexList selektovaniRedovi = ui->tblOutput->selectionModel()->selectedRows();

    qStableSort(selektovaniRedovi.begin(), selektovaniRedovi.end());

    QString copy;

    foreach (QModelIndex index, selektovaniRedovi)
        copy += ui->tblOutput->item(index.row(), 3)->text() + "\n";

    copy = copy.remove(copy.lastIndexOf("\n"), copy.length());

    QApplication::clipboard()->setText(copy);
}

void FrmMainWindow::actionTOCopy_triggered()
{
    QString strAll;

    QModelIndexList selektovaniRedovi = ui->tblOutput->selectionModel()->selectedRows();

    qStableSort(selektovaniRedovi.begin(), selektovaniRedovi.end());

    foreach (QModelIndex index, selektovaniRedovi)
    {
        QString str;

        for (int i = 0; i < ui->tblOutput->columnCount(); i++)
            str += ui->tblOutput->item(index.row(), i)->text() + "\t";

        str = str.remove(str.lastIndexOf("\t"), str.length());

        strAll += str + "\n";
    }

    strAll = strAll.remove(strAll.lastIndexOf("\n"), strAll.length());

    QApplication::clipboard()->setText(strAll);
}

void FrmMainWindow::actionTOClearOutput_triggered()
{
    for (int i = ui->tblOutput->rowCount(); i >= 0; i--)
        ui->tblOutput->removeRow(i);
}

void FrmMainWindow::actionTRCopyRow_triggered()
{
    // kopira se samo red
    QString str;

    QTableView *tableResult = activeResultTable(ui->tabResult->currentIndex());

    for (int i = 0; i < tableResult->model()->columnCount(); i++)
        str += tableResult->model()->data(tableResult->model()->index(indexContextMenu.row(), i), Qt::DisplayRole).toString() + "\t";

    str = str.remove(str.lastIndexOf("\t"), str.length());

    QApplication::clipboard()->setText(str);
}

void FrmMainWindow::actionTRCopyField_triggered()
{
    //kopira se samo ćelija
    QTableView *tableResult = activeResultTable(ui->tabResult->currentIndex());

    QString str = tableResult->model()->data(indexContextMenu, Qt::DisplayRole).toString();

    QApplication::clipboard()->setText(str);
}

void FrmMainWindow::actionTRCopy_triggered()
{
    //kopira se selekcija u tabeli
    QTableView *tableResult = activeResultTable(ui->tabResult->currentIndex());

    int ukupnoCelija = tableResult->model()->rowCount() * tableResult->model()->columnCount();

    QModelIndexList selektovaneFields = tableResult->selectionModel()->selectedIndexes();

    qStableSort(selektovaneFields.begin(), selektovaneFields.end());

    QString str;

    QModelIndex prethodni = selektovaneFields[0];

    if (ukupnoCelija == selektovaneFields.count())//ovo znači da je selektovana cela tabela, pa treba kopirati i hedere kolona
    {
        for (int i = 0; i < tableResult->model()->columnCount(); i++)
            str += tableResult->model()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString() + "\t";

        str = str.remove(str.lastIndexOf("\t"), str.length());

        str += "\n";
    }

    foreach (QModelIndex ind, selektovaneFields)
    {
        if (prethodni.row() != ind.row())
            str += "\n";

        str += tableResult->model()->data(ind, Qt::DisplayRole).toString() + "\t";

        prethodni = ind;
    }

    str = str.remove(str.lastIndexOf("\t"), str.length());

    QApplication::clipboard()->setText(str);
}

void FrmMainWindow::actionTRBlob_triggered()
{
    QTableView *tableResult = activeResultTable(ui->tabResult->currentIndex());

    QByteArray blob = tableResult->model()->data(indexContextMenu, Qt::EditRole).toByteArray();

    FrmBlobDetails *frm = new FrmBlobDetails(blob, path, this);
    connect(frm,SIGNAL(finished()),frm,SLOT(deleteLater()));
    connect(frm,SIGNAL(setPath(QString)),this,SLOT(setPath(QString)));
    frm->show();
}

void FrmMainWindow::actionCloseResultsOnRight_triggered()
{
    for (int i = ui->tabResult->count() - 1; i > indexResultClicked; i--)
        emit ui->tabResult->tabCloseRequested(i);
}

void FrmMainWindow::actionCloseAllResultsExceptThis_triggered()
{
    for (int i = ui->tabResult->count()-1; i >=0; i--)
    {
        if (i != indexResultClicked)
            emit ui->tabResult->tabCloseRequested(i);
    }
}

void FrmMainWindow::actionCloseAllResults_triggered()
{
    for (int i = ui->tabResult->count() - 1; i>= 0; i--)
        emit ui->tabResult->tabCloseRequested(i);
}

void FrmMainWindow::updateCheck()
{
    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("update");

    //konverzija verzije u broj radi uporedjivanja
    int currentVersion = QApplication::applicationVersion().remove('.').toInt();
    int checkVersion = QString::fromUtf8(d->downloadedDataFromUrl()).remove('.').toInt();

    //verzija 1.0.0 >> 100
    //verzija 0.0.1 >> 1

    if (loading == true)
    {
        if (settingsConfig.value("showUpdateInfo", false).toBool() == false)
        {
            if (currentVersion < checkVersion)
            {
                FrmUpdate *update = new FrmUpdate(true, this);
                update->show();
            }
        }
    }
    else
    {
        if (currentVersion < checkVersion)
        {
            FrmUpdate *update = new FrmUpdate(true, this);
            update->show();
        }
        else if (currentVersion == checkVersion)
        {
            FrmUpdate *update = new FrmUpdate(false, this);
            update->show();
        }
    }

    ui->actionCheck_For_Updates->setEnabled(true);

    settingsConfig.endGroup();
}

void FrmMainWindow::errorMessage(const QString &error)
{
    //na loadingu, ako je zahtevam da mi prikaže prilikom paljenja da li ima nove verzije, ukoliko nastane greška, grešku ne prikazujem
    if (loading == false)
        QMessageBox::critical(this, tr("Message"), error, QMessageBox::Ok);

    ui->actionCheck_For_Updates->setEnabled(true);
}

void FrmMainWindow::optionsIzmena()
{
    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    ///Enviroment

    ///query editor
    //Font
    settingsConfig.beginGroup("font");

    QFont font;
    font.setFamily(settingsConfig.value("family", "Sans Serif").toString());
    font.setPointSize(settingsConfig.value("size",9).toInt());

    settingsConfig.endGroup();
    //Text

    settingsConfig.beginGroup("text");
    bool wrapMode = settingsConfig.value("textWrapping", false).toBool();
    settingsConfig.endGroup();

    settingsConfig.beginGroup("Enviroment");
    autosaveTime = settingsConfig.value("autosave", 5).toInt();
    confirmClosing = settingsConfig.value("confirmClosing", true).toBool();
    bool showLineNumber = settingsConfig.value("showLineNumber", true).toBool();
    recentlyUsedDatabases = settingsConfig.value("recentlyUsedDatabases",5).toInt();
    settingsConfig.endGroup();

    inicijalizacijaRecentDatabases();

    autosave();

    for (int i = 0; i < tabQuery->count(); i++)
    {
        activeSqlEditor(i)->setFont(font);
        activeSqlEditor(i)->setWrapText(wrapMode);
        activeSqlEditor(i)->setShowLineNumber(showLineNumber);
    }

    //Highligher
    settingsConfig.beginGroup("highlighter");
    QList<QColor> colors;
    colors.append(settingsConfig.value("normalText", QColor(Qt::black)).value<QColor>());
    colors.append(settingsConfig.value("comment", QColor(Qt::gray)).value<QColor>());
    colors.append(settingsConfig.value("number", QColor(Qt::darkYellow)).value<QColor>());
    colors.append(settingsConfig.value("stringDouble", QColor(Qt::darkGreen)).value<QColor>());
    colors.append(settingsConfig.value("stringSingle", QColor(Qt::darkGreen)).value<QColor>());
    colors.append(settingsConfig.value("keywords", QColor(Qt::darkBlue)).value<QColor>());
    colors.append(settingsConfig.value("functions", QColor(Qt::blue)).value<QColor>());

    settingsConfig.endGroup();

    emit changeColors(colors);

    ///language --menja se prilikom restarta
}

void FrmMainWindow::openRecentDatabase()
{
    QAction *action = (QAction*)sender();
    QString database = action->text();

    if (QFileInfo(database).exists())
    {
        openDatabase(database);

        relocateRecentDatabase(database);
    }
}
/////////////////////////////////////////////////////////////////////////
void FrmMainWindow::actionCreateTable_triggered()
{
    on_actionNew_Query_Tab_triggered();
    SqlEditor *sqlEditorTrigger = activeSqlEditor(tabQuery->count() - 1);

    sqlEditorTrigger->setPlainText(QString("CREATE TABLE IF NOT EXISTS %1.<table>\n"
                                           "(\n"
                                           "\t<column> <type> <constraint>,\n"
                                           "\t...\n"
                                           ");").arg(itemRightClicked->parent()->text(0))
                                   );

    tabQuery->setCurrentIndex(tabQuery->count() - 1);
}

void FrmMainWindow::actionDescribeTable_triggered()
{
    QString upit = QString("select name as 'Table', sql as 'Description' from %1.sqlite_master where name = '%2' and type = 'table'").arg(itemRightClicked->parent()->parent()->text(0)).arg(itemRightClicked->text(0));
    executeSqlQuery(QStringList() << upit, activeConnection());
}

void FrmMainWindow::actionAlterTable_triggered()
{
    ///alter table može samo da preimenuje tabelu i da dodaje kolonu na kraj NPR: alter table doba add column starost text null;
    ///
    ///The column may not have a PRIMARY KEY or UNIQUE constraint.
    ///The column may not have a default value of CURRENT_TIME, CURRENT_DATE, CURRENT_TIMESTAMP, or an expression in parentheses.
    ///If a NOT NULL constraint is specified, then the column must have a default value other than NULL.
    ///If foreign key constraints are enabled and a column with a REFERENCES clause is added, the column must have a default value of NULL.

    QSqlDatabase db = QSqlDatabase::database(activeConnection());

    if (db.isOpen())
    {
        QSqlQuery upit(db);
        upit.exec(QString("select sql from %1.sqlite_master where name = '%2' and type = 'table'").arg(itemRightClicked->parent()->parent()->text(0)).arg(itemRightClicked->text(0)));

        QString rezultat;

        while (upit.next())
            rezultat = upit.value(0).toString();

        if (!rezultat.isEmpty())
        {
            //ako se na kraju upita ne nalazi ; onda ga dodaj
            if (rezultat.lastIndexOf(";") != rezultat.count() - 1)
                rezultat.append(";");

            int br = 0;
            for (int i = 0; i < rezultat.count(); i++)
            {
                if (rezultat.at(i) == ' ')
                    br++;

                if (br == 2)
                {
                    rezultat = rezultat.insert(i + 1, itemRightClicked->parent()->parent()->text(0) + ".");
                    break;
                }
            }

            rezultat.insert(0, QString("DROP TABLE %1.%2; --Read More at http://www.sqlite.org/lang_altertable.html\n").arg(itemRightClicked->parent()->parent()->text(0)).arg(itemRightClicked->text(0)));

            on_actionNew_Query_Tab_triggered();
            SqlEditor *sqlEditorView = activeSqlEditor(tabQuery->count() - 1);

            sqlEditorView->setPlainText(rezultat);

            tabQuery->setCurrentIndex(tabQuery->count() - 1);
        }
    }
}

void FrmMainWindow::actionRenameTable_triggered()
{
    FrmRenameTable *frm = new FrmRenameTable(itemRightClicked->text(0), this);
    if (frm->exec() == QDialog::Accepted)
    {
        QString name = frm->tableName(); //čitam ime tabele koje je uneto u txt
        frm->deleteLater(); //brisanje ovog objekta, više je nepotreban, kreiraće se nov

        QString upit = QString("alter table %1.%2 rename to %3").arg(itemRightClicked->parent()->parent()->text(0)).arg(itemRightClicked->text(0)).arg(name);
        executeSqlQuery(QStringList() << upit, activeConnection());
    }

    frm->deleteLater();
}

void FrmMainWindow::actionDropTable_triggered()
{
    if (QMessageBox::question(this, tr("Message"), tr("Dropping table %1.\nContinue?").arg(itemRightClicked->text(0)), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QString upit = QString("drop table %1.%2").arg(itemRightClicked->parent()->parent()->text(0)).arg(itemRightClicked->text(0));
        executeSqlQuery(QStringList() << upit, activeConnection());
    }
}

void FrmMainWindow::actionImportData_triggered()
{
    FrmImportData *frm = new FrmImportData(activeConnection(), itemRightClicked->text(0), path, this);
    connect(frm,SIGNAL(executeSqlQuery(QStringList, QString)),this,SLOT(executeSqlQuery(QStringList, QString)));
    connect(frm,SIGNAL(finished(bool)),actionImportData,SLOT(setEnabled(bool)));
    connect(frm,SIGNAL(finished()),frm,SLOT(deleteLater()));
    connect(frm,SIGNAL(setPath(QString)),this,SLOT(setPath(QString)));
    frm->show();
    actionImportData->setEnabled(false);
}

void FrmMainWindow::actionEditData_triggered()
{
    FrmEditData *frm = new FrmEditData(activeConnection(), itemRightClicked->text(0), path, this);
    connect(frm,SIGNAL(finished(bool)),actionEditData,SLOT(setEnabled(bool)));
    connect(frm,SIGNAL(finished()),frm,SLOT(deleteLater()));
    connect(frm, SIGNAL(setPath(QString)), this,SLOT(setPath(QString)));
    frm->show();

    actionEditData->setEnabled(false);
}

void FrmMainWindow::actionCreateView_triggered()
{
    on_actionNew_Query_Tab_triggered();
    SqlEditor *sqlEditorTrigger = activeSqlEditor(tabQuery->count() - 1);

    sqlEditorTrigger->setPlainText( QString("CREATE VIEW %1.<name> AS\nSELECT ").arg(itemRightClicked->parent()->text(0)) );

    tabQuery->setCurrentIndex(tabQuery->count() - 1);
}

void FrmMainWindow::actionDescribeView_triggered()
{
    QString upit = QString("select name as 'View', sql as 'Description' from %1.sqlite_master where name = '%2' and type = 'view'").arg(itemRightClicked->parent()->parent()->text(0)).arg(itemRightClicked->text(0));
    executeSqlQuery(QStringList() << upit, activeConnection());
}

void FrmMainWindow::actionAlterView_triggered()
{
    QSqlDatabase db = QSqlDatabase::database(activeConnection());

    if (db.isOpen())
    {
        QSqlQuery upit(db);
        upit.exec(QString("select sql from %1.sqlite_master where name = '%2' and type = 'view'").arg(itemRightClicked->parent()->parent()->text(0)).arg(itemRightClicked->text(0)));

        QString rezultat = "";

        while (upit.next())
            rezultat = upit.value(0).toString();

        if (!rezultat.isEmpty())
        {
            //ako se na kraju upita ne nalazi ; onda ga dodaj
            if (rezultat.lastIndexOf(";") != rezultat.count() - 1)
                rezultat.append(";");

            int br = 0;
            for (int i = 0; i < rezultat.count(); i++)
            {
                if (rezultat.at(i) == ' ')
                    br++;

                if (br == 2)
                {
                    rezultat = rezultat.insert(i + 1, itemRightClicked->parent()->parent()->text(0) + ".");
                    break;
                }
            }

            rezultat.insert(0, QString("DROP VIEW %1.%2;\n").arg(itemRightClicked->parent()->parent()->text(0)).arg(itemRightClicked->text(0)));

            on_actionNew_Query_Tab_triggered();
            SqlEditor *sqlEditorView = activeSqlEditor(tabQuery->count() - 1);

            sqlEditorView->setPlainText(rezultat);

            tabQuery->setCurrentIndex(tabQuery->count() - 1);
        }
    }
}

void FrmMainWindow::actionDropView_triggered()
{
    if (QMessageBox::question(this, tr("Message"), tr("Dropping view %1.\nContinue?").arg(itemRightClicked->text(0)), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QString upit = QString("drop view %1.%2").arg(itemRightClicked->parent()->parent()->text(0)).arg(itemRightClicked->text(0));
        executeSqlQuery(QStringList() << upit, activeConnection());
    }
}

void FrmMainWindow::actionCreateTrigger_triggered()
{
    if (itemRightClicked->parent()->parent()->text(0) == "Tables")
    {
        on_actionNew_Query_Tab_triggered();
        SqlEditor *sqlEditorTrigger = activeSqlEditor(tabQuery->count() - 1);

        sqlEditorTrigger->setPlainText( QString("CREATE TRIGGER [IF NOT EXISTS] \"%1\".\"<trigger_name>\"\n"
                                                "\t[ BEFORE | AFTER ]\n"
                                                "\tDELETE | INSERT | UPDATE | UPDATE OF <column-list>\n"
                                                "\tON %2\n"
                                                "\t[ FOR EACH ROW | FOR EACH STATEMENT ] [ WHEN expression ]\n"
                                                "BEGIN\n"
                                                "\t;\n"
                                                "END;\n").arg(itemRightClicked->parent()->parent()->parent()->text(0)).arg(itemRightClicked->text(0)) );

        tabQuery->setCurrentIndex(tabQuery->count() - 1);

    }
    else if (itemRightClicked->parent()->parent()->text(0) == "Views")
    {

        on_actionNew_Query_Tab_triggered();
        SqlEditor *sqlEditorTrigger = activeSqlEditor(tabQuery->count() - 1);

        sqlEditorTrigger->setPlainText( QString("CREATE TRIGGER [IF NOT EXISTS] \"%1\".\"<trigger_name>\"\n"
                                                "\tINSTEAD OF [DELETE | INSERT | UPDATE | UPDATE OF <column-list>]\n"
                                                "\tON %2\n"
                                                "\t[ FOR EACH ROW | FOR EACH STATEMENT ] [ WHEN expression ]\n"
                                                "BEGIN\n"
                                                "\t;\n"
                                                "END;\n").arg(itemRightClicked->parent()->parent()->parent()->text(0)).arg(itemRightClicked->text(0)) );

        tabQuery->setCurrentIndex(tabQuery->count() - 1);
    }
}

void FrmMainWindow::actionDescribeTrigger_triggered()
{
    QString upit = QString("select name as 'Trigger', sql as 'Description' from %1.sqlite_master where name = '%2' and type = 'trigger'").arg(itemRightClicked->parent()->parent()->parent()->parent()->text(0)).arg(itemRightClicked->text(0));
    executeSqlQuery(QStringList() << upit, activeConnection());
}

void FrmMainWindow::actionAlterTrigger_triggered()
{
    QSqlDatabase db = QSqlDatabase::database(activeConnection());

    if (db.isOpen())
    {
        QSqlQuery upit(db);
        upit.exec(QString("select sql from %1.sqlite_master where name = '%2' and type = 'trigger'").arg(itemRightClicked->parent()->parent()->parent()->parent()->text(0)).arg(itemRightClicked->text(0)));

        QString rezultat = "";
        while (upit.next())
            rezultat = upit.value(0).toString();

        if (!rezultat.isEmpty())
        {
            //ako se na kraju upita ne nalazi ; onda ga dodaj
            if (rezultat.lastIndexOf(";") != rezultat.count() - 1)
                rezultat.append(";");

            int br = 0;
            for (int i = 0; i < rezultat.count(); i++)
            {
                if (rezultat.at(i) == ' ')
                    br++;

                if (br == 2)
                {
                    rezultat = rezultat.insert(i + 1, itemRightClicked->parent()->parent()->parent()->parent()->text(0) + ".");
                    break;
                }
            }

            //ukoliko je trigger pronađen na početak se doda kod za brisanje
            rezultat.insert(0, QString("DROP TRIGGER %1.%2;\n").arg(itemRightClicked->parent()->parent()->parent()->parent()->text(0)).arg(itemRightClicked->text(0)));

            on_actionNew_Query_Tab_triggered();
            SqlEditor *sqlEditorView = activeSqlEditor(tabQuery->count() - 1);

            sqlEditorView->setPlainText(rezultat);

            tabQuery->setCurrentIndex(tabQuery->count() - 1);

        }
    }
}

void FrmMainWindow::actionDropTrigger_triggered()
{
    if (QMessageBox::question(this, tr("Message"), tr("Dropping trigger %1.\nContinue?").arg(itemRightClicked->text(0)), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QString upit = QString("drop trigger %1.%2").arg(itemRightClicked->parent()->parent()->parent()->parent()->text(0)).arg(itemRightClicked->text(0));
        executeSqlQuery(QStringList() << upit, activeConnection());
    }
}

void FrmMainWindow::actionCreateIndex_triggered()
{
    on_actionNew_Query_Tab_triggered();
    SqlEditor *sqlEditorTrigger = activeSqlEditor(tabQuery->count() - 1);

    sqlEditorTrigger->setPlainText( QString("CREATE INDEX %1.<name> ON %1.<table>(<column>);").arg(itemRightClicked->parent()->parent()->parent()->text(0)) );

    tabQuery->setCurrentIndex(tabQuery->count() - 1);
}

void FrmMainWindow::actionDescribeIndex_triggered()
{
    QString upit = QString("select name as 'Index', sql as 'Description' from %1.sqlite_master where name = '%2' and type = 'index'").arg(itemRightClicked->parent()->parent()->parent()->parent()->text(0)).arg(itemRightClicked->text(0));
    executeSqlQuery(QStringList() << upit, activeConnection());
}

void FrmMainWindow::actionReIndex_triggered()
{
    QString upit = QString("REINDEX %1.%2").arg(itemRightClicked->parent()->parent()->parent()->parent()->text(0)).arg(itemRightClicked->text(0));
    executeSqlQuery(QStringList() << upit, activeConnection());
}

void FrmMainWindow::actionDropIndex_triggered()
{
    if (QMessageBox::question(this, tr("Message"), tr("Dropping index %1.\nContinue?").arg(itemRightClicked->text(0)), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QString upit = QString("drop index %1.%2").arg(itemRightClicked->parent()->parent()->parent()->parent()->text(0)).arg(itemRightClicked->text(0));
        executeSqlQuery(QStringList() << upit, activeConnection());
    }
}

void FrmMainWindow::actionDetachDatabase_triggered()
{
    QString upit = QString("detach database %1").arg(itemRightClicked->text(0));
    executeSqlQuery(QStringList() << upit, activeConnection());

    on_actionReload_triggered();
}

void FrmMainWindow::executeSqlQuery(QStringList upitList, QString connection)
{
        //zatvaranje svih Result tabova pre izvršavanja upita
    for (int i = ui->tabResult->count() - 1; i >= 0; i--)
        ui->tabResult->tabCloseRequested(i);

    //prosleđivanje imena konekcije sa kojom se radi
    queryExecution = new QueryExecutionObject();
    queryExecution->setObjectName(activeConnection());

    queryThread = new QThread;
    queryThread.data()->setObjectName(activeConnection());

    queryExecution->setQuery(upitList);//prosleđivanje upita koji treba da se izvrše
    queryExecution->setConnection(connection);

    //prebacivanje izvršenja upita na drugu nit
    queryExecution->moveToThread(queryThread);
    //konektovanje signala i slotova, koliko sam dosad skontao, ima veze po kom redosledu idu
    connect(queryThread,SIGNAL(started()),queryExecution,SLOT(executeQuery()));

    connect(queryExecution,SIGNAL(queryOutput(QString,QString,QString,QString)),this,SLOT(queryOutput(QString,QString,QString,QString)));
    connect(queryExecution,SIGNAL(queryResult(QSqlQuery)),this,SLOT(queryResult(QSqlQuery)),Qt::BlockingQueuedConnection);//mora da se blokira queued connection, jer inače puca nit

    connect(queryExecution,SIGNAL(finished()),queryThread,SLOT(quit()));
    connect(queryExecution,SIGNAL(enable(bool)),ui->actionExecute_Query,SLOT(setEnabled(bool)));
    connect(queryExecution,SIGNAL(enable(bool)),ui->actionExecute_Query_Under_Cursor,SLOT(setEnabled(bool)));
    connect(queryExecution,SIGNAL(enable(bool)),ui->actionExecute_Explain_Query_Plan,SLOT(setEnabled(bool)));

    connect(queryExecution,SIGNAL(finished()),queryExecution,SLOT(deleteLater()));
    connect(queryThread,SIGNAL(finished()),queryThread,SLOT(deleteLater()));

    queryThread->start();

    ui->actionExecute_Query->setEnabled(false);
    ui->actionExecute_Query_Under_Cursor->setEnabled(false);
    ui->actionExecute_Explain_Query_Plan->setEnabled(false);
}

void FrmMainWindow::update_timeout()
{
    QSettings settingsQuery(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName().append("_query"));

    settingsQuery.beginWriteArray("querys");

    int br = 0;

    for(int i = 0; i < tabQuery->count(); i++)
    {
        if (!activeSqlEditor(i)->toPlainText().isEmpty())
        {
            settingsQuery.setArrayIndex(br);
            settingsQuery.setValue("tab", tabQuery->tabText(i));
            settingsQuery.setValue("query", activeSqlEditor(i)->toPlainText());
            br++;
        }
    }
    settingsQuery.endArray();

    ui->statusBar->showMessage(tr("SQL querys auto saved!"),5000);
}

void FrmMainWindow::setPath(QString path)
{
    this->path = path;

    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("savePath");
    settingsConfig.setValue("path",path);
    settingsConfig.endGroup();
}

void FrmMainWindow::setFindAndReplaceActivity(bool activity)
{
    isFindAndReplaceActive = activity;
}

void FrmMainWindow::autosave()
{
    if (this->findChild<QTimer*>("autosavetimer") != NULL)
        delete this->findChild<QTimer*>("autosavetimer");

    if (autosaveTime > 0)
    {
        QTimer *timer = new QTimer(this);
        timer->setObjectName("autosavetimer");
        connect(timer,SIGNAL(timeout()),this,SLOT(update_timeout()));
        timer->start(autosaveTime * 60000); //! 1000 msc je sekunda, onda je očigledno 60.000 msc je jedan minut
    }
}

void FrmMainWindow::relocateRecentDatabase(QString database)
{
    //ukoliko nema nijedne recent database dodaj novu
    if (menuRecentDatabases->actions().count() == 0)
    {
        QAction *action = new QAction(database, this);
        menuRecentDatabases->addAction(action);
        connect(action,SIGNAL(triggered()),this,SLOT(openRecentDatabase()));
    }
    else
    {
        //ukoliko ima proveri da li je već tu
        bool isInRecentDatabases = false;

        for (int i = 0; i < recentlyUsedDatabases; i++)
        {
            //ukoliko se baza već nalazi u recent databases, taj obriši i dodaj ga na početak
            if (menuRecentDatabases->actions().at(i)->text() == database)
            {
                QAction *action = menuRecentDatabases->actions().at(i);
                menuRecentDatabases->removeAction(action);

                if (menuRecentDatabases->actions().count() == 0)
                    menuRecentDatabases->addAction(action);
                else
                    menuRecentDatabases->insertAction(menuRecentDatabases->actions().first(), action);

                isInRecentDatabases = true;
                break;
            }

            //recentlyUsedDatabases je broj koji je korisnik ograničio da može a ne mora toliko da bude,
            if (i == menuRecentDatabases->actions().count() - 1)
                break;
        }

        //nema ga u recent databases, ,
        if (!isInRecentDatabases)
        {
            //obim je dostignut, dodaj ga na početak a zadnji obriši
            if (menuRecentDatabases->actions().count() == recentlyUsedDatabases)
            {
                QAction *action = menuRecentDatabases->actions().last();
                action->setText(database);
                menuRecentDatabases->removeAction(action);
                menuRecentDatabases->insertAction(menuRecentDatabases->actions().first(), action);
            }
            else
            { //obim nije dostignut, dodaj ga na početak
                QAction *action = new QAction(database, this);
                menuRecentDatabases->insertAction(menuRecentDatabases->actions().first(), action);
                connect(action,SIGNAL(triggered()),this,SLOT(openRecentDatabase()));
            }
        }
    }

    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("databases");

    QStringList dbs;
    for (int i = menuRecentDatabases->actions().count() - 1; i >= 0; i--)
        dbs.append(menuRecentDatabases->actions().at(i)->text());

    settingsConfig.setValue("databases", dbs);

    settingsConfig.endGroup();
}

QTreeWidgetItem *FrmMainWindow::addDatabase(QTreeWidget *parent, QString name)
{
    QTreeWidgetItem *itemDatabase = new QTreeWidgetItem(parent);
    itemDatabase->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemDatabase->setText(0, name);
    itemDatabase->setIcon(0, QIcon(":/icons/icons/database.png"));
    itemDatabase->addChild(itemDatabase);

    QTreeWidgetItem *itemTables = new QTreeWidgetItem();
    itemTables->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemTables->setText(0, "Tables");
    itemDatabase->addChild(itemTables);

    QTreeWidgetItem *itemViews = new QTreeWidgetItem();
    itemViews->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemViews->setText(0, "Views");
    itemDatabase->addChild(itemViews);

    QTreeWidgetItem *itemSystemCatalogue = new QTreeWidgetItem();
    itemSystemCatalogue->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemSystemCatalogue->setText(0, "System Catalogue");
    itemDatabase->addChild(itemSystemCatalogue);

    return itemDatabase;
}

QTreeWidgetItem* FrmMainWindow::addTable(QTreeWidgetItem *parent, QString name)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    item->setText(0,name);
    item->setIcon(0, QIcon(":/icons/icons/table.png"));
    parent->addChild(item);
    ////
    //dodavanje kolona
    QTreeWidgetItem *itemColum = new QTreeWidgetItem();
    itemColum->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemColum->setText(0,"Columns");
    //itemColum->setIcon(0, QIcon(":/icons/icons/table.png"));
    item->addChild(itemColum);

    //dodavanje indexa
    QTreeWidgetItem *itemIndexes = new QTreeWidgetItem();
    itemIndexes->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemIndexes->setText(0,"Indexes");
    //itemColum->setIcon(0, QIcon(":/icons/icons/table.png"));
    item->addChild(itemIndexes);

    //dodavanje system indexa
    QTreeWidgetItem *itemSystemIndexes = new QTreeWidgetItem();
    itemSystemIndexes->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemSystemIndexes->setText(0,"System Indexes");
    //itemColum->setIcon(0, QIcon(":/icons/icons/table.png"));
    item->addChild(itemSystemIndexes);

    //dodavanje triggera
    QTreeWidgetItem *itemTreggers = new QTreeWidgetItem();
    itemTreggers->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemTreggers->setText(0,"Triggers");
    //itemColum->setIcon(0, QIcon(":/icons/icons/table.png"));
    item->addChild(itemTreggers);

    return item;
}

void FrmMainWindow::addField(QTreeWidgetItem *parent, QString name, int isPK)
{
    QTreeWidgetItem *itemField = new QTreeWidgetItem();
    itemField->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemField->setText(0, name);

    if (isPK == 1)
        itemField->setIcon(0, QIcon(":/icons/icons/primarykey.png"));
    else
        itemField->setIcon(0, QIcon(":/icons/icons/field.png"));

    parent->addChild(itemField);
}

void FrmMainWindow::addIndexTrigger(QTreeWidgetItem *parent, QString name)
{
    QTreeWidgetItem *itemIndex = new QTreeWidgetItem();
    itemIndex->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemIndex->setText(0, name);
    parent->addChild(itemIndex);
}

QTreeWidgetItem* FrmMainWindow::addViews(QTreeWidgetItem *parent, QString name)
{
    QTreeWidgetItem *itemViews = new QTreeWidgetItem();
    itemViews->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemViews->setText(0, name);
    parent->addChild(itemViews);

    //triggeri na pogled
    QTreeWidgetItem *itemViewsTrigger = new QTreeWidgetItem();
    itemViewsTrigger->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemViewsTrigger->setText(0, "Triggers");
    itemViews->addChild(itemViewsTrigger);

    return itemViews;
}

void FrmMainWindow::addViewTrigger(QTreeWidgetItem *parent, QString name)
{
    QTreeWidgetItem *itemIndex = new QTreeWidgetItem();
    itemIndex->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemIndex->setText(0, name);
    parent->addChild(itemIndex);
}

void FrmMainWindow::addSystemCatalogue(QTreeWidgetItem *parent, QString name)
{
    QTreeWidgetItem *itemSystemCatalogue = new QTreeWidgetItem();
    itemSystemCatalogue->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);
    itemSystemCatalogue->setText(0, name);
    parent->addChild(itemSystemCatalogue);
}

void FrmMainWindow::inicijalizacijaFieldInformation()
{
    ///[0] --inicijalizacija field labela (prikazuju info o kliknutom polju)
    lblFieldValue = new LabelValue(this);
    lblFieldValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lblFieldValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->fieldGrid->addWidget(lblFieldValue,0,1);
    connect(lblFieldValue,SIGNAL(mouseDoubleClicked()),this,SLOT(lblFieldValue_doubleClicked()));

    lblTypeValue = new LabelValue(this);
    lblTypeValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lblTypeValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->fieldGrid->addWidget(lblTypeValue,1,1);
    connect(lblTypeValue,SIGNAL(mouseDoubleClicked()),this,SLOT(lblTypeValue_doubleClicked()));

    lblNullValue = new LabelValue(this);
    lblNullValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lblNullValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->fieldGrid->addWidget(lblNullValue,2,1);
    connect(lblNullValue,SIGNAL(mouseDoubleClicked()),this,SLOT(lblNullValue_doubleClicked()));

    lblDefaultValue = new LabelValue(this);
    lblDefaultValue->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lblDefaultValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->fieldGrid->addWidget(lblDefaultValue,3,1);
    connect(lblDefaultValue,SIGNAL(mouseDoubleClicked()),this,SLOT(lblDefaultValue_doubleClicked()));
    ///[0]
}

void FrmMainWindow::inicijalizacijaGrupnihAkcija()
{
    ///[1] povezivanje cut, copy itd sa slotovima
    connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(activeCut()));
    connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(activeCopy()));
    connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(activePaste()));
    connect(ui->actionUndo,SIGNAL(triggered()),this,SLOT(activeUndo()));
    connect(ui->actionRedo,SIGNAL(triggered()),this,SLOT(activeRedo()));
    connect(ui->actionDelete,SIGNAL(triggered()),this,SLOT(activeDelete()));

    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);

    ui->actionCopy->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
    ui->actionDelete->setEnabled(false);

    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(clipbord_dataChanged()));
    ///[1]
}

void FrmMainWindow::inicijalizacijaTabQuery()
{
    ///dodavanje custom tab widgeta za sqlEditor
    tabQuery = new SqlTabWidget(this);
    connect(tabQuery,SIGNAL(tabCloseRequested(int)),this,SLOT(tabQuery_tabCloseRequested(int)));
    connect(tabQuery,SIGNAL(currentChanged(int)),this,SLOT(tabQuery_currentChanged(int)));
    connect(tabQuery,SIGNAL(middleClicked(int)),this,SLOT(tabQuery_middleClicked(int)));
    connect(tabQuery,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(tabQuery_customContextMenuRequested(QPoint)));

    ui->splitterQueryOdTblOdMessage->insertWidget(0, tabQuery);
}

void FrmMainWindow::inicijalizacijaTabeleOutput()
{
    ///tblOutput --poruke o greškama, i uspešno izvršenim upitima
    QStringList kolone = QStringList() << tr("No.") << tr("Time") << tr("Duration") << tr("Action") << tr("Message");
    ui->tblOutput->setColumnCount(kolone.count());
    ui->tblOutput->setHorizontalHeaderLabels(kolone);
    ui->tblOutput->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblOutput->setFocusPolicy(Qt::ClickFocus);
    ui->tblOutput->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    //inicijalne vrednosti kolona u tblOutput, za rezoluciju 1366x766
    ui->tblOutput->setColumnWidth(0,35);
    ui->tblOutput->setColumnWidth(1,70);
    ui->tblOutput->setColumnWidth(2,70);
    ui->tblOutput->setColumnWidth(3,440);
    ui->tblOutput->setColumnWidth(4,440);
    ui->tblOutput->verticalHeader()->setDefaultSectionSize(20);
    ui->tblOutput->setAlternatingRowColors(true);
}

void FrmMainWindow::inicijalizacijaDesniKlikMenija()
{
    ///sqlTabWidget menu
    menuTabQuery = new QMenu(this);
    actionCloneTab = new QAction(tr("Clone Tab"), this);
    menuTabQuery->addAction(ui->actionNew_Query_Tab);
    menuTabQuery->addSeparator();
    menuTabQuery->addAction(ui->actionClose_All_Querys_Except_This);
    menuTabQuery->addAction(ui->actionClose_All_Querys);
    menuTabQuery->addSeparator();
    menuTabQuery->addAction(actionCloneTab);

    connect(actionCloneTab,SIGNAL(triggered()),this,SLOT(actionCloneTab_triggered()));
    ////

    ///tableView Result menu
    menuTblOutput = new QMenu(this);
    actionTOCopyAction = new QAction(tr("Copy Action"), this);
    actionTOCopyMessage = new QAction(tr("Copy Message"), this);
    actionTOCopy = new QAction(tr("Copy"), this);
    actionTOClearOutput = new QAction(tr("Clear"),this);

    menuTblOutput->addAction(actionTOCopyAction);
    menuTblOutput->addAction(actionTOCopyMessage);
    menuTblOutput->addAction(actionTOCopy);
    menuTblOutput->addSeparator();
    menuTblOutput->addAction(actionTOClearOutput);

    ui->tblOutput->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tblOutput,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(tblOutput_customContextMenuRequested(QPoint)));

    connect(actionTOCopyAction,SIGNAL(triggered()),this,SLOT(actionTOCopyAction_triggered()));
    connect(actionTOCopyMessage,SIGNAL(triggered()),this,SLOT(actionTOCopyMessage_triggered()));
    connect(actionTOCopy,SIGNAL(triggered()),this,SLOT(actionTOCopy_triggered()));
    connect(actionTOClearOutput,SIGNAL(triggered()),this,SLOT(actionTOClearOutput_triggered()));

    ////

    /// desni klik meni na sqlEditor
    menuSqlEditor = new QMenu(this);
    menuSqlEditor->addAction(ui->actionExecute_Query);
    menuSqlEditor->addSeparator();
    menuSqlEditor->addAction(ui->actionCut);
    menuSqlEditor->addAction(ui->actionCopy);
    menuSqlEditor->addAction(ui->actionPaste);
    menuSqlEditor->addAction(ui->actionDelete);
    menuSqlEditor->addSeparator();
    menuSqlEditor->addAction(ui->actionSelect_All);
    ////

    ///tree database menu
    menuTreeDatabase = new QMenu(this);

    actionCreateTable = new QAction(tr("Create Table"), this);
    actionDescribeTable = new QAction(tr("Describe Table"), this);
    actionAlterTable = new QAction(tr("Alter Table"), this);
    actionRenameTable = new QAction(tr("Rename Table"), this);
    actionDropTable = new QAction(tr("Drop Table"), this);
    actionImportData = new QAction(tr("Import Data"), this);
    actionEditData = new QAction(tr("Edit Data"), this);

    connect(actionCreateTable,SIGNAL(triggered()),this,SLOT(actionCreateTable_triggered()));
    connect(actionDescribeTable,SIGNAL(triggered()),this,SLOT(actionDescribeTable_triggered()));
    connect(actionAlterTable,SIGNAL(triggered()),this,SLOT(actionAlterTable_triggered()));
    connect(actionRenameTable,SIGNAL(triggered()),this,SLOT(actionRenameTable_triggered()));
    connect(actionDropTable,SIGNAL(triggered()),this,SLOT(actionDropTable_triggered()));
    connect(actionImportData,SIGNAL(triggered()),this,SLOT(actionImportData_triggered()));
    connect(actionEditData,SIGNAL(triggered()),this,SLOT(actionEditData_triggered()));

    actionCreateView = new QAction(tr("Create View"), this);
    actionDescribeView = new QAction(tr("Describe View"), this);
    actionAlterView = new QAction(tr("Alter View"),this);
    actionDropView = new QAction(tr("Drop View"), this);

    connect(actionCreateView,SIGNAL(triggered()),this,SLOT(actionCreateView_triggered()));
    connect(actionDescribeView,SIGNAL(triggered()),this,SLOT(actionDescribeView_triggered()));
    connect(actionAlterView,SIGNAL(triggered()),this,SLOT(actionAlterView_triggered()));
    connect(actionDropView,SIGNAL(triggered()),this,SLOT(actionDropView_triggered()));

    actionCreateTrigger = new QAction(tr("Create Trigger"), this);
    actionDescribeTrigger = new QAction(tr("Describe Trigger"), this);
    actionAlterTrigger = new QAction(tr("Alter Trigger"), this);
    actionDropTrigger = new QAction(tr("Drop Trigger"), this);

    connect(actionCreateTrigger,SIGNAL(triggered()),this,SLOT(actionCreateTrigger_triggered()));
    connect(actionDescribeTrigger,SIGNAL(triggered()),this,SLOT(actionDescribeTrigger_triggered()));
    connect(actionAlterTrigger,SIGNAL(triggered()),this,SLOT(actionAlterTrigger_triggered()));
    connect(actionDropTrigger,SIGNAL(triggered()),this,SLOT(actionDropTrigger_triggered()));

    actionCreateIndex = new QAction(tr("Create Index"), this);
    actionDescribeIndex = new QAction(tr("Describe Index"), this);
    actionReIndex = new QAction(tr("Reindex"), this);
    actionDropIndex = new QAction(tr("Drop Index"), this);

    connect(actionCreateIndex,SIGNAL(triggered()),this,SLOT(actionCreateIndex_triggered()));
    connect(actionDescribeIndex,SIGNAL(triggered()),this,SLOT(actionDescribeIndex_triggered()));
    connect(actionReIndex,SIGNAL(triggered()),this,SLOT(actionReIndex_triggered()));
    connect(actionDropIndex,SIGNAL(triggered()),this,SLOT(actionDropIndex_triggered()));

    actionDetachDatabase = new QAction(tr("Detach Database"), this);

    connect(actionDetachDatabase,SIGNAL(triggered()),this,SLOT(actionDetachDatabase_triggered()));

    menuTreeDatabase->addAction(actionCreateTable);//ovaj ide na Tables
    menuTreeDatabase->addAction(actionDescribeTable);
    menuTreeDatabase->addAction(actionAlterTable);
    menuTreeDatabase->addAction(actionRenameTable);
    menuTreeDatabase->addAction(actionDropTable);
    menuTreeDatabase->addAction(actionImportData);
    menuTreeDatabase->addAction(actionEditData);
    menuTreeDatabase->addAction(actionCreateView);
    menuTreeDatabase->addAction(actionCreateIndex);
    menuTreeDatabase->addAction(actionCreateTrigger);
    menuTreeDatabase->addAction(actionDescribeView);
    menuTreeDatabase->addAction(actionAlterView);
    menuTreeDatabase->addAction(actionDropView);
    menuTreeDatabase->addAction(actionDescribeIndex);
    menuTreeDatabase->addAction(actionReIndex);
    menuTreeDatabase->addAction(actionDropIndex);
    menuTreeDatabase->addAction(actionDescribeTrigger);
    menuTreeDatabase->addAction(actionAlterTrigger);
    menuTreeDatabase->addAction(actionDropTrigger);
    menuTreeDatabase->addSeparator();
    menuTreeDatabase->addAction(actionDetachDatabase);
    menuTreeDatabase->addAction(ui->actionReload);
    ////

    ///meni na tabelu query result
    menuTblResult = new QMenu(this);

    actionTRCopyField = new QAction(tr("Copy Field"),this);
    actionTRCopyRow = new QAction(tr("Copy Row"),this);
    actionTRCopy = new QAction(tr("Copy"),this);
    actionTRBlob = new QAction(QIcon(":/icons/icons/preview.png"), tr("BLOB Details"),this);

    menuTblResult->addAction(actionTRCopyField);
    menuTblResult->addAction(actionTRCopyRow);
    menuTblResult->addAction(actionTRCopy);
    menuTblResult->addSeparator();
    menuTblResult->addAction(actionTRBlob);

    connect(actionTRCopyField,SIGNAL(triggered()),this,SLOT(actionTRCopyField_triggered()));
    connect(actionTRCopyRow,SIGNAL(triggered()),this,SLOT(actionTRCopyRow_triggered()));
    connect(actionTRCopy,SIGNAL(triggered()),this,SLOT(actionTRCopy_triggered()));
    connect(actionTRBlob,SIGNAL(triggered()),this,SLOT(actionTRBlob_triggered()));
    ////

    ui->tabResult->setContextMenuPolicy(Qt::CustomContextMenu);
    menuTblResultTab = new QMenu(this);

    actionCloseResultsOnRight = new QAction(tr("Close Results On Right"), this);
    actionCloseAllResultsExceptThis = new QAction(tr("Close All Results Except This"), this);
    actionCloseAllResults = new QAction(tr("Close All Results"), this);

    menuTblResultTab->addAction(actionCloseResultsOnRight);
    menuTblResultTab->addAction(actionCloseAllResultsExceptThis);
    menuTblResultTab->addAction(actionCloseAllResults);

    connect(actionCloseResultsOnRight,SIGNAL(triggered()),this,SLOT(actionCloseResultsOnRight_triggered()));
    connect(actionCloseAllResultsExceptThis,SIGNAL(triggered()),this,SLOT(actionCloseAllResultsExceptThis_triggered()));
    connect(actionCloseAllResults,SIGNAL(triggered()),this,SLOT(actionCloseAllResults_triggered()));
}

void FrmMainWindow::inicijalizacijaOsnovnihVrednostiIzSettings()
{
    ///podešavanja prozora
    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("savePath");
    path = settingsConfig.value("path", QDir::homePath()).toString();
    settingsConfig.endGroup();

    settingsConfig.beginGroup("splitterState");
    ui->splitterDatabaseOdFieldInformation->restoreState(settingsConfig.value("splitterDatabaseOdFieldInformation", ui->splitterDatabaseOdFieldInformation->saveState()).toByteArray());
    ui->splitterLeveOdDesneStrane->restoreState(settingsConfig.value("splitterLeveOdDesneStrane", ui->splitterLeveOdDesneStrane->saveState()).toByteArray());
    ui->splitterQueryOdTblOdMessage->restoreState(settingsConfig.value("splitterQueryOdTblOdMessage", ui->splitterQueryOdTblOdMessage->saveState()).toByteArray());
    settingsConfig.endGroup();
    ///

    settingsConfig.beginGroup("Enviroment");
    autosaveTime = settingsConfig.value("autosave", 5).toInt();
    keepContent = settingsConfig.value("keepContent", true).toBool();
    openLastDatabasesOnStart = settingsConfig.value("openLastDatabasesOnStart", false).toBool();
    confirmClosing = settingsConfig.value("confirmClosing", true).toBool();
    recentlyUsedDatabases = settingsConfig.value("recentlyUsedDatabases", 5).toInt();
    settingsConfig.endGroup();

    //pozivanje inicijalnog tajmera
    autosave();
}

void FrmMainWindow::inicijalizacijaDokumenataKojiSuBiliOtvoreni()
{
    if (keepContent == true)
    {
        QSettings settingsQuery(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName().append("_query"));

        int size = settingsQuery.beginReadArray("querys");

        for (int i = 0; i < size; ++i)
        {
            settingsQuery.setArrayIndex(i);
            on_actionNew_Query_Tab_triggered();

            if (!settingsQuery.value("query").toString().isEmpty())
                activeSqlEditor(i)->setPlainText(settingsQuery.value("query").toString());
        }
        settingsQuery.endArray();

        if (size == 0)
            on_actionNew_Query_Tab_triggered();
    }
    else
        on_actionNew_Query_Tab_triggered();

    activeSqlEditor(0)->setFocus();
}

void FrmMainWindow::inicijalizacijaPrethodnoOtovorenihBaza()
{
    if (openLastDatabasesOnStart == true)
    {
        QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

        settingsConfig.beginGroup("databases");
        QStringList lastDatabases = settingsConfig.value("lastDatabases", QStringList()).toStringList();
        settingsConfig.endGroup();

        foreach (QString db, lastDatabases)
        {
            if (QFileInfo(db).exists())
                openDatabase(db);
        }
    }
}

void FrmMainWindow::inicijalizacijaProveraUpdate()
{
    ///check for update
    d = new Downloader(this);
    connect(d,SIGNAL(downloaded()),this,SLOT(updateCheck()));
    connect(d,SIGNAL(errorMessage(QString)),this,SLOT(errorMessage(QString)));
    loading = true;
    d->checkUpdate();
    ///
}

void FrmMainWindow::inicijalizacijaRecentDatabases()
{
    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    ///recently otvarane baze
    menuRecentDatabases = new QMenu(this);
    ui->actionRecent_Databases->setMenu(menuRecentDatabases);

    settingsConfig.beginGroup("databases");
    QStringList dbs = QStringList() << settingsConfig.value("databases").toStringList();
    settingsConfig.endGroup();

    for (int br = 0, i = dbs.count() - 1; i >= 0; i--, br++)
    {
        if (QFileInfo(dbs.at(i)).exists())
        {
            QAction *action = new QAction(dbs.at(i), this);
            menuRecentDatabases->addAction(action);
            connect(action,SIGNAL(triggered()),this,SLOT(openRecentDatabase()));
        }

        //ukoliko dosegne maksimum dozvoljenih pređašnjih baza, prekidaj
        if (br == recentlyUsedDatabases-1)
            break;
    }
}

void FrmMainWindow::inicijalizacijaSystemMenija()
{
    if (ui->tabDatabase->count() > 0)
    {
        ui->actionExport_Schema->setEnabled(true);
        ui->actionDump_Database->setEnabled(true);
        ui->actionAnalyze_Statistics->setEnabled(true);
        ui->actionVacuum->setEnabled(true);
        ui->actionAttach_Database->setEnabled(true);
        ui->actionPragma_Statements->setEnabled(true);
    }
    else
    {
        ui->actionExport_Schema->setEnabled(false);
        ui->actionDump_Database->setEnabled(false);
        ui->actionAnalyze_Statistics->setEnabled(false);
        ui->actionVacuum->setEnabled(false);
        ui->actionAttach_Database->setEnabled(false);
        ui->actionPragma_Statements->setEnabled(false);
    }
}

QTreeWidget *FrmMainWindow::activeTreeDatabase(int current)
{
    return ui->tabDatabase->findChild<QTreeWidget*>(ui->tabDatabase->widget(current)->objectName());
}

SqlEditor *FrmMainWindow::activeSqlEditor(int current)
{
    return tabQuery->findChild<SqlEditor*>(tabQuery->widget(current)->objectName());
}

QTableView *FrmMainWindow::activeResultTable(int current)
{
    return ui->tabResult->findChild<QTableView*>(ui->tabResult->widget(current)->objectName());
}

QString FrmMainWindow::activeConnection()
{
    if (QSqlDatabase::connectionNames().count() > 0)
        return dbConnections.value(activeTreeDatabase(ui->tabDatabase->currentIndex())->objectName());

    return QString();
}


