#include "frmoptions.h"
#include "ui_frmoptions.h"

FrmOptions::FrmOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmOptions)
{
    ui->setupUi(this);

    ui->listOptions->setViewMode(QListView::ListMode);
    ui->listOptions->setIconSize(QSize(32,32));
    ui->listOptions->setMovement(QListView::Static);
    ui->listOptions->setMaximumWidth(165);
    ui->listOptions->setMinimumWidth(165);
    ui->listOptions->setSpacing(5);

    QListWidgetItem *enviromentItem = new QListWidgetItem(ui->listOptions);
    enviromentItem->setText(tr("Enviroment"));
    enviromentItem->setIcon(QIcon(":/icons/icons/enviroment.png"));
    enviromentItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *queryEdiorItem = new QListWidgetItem(ui->listOptions);
    queryEdiorItem->setText(tr("Query Editor"));
    queryEdiorItem->setIcon(QIcon(":/icons/icons/document.png"));
    queryEdiorItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *languageItem = new QListWidgetItem(ui->listOptions);
    languageItem->setText(tr("Language"));
    languageItem->setIcon(QIcon(":/icons/icons/applications-education-language.png"));
    languageItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    pagesWidget = new QStackedWidget;
    pagesWidget->addWidget(new EnviromentPage);
    pagesWidget->addWidget(new QueryEditorPage);
    pagesWidget->addWidget(new LanguagePage);

    ui->hLayoutGlobal->addWidget(pagesWidget, 1);

    QPushButton *btnNormalText = this->findChild<QPushButton*>("btnNormalText");
    connect(btnNormalText,SIGNAL(clicked()),this,SLOT(btnNormalText_clicked()));

    QPushButton *btnNumber = this->findChild<QPushButton*>("btnNumber");
    connect(btnNumber,SIGNAL(clicked()),this,SLOT(btnNumber_clicked()));

    QPushButton *btnStringDouble = this->findChild<QPushButton*>("btnStringDouble");
    connect(btnStringDouble,SIGNAL(clicked()),this,SLOT(btnStringDouble_clicked()));

    QPushButton *btnStringSingle = this->findChild<QPushButton*>("btnStringSingle");
    connect(btnStringSingle,SIGNAL(clicked()),this,SLOT(btnStringSingle_clicked()));

    QPushButton *btnKeywords = this->findChild<QPushButton*>("btnKeywords");
    connect(btnKeywords,SIGNAL(clicked()),this,SLOT(btnKeyword_clicked()));

    QPushButton *btnFunction = this->findChild<QPushButton*>("btnFunctions");
    connect(btnFunction,SIGNAL(clicked()),this,SLOT(btnFunction_clicked()));

    QPushButton *btnComment = this->findChild<QPushButton*>("btnComment");
    connect(btnComment,SIGNAL(clicked()),this,SLOT(btnComment_clicked()));

    QPushButton *btnResetColors = this->findChild<QPushButton*>("btnResetColors");
    connect(btnResetColors,SIGNAL(clicked()),this,SLOT(btnResetColors_clicked()));

    loadValues();

}

FrmOptions::~FrmOptions()
{
    delete ui;
}

void FrmOptions::on_listOptions_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(ui->listOptions->row(current));
}

void FrmOptions::btnNormalText_clicked()
{
    QColorDialog *dialog = new QColorDialog(this);
    dialog->setCurrentColor(listaBoja.at(0));

    if (dialog->exec() == QDialog::Accepted)
    {
        QPushButton *btn = this->findChild<QPushButton*>("btnNormalText");
        btn->setIcon(createIcon(dialog->selectedColor()));
        listaBoja.replace(0, dialog->selectedColor());
    }
}

void FrmOptions::btnNumber_clicked()
{
    QColorDialog *dialog = new QColorDialog(this);
    dialog->setCurrentColor(listaBoja.at(1));

    if (dialog->exec() == QDialog::Accepted)
    {
        QPushButton *btn = this->findChild<QPushButton*>("btnNumber");
        btn->setIcon(createIcon(dialog->selectedColor()));
        listaBoja.replace(1, dialog->selectedColor());
    }
}

void FrmOptions::btnStringDouble_clicked()
{
    QColorDialog *dialog = new QColorDialog(this);
    dialog->setCurrentColor(listaBoja.at(2));

    if (dialog->exec() == QDialog::Accepted)
    {
        QPushButton *btn = this->findChild<QPushButton*>("btnStringDouble");
        btn->setIcon(createIcon(dialog->selectedColor()));
        listaBoja.replace(2, dialog->selectedColor());
    }
}

void FrmOptions::btnStringSingle_clicked()
{
    QColorDialog *dialog = new QColorDialog(this);
    dialog->setCurrentColor(listaBoja.at(3));

    if (dialog->exec() == QDialog::Accepted)
    {
        QPushButton *btn = this->findChild<QPushButton*>("btnStringSingle");
        btn->setIcon(createIcon(dialog->selectedColor()));
        listaBoja.replace(3, dialog->selectedColor());
    }
}

void FrmOptions::btnKeyword_clicked()
{
    QColorDialog *dialog = new QColorDialog(this);
    dialog->setCurrentColor(listaBoja.at(4));

    if (dialog->exec() == QDialog::Accepted)
    {
        QPushButton *btn = this->findChild<QPushButton*>("btnKeywords");
        btn->setIcon(createIcon(dialog->selectedColor()));
        listaBoja.replace(4, dialog->selectedColor());
    }
}

void FrmOptions::btnFunction_clicked()
{
    QColorDialog *dialog = new QColorDialog(this);
    dialog->setCurrentColor(listaBoja.at(5));

    if (dialog->exec() == QDialog::Accepted)
    {
        QPushButton *btn = this->findChild<QPushButton*>("btnFunctions");
        btn->setIcon(createIcon(dialog->selectedColor()));
        listaBoja.replace(5, dialog->selectedColor());
    }
}

void FrmOptions::btnComment_clicked()
{
    QColorDialog *dialog = new QColorDialog(this);
    dialog->setCurrentColor(listaBoja.at(6));

    if (dialog->exec() == QDialog::Accepted)
    {
        QPushButton *btn = this->findChild<QPushButton*>("btnComment");
        btn->setIcon(createIcon(dialog->selectedColor()));
        listaBoja.replace(6, dialog->selectedColor());
    }
}

void FrmOptions::btnResetColors_clicked()
{
    //Query Editor - Highlighter
    listaBoja.clear();

    QColor colorNormalText(Qt::black);
    QColor colorNumber(Qt::darkYellow);
    QColor colorStringDoubleSingle(Qt::darkGreen);
    QColor colorKeywords(Qt::darkBlue);
    QColor colorFunctions(Qt::blue);
    QColor colorComment(Qt::gray);

    QPushButton *btnNormalText = this->findChild<QPushButton*>("btnNormalText");
    listaBoja.append(colorNormalText);
    btnNormalText->setIcon(createIcon( listaBoja.at(0) ));

    QPushButton *btnNumber = this->findChild<QPushButton*>("btnNumber");
    listaBoja.append(colorNumber);
    btnNumber->setIcon(createIcon( listaBoja.at(1) ));

    QPushButton *btnStringDouble = this->findChild<QPushButton*>("btnStringDouble");
    listaBoja.append(colorStringDoubleSingle);
    btnStringDouble->setIcon(createIcon( listaBoja.at(2) ));

    QPushButton *btnStringSingle = this->findChild<QPushButton*>("btnStringSingle");
    listaBoja.append(colorStringDoubleSingle);
    btnStringSingle->setIcon(createIcon( listaBoja.at(3) ));

    QPushButton *btnKeywords = this->findChild<QPushButton*>("btnKeywords");
    listaBoja.append(colorKeywords);
    btnKeywords->setIcon(createIcon( listaBoja.at(4) ));

    QPushButton *btnFunctions = this->findChild<QPushButton*>("btnFunctions");
    listaBoja.append(colorFunctions);
    btnFunctions->setIcon(createIcon( listaBoja.at(5) ));

    QPushButton *btnComment = this->findChild<QPushButton*>("btnComment");
    listaBoja.append(colorComment);
    btnComment->setIcon(createIcon( listaBoja.at(6) ));

}

void FrmOptions::on_btnResetAll_clicked()
{
    int rez = QMessageBox::information(this,tr("Message"),tr("Reset all configurations to default.\nContinue?"), QMessageBox::Yes | QMessageBox::No);

    if (rez == QMessageBox::Yes)
        loadDefaultValues();
}

void FrmOptions::on_btnOK_clicked()
{
    saveValues();
    emit optionsIzmena();
    this->close();
}

void FrmOptions::on_btnApply_clicked()
{
    saveValues();
    emit optionsIzmena();
}

void FrmOptions::on_btnCancel_clicked()
{
    this->close();
}

void FrmOptions::loadValues()
{
    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("language");
    QComboBox *cmbLanguage = this->findChild<QComboBox*>("cmbLanguage");

    QString language =  settingsConfig.value("language","").toString();

    if (language == "")
        cmbLanguage->setCurrentIndex(0);

    else if (language == "en_US")
        cmbLanguage->setCurrentIndex(1);

    else if (language == "sr_RS")
        cmbLanguage->setCurrentIndex(2);

    else if (language == "sr_RS@latin")
        cmbLanguage->setCurrentIndex(3);

    settingsConfig.endGroup();
///////////////////////////////////////////////////////
    settingsConfig.beginGroup("font");

    QFontComboBox *fcmbFamily = this->findChild<QFontComboBox*>("fcmbFamily");
    fcmbFamily->setCurrentText(settingsConfig.value("family","Sans Serif").toString());

    QSpinBox *sbSize = this->findChild<QSpinBox*>("sbSize");
    sbSize->setValue(settingsConfig.value("size",9).toInt());

    settingsConfig.endGroup();
///////////////////////////////////////////////////////
    settingsConfig.beginGroup("text");

    QCheckBox *chcEnableTextWrapping = this->findChild<QCheckBox*>("chcEnableTextWrapping");
    chcEnableTextWrapping->setChecked(settingsConfig.value("textWrapping", false).toBool());

    settingsConfig.endGroup();
///////////////////////////////////////////////////////
    settingsConfig.beginGroup("highlighter");

    QColor colorNormalText(Qt::black);
    QColor colorNumber(Qt::darkYellow);
    QColor colorStringDoubleSingle(Qt::darkGreen);
    QColor colorKeywords(Qt::darkBlue);
    QColor colorFunctions(Qt::blue);
    QColor colorComment(Qt::gray);

    QPushButton *btnNormalText = this->findChild<QPushButton*>("btnNormalText");
    listaBoja.append(settingsConfig.value("normalText", colorNormalText).value<QColor>());
    btnNormalText->setIcon(createIcon( listaBoja.at(0) ));

    QPushButton *btnNumber = this->findChild<QPushButton*>("btnNumber");
    listaBoja.append( settingsConfig.value("number", colorNumber).value<QColor>() );
    btnNumber->setIcon(createIcon( listaBoja.at(1) ));

    QPushButton *btnStringDouble = this->findChild<QPushButton*>("btnStringDouble");
    listaBoja.append( settingsConfig.value("stringDouble", colorStringDoubleSingle).value<QColor>() );
    btnStringDouble->setIcon(createIcon( listaBoja.at(2) ));

    QPushButton *btnStringSingle = this->findChild<QPushButton*>("btnStringSingle");
    listaBoja.append( settingsConfig.value("stringSingle", colorStringDoubleSingle).value<QColor>() );
    btnStringSingle->setIcon(createIcon( listaBoja.at(3) ));

    QPushButton *btnKeywords = this->findChild<QPushButton*>("btnKeywords");
    listaBoja.append( settingsConfig.value("keywords", colorKeywords).value<QColor>() );
    btnKeywords->setIcon(createIcon( listaBoja.at(4) ));

    QPushButton *btnFunctions = this->findChild<QPushButton*>("btnFunctions");
    listaBoja.append( settingsConfig.value("functions", colorFunctions).value<QColor>() );
    btnFunctions->setIcon(createIcon( listaBoja.at(5) ));

    QPushButton *btnComment = this->findChild<QPushButton*>("btnComment");
    listaBoja.append( settingsConfig.value("comment", colorComment).value<QColor>() );
    btnComment->setIcon(createIcon( listaBoja.at(6) ));

    settingsConfig.endGroup();
///////////////////////////////////////////////////////
    settingsConfig.beginGroup("Enviroment");

    QSpinBox *sbAutoSave = this->findChild<QSpinBox*>("sbAutoSave");
    sbAutoSave->setValue( settingsConfig.value("autosave",1).toInt());

    QCheckBox *chcConfirmClosing = this->findChild<QCheckBox*>("chcConfirmClosing");
    chcConfirmClosing->setChecked( settingsConfig.value("confirmClosing", true).toBool() );

    QCheckBox *chcKeepContent = this->findChild<QCheckBox*>("chcKeepContent");
    chcKeepContent->setChecked( settingsConfig.value("keepContent", true).toBool() );

    QCheckBox *chcShowLineNumber = this->findChild<QCheckBox*>("chcShowLineNumber");
    chcShowLineNumber->setChecked( settingsConfig.value("showLineNumber", true).toBool() );

    QSpinBox *sbRecentlyUsedDatabases = this->findChild<QSpinBox*>("sbRecentlyUsedDatabases");
    sbRecentlyUsedDatabases->setValue( settingsConfig.value("recentlyUsedDatabases", 5).toInt() );

    QCheckBox *chcOpenLastDatabasesOnStart = this->findChild<QCheckBox*>("chcOpenLastDatabasesOnStart");
    chcOpenLastDatabasesOnStart->setChecked( settingsConfig.value("openLastDatabasesOnStart", false).toBool() );

    settingsConfig.endGroup();
}

void FrmOptions::loadDefaultValues()
{
    //Enviroment - Auto Save
    QSpinBox *sbAutoSave = this->findChild<QSpinBox*>("sbAutoSave");
    sbAutoSave->setValue(1);

    //Enviroment - Query Editor
    QCheckBox *chcConfirmClosing = this->findChild<QCheckBox*>("chcConfirmClosing");
    chcConfirmClosing->setChecked(true);

    QCheckBox *chcKeepContent = this->findChild<QCheckBox*>("chcKeepContent");
    chcKeepContent->setChecked(true);

    QCheckBox *chcShowLineNumber = this->findChild<QCheckBox*>("chcShowLineNumber");
    chcShowLineNumber->setChecked(true);

    //Enviroment - Database
    QSpinBox *sbRecentlyUsedDatabases = this->findChild<QSpinBox*>("sbRecentlyUsedDatabases");
    sbRecentlyUsedDatabases->setValue(5);

    QCheckBox *chcOpenLastDatabasesOnStart = this->findChild<QCheckBox*>("chcOpenLastDatabasesOnStart");
    chcOpenLastDatabasesOnStart->setChecked(false);

    //Query Editor - Font
    QFontComboBox *fcmbFamily = this->findChild<QFontComboBox*>("fcmbFamily");
    fcmbFamily->setCurrentText("Sans Serif");

    QSpinBox *sbSize = this->findChild<QSpinBox*>("sbSize");
    sbSize->setValue(9);

    //Query Editor - Text
    QCheckBox *chcEnableTextWrapping = this->findChild<QCheckBox*>("chcEnableTextWrapping");
    chcEnableTextWrapping->setChecked(false);

    //Query Editor - Highlighter
    listaBoja.clear();

    QColor colorNormalText(Qt::black);
    QColor colorNumber(Qt::darkYellow);
    QColor colorStringDoubleSingle(Qt::darkGreen);
    QColor colorKeywords(Qt::darkBlue);
    QColor colorFunctions(Qt::blue);
    QColor colorComment(Qt::gray);

    QPushButton *btnNormalText = this->findChild<QPushButton*>("btnNormalText");
    listaBoja.append(colorNormalText);
    btnNormalText->setIcon(createIcon( listaBoja.at(0) ));

    QPushButton *btnNumber = this->findChild<QPushButton*>("btnNumber");
    listaBoja.append(colorNumber);
    btnNumber->setIcon(createIcon( listaBoja.at(1) ));

    QPushButton *btnStringDouble = this->findChild<QPushButton*>("btnStringDouble");
    listaBoja.append(colorStringDoubleSingle);
    btnStringDouble->setIcon(createIcon( listaBoja.at(2) ));

    QPushButton *btnStringSingle = this->findChild<QPushButton*>("btnStringSingle");
    listaBoja.append(colorStringDoubleSingle);
    btnStringSingle->setIcon(createIcon( listaBoja.at(3) ));

    QPushButton *btnKeywords = this->findChild<QPushButton*>("btnKeywords");
    listaBoja.append(colorKeywords);
    btnKeywords->setIcon(createIcon( listaBoja.at(4) ));

    QPushButton *btnFunctions = this->findChild<QPushButton*>("btnFunctions");
    listaBoja.append(colorFunctions);
    btnFunctions->setIcon(createIcon( listaBoja.at(5) ));

    QPushButton *btnComment = this->findChild<QPushButton*>("btnComment");
    listaBoja.append(colorComment);
    btnComment->setIcon(createIcon( listaBoja.at(6) ));

    //LANGUAGE
    QComboBox *cmbLanguage = this->findChild<QComboBox*>("cmbLanguage");
    cmbLanguage->setCurrentIndex(0);
}

void FrmOptions::saveValues()
{
    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("language");
    QComboBox *cmbLanguage = this->findChild<QComboBox*>("cmbLanguage");

    switch(cmbLanguage->currentIndex())
    {
    case 0://system default
        settingsConfig.setValue("language", "");
        break;
    case 1://american english
        settingsConfig.setValue("language", "en_US");
        break;
    case 2://serbian
        settingsConfig.setValue("language", "sr_RS");
        break;
    case 3://serbian latin
        settingsConfig.setValue("language", "sr_RS@latin");
        break;
    }
    settingsConfig.endGroup();
///////////////////////////////////////////////////////
    settingsConfig.beginGroup("font");

    QFontComboBox *fcmbFamily = this->findChild<QFontComboBox*>("fcmbFamily");
    settingsConfig.setValue("family", fcmbFamily->currentText());

    QSpinBox *sbSize = this->findChild<QSpinBox*>("sbSize");
    settingsConfig.setValue("size", sbSize->value());

    settingsConfig.endGroup();
///////////////////////////////////////////////////////
    settingsConfig.beginGroup("text");

    QCheckBox *chcEnableTextWrapping = this->findChild<QCheckBox*>("chcEnableTextWrapping");
    settingsConfig.setValue("textWrapping",chcEnableTextWrapping->isChecked());

    settingsConfig.endGroup();
///////////////////////////////////////////////////////
    settingsConfig.beginGroup("highlighter");

    settingsConfig.setValue("normalText", listaBoja.at(0));
    settingsConfig.setValue("number", listaBoja.at(1));
    settingsConfig.setValue("stringDouble", listaBoja.at(2));
    settingsConfig.setValue("stringSingle", listaBoja.at(3));
    settingsConfig.setValue("keywords", listaBoja.at(4));
    settingsConfig.setValue("functions", listaBoja.at(5));
    settingsConfig.setValue("comment", listaBoja.at(6));

    settingsConfig.endGroup();
///////////////////////////////////////////////////////
    settingsConfig.beginGroup("Enviroment");

    QSpinBox *sbAutoSave = this->findChild<QSpinBox*>("sbAutoSave");
    settingsConfig.setValue("autosave", sbAutoSave->value());

    QCheckBox *chcConfirmClosing = this->findChild<QCheckBox*>("chcConfirmClosing");
    settingsConfig.setValue("confirmClosing", chcConfirmClosing->isChecked());

    QCheckBox *chcKeepContent = this->findChild<QCheckBox*>("chcKeepContent");
    settingsConfig.setValue("keepContent", chcKeepContent->isChecked());

    QCheckBox *chcShowLineNumber = this->findChild<QCheckBox*>("chcShowLineNumber");
    settingsConfig.setValue("showLineNumber", chcShowLineNumber->isChecked());

    QSpinBox *sbRecentlyUsedDatabases = this->findChild<QSpinBox*>("sbRecentlyUsedDatabases");
    settingsConfig.setValue("recentlyUsedDatabases",  sbRecentlyUsedDatabases->value());

    QCheckBox *chcOpenLastDatabasesOnStart = this->findChild<QCheckBox*>("chcOpenLastDatabasesOnStart");
    settingsConfig.setValue("openLastDatabasesOnStart", chcOpenLastDatabasesOnStart->isChecked());

    settingsConfig.endGroup();
}

QIcon FrmOptions::createIcon(QColor color)
{
    QPixmap pixmap(16,16);
    pixmap.fill(color);
    QIcon icon(pixmap);

    return icon;
}
