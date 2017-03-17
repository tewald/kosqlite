#include "pages.h"

LanguagePage::LanguagePage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    QGroupBox *groupLanguage = new QGroupBox(this);
    groupLanguage->setTitle(tr("Language"));

    layout->addWidget(groupLanguage);

    QGridLayout *gridLayout = new QGridLayout(groupLanguage);
    groupLanguage->setLayout(gridLayout);
    gridLayout->setSpacing(20);

    QLabel *labelText = new QLabel(this);
    labelText->setText(tr("Please choose language which should be used for this application."));

    QLabel *language = new QLabel(this);
    language->setText(tr("Language:"));

    QLabel *notification = new QLabel(this);
    notification->setText(tr("*This change will take effect the next time you start the application."));

    QComboBox *cmbLanguage = new QComboBox(this);
    cmbLanguage->setObjectName("cmbLanguage");
    cmbLanguage->addItem(tr("System Default"));
    cmbLanguage->addItem(tr("English"));
    cmbLanguage->addItem(tr("Serbian"));
    cmbLanguage->addItem(tr("Serbian Latin"));

    QSpacerItem *spacer = new QSpacerItem(20,20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addWidget(labelText,0,0,1,3);
    gridLayout->addWidget(language,1,0,1,1);
    gridLayout->addWidget(cmbLanguage,1,1,1,1);
    gridLayout->addItem(spacer,1,2,1,1);
    gridLayout->addWidget(notification,2,0,1,3);

    QSpacerItem *hSpacer = new QSpacerItem(0,20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(hSpacer);
}

QueryEditorPage::QueryEditorPage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    QGroupBox *groupFont = new QGroupBox(this);
    groupFont->setTitle(tr("Font"));

    QGroupBox *groupText = new QGroupBox(this);
    groupText->setTitle(tr("Text"));

    QGroupBox *groupHighlighter = new QGroupBox(this);
    groupHighlighter->setTitle(tr("Highlighter"));

    layout->addWidget(groupFont);
    layout->addWidget(groupText);
    layout->addWidget(groupHighlighter);

    //group box font komponente
    QGridLayout *gridFont = new QGridLayout(groupFont);
    groupFont->setLayout(gridFont);
    gridFont->setVerticalSpacing(20);

    QLabel *lblFamily = new QLabel(groupFont);
    lblFamily->setObjectName("lblFamily");
    lblFamily->setText(tr("Family: "));

    QFontComboBox *fcmbFamily = new QFontComboBox(groupFont);
    fcmbFamily->setObjectName("fcmbFamily");

    QLabel *lblSize = new QLabel(groupFont);
    lblSize->setObjectName("lblSize");
    lblSize->setText(tr("Size: "));

    QSpinBox *sbSize = new QSpinBox(groupFont);
    sbSize->setObjectName("sbSize");
    sbSize->setMinimum(6);
    sbSize->setMaximum(72);
    sbSize->setValue(9);
    sbSize->setSuffix(" pt.");

    QSpacerItem *sizeSpacer = new QSpacerItem(20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QSpacerItem *familySpacer = new QSpacerItem(50,0,QSizePolicy::Fixed, QSizePolicy::Minimum);

    gridFont->addWidget(lblFamily,0,0,1,1);
    gridFont->addWidget(fcmbFamily,0,1,1,1);
    gridFont->addItem(familySpacer,0,2);
    gridFont->addWidget(lblSize, 0,3,1,1);
    gridFont->addWidget(sbSize, 0,4,1,1);
    gridFont->addItem(sizeSpacer,0,5);

    //group box text componente
    QGridLayout *gridText = new QGridLayout(groupText);
    groupText->setLayout(gridText);
    gridText->setSpacing(20);

    QCheckBox *chcEnableTextWrapping = new QCheckBox(groupText);
    chcEnableTextWrapping->setObjectName("chcEnableTextWrapping");
    chcEnableTextWrapping->setText(tr("Enable Text Wrapping"));

    gridText->addWidget(chcEnableTextWrapping, 0,0,1,1);

    //group box highligher componente

    QGridLayout *gridHighlighter = new QGridLayout(groupHighlighter);
    groupHighlighter->setLayout(gridHighlighter);
    gridHighlighter->setVerticalSpacing(20);

    QLabel *lblNormalText = new QLabel(groupHighlighter);
    lblNormalText->setText(tr("Normal Text"));

    QPushButton *btnNormalText = new QPushButton(groupHighlighter);
    btnNormalText->setObjectName("btnNormalText");

    QLabel *lblNumber = new QLabel(groupHighlighter);
    lblNumber->setText(tr("Number"));

    QPushButton *btnNumber = new QPushButton(groupHighlighter);
    btnNumber->setObjectName("btnNumber");

    QLabel *lblStringDouble = new QLabel(groupHighlighter);
    lblStringDouble->setText(tr("Double Quotes String"));

    QPushButton *btnStringDouble = new QPushButton(groupHighlighter);
    btnStringDouble->setObjectName("btnStringDouble");

    QLabel *lblStringSingle = new QLabel(groupHighlighter);
    lblStringSingle->setText(tr("Single Quotes String"));

    QPushButton *btnStringSingle = new QPushButton(groupHighlighter);
    btnStringSingle->setObjectName("btnStringSingle");

    QLabel *lblKeyword = new QLabel(groupHighlighter);
    lblKeyword->setText(tr("Keywords"));

    QPushButton *btnKeyword = new QPushButton(groupHighlighter);
    btnKeyword->setObjectName("btnKeywords");

    QLabel *lblFunctions = new QLabel(groupHighlighter);
    lblFunctions->setText(tr("Functions"));

    QPushButton *btnFunctions = new QPushButton(groupHighlighter);
    btnFunctions->setObjectName("btnFunctions");

    QLabel *lblComment = new QLabel(groupHighlighter);
    lblComment->setText(tr("Comment"));

    QPushButton *btnComment = new QPushButton(groupHighlighter);
    btnComment->setObjectName("btnComment");

    QPushButton *btnResetColors = new QPushButton(tr("Reset Colors"));
    btnResetColors->setObjectName("btnResetColors");

    QSpacerItem *spacerFont1 = new QSpacerItem(120,20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    QSpacerItem *spacerFont2 = new QSpacerItem(20,20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridHighlighter->addWidget(lblNormalText,0,0,1,1);
    gridHighlighter->addWidget(btnNormalText,0,1,1,1);
    gridHighlighter->addWidget(lblNumber,1,0,1,1);
    gridHighlighter->addWidget(btnNumber,1,1,1,1);
    gridHighlighter->addWidget(lblStringDouble,2,0,1,1);
    gridHighlighter->addWidget(btnStringDouble,2,1,1,1);
    gridHighlighter->addWidget(lblStringSingle,3,0,1,1);
    gridHighlighter->addWidget(btnStringSingle,3,1,1,1);

    gridHighlighter->addItem(spacerFont1,0,3,1,1);

    gridHighlighter->addWidget(lblKeyword,0,4,1,1);
    gridHighlighter->addWidget(btnKeyword,0,5,1,1);
    gridHighlighter->addWidget(lblFunctions,1,4,1,1);
    gridHighlighter->addWidget(btnFunctions,1,5,1,1);
    gridHighlighter->addWidget(lblComment,2,4,1,1);
    gridHighlighter->addWidget(btnComment,2,5,1,1);
    gridHighlighter->addWidget(btnResetColors,3,4,1,2);
    gridHighlighter->addItem(spacerFont2,0,6);

    QSpacerItem *hSpacer = new QSpacerItem(0,20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(hSpacer);
}

EnviromentPage::EnviromentPage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    setLayout(vLayout);

    QGroupBox *groupAutoSave = new QGroupBox(this);
    groupAutoSave->setTitle(tr("Auto Save"));

    QGroupBox *groupQueryEditor = new QGroupBox(this);
    groupQueryEditor->setTitle(tr("Query Editor"));

    QGroupBox *groupDatabase = new QGroupBox(this);
    groupDatabase->setTitle(tr("Database"));

    vLayout->addWidget(groupAutoSave);
    vLayout->addWidget(groupQueryEditor);
    vLayout->addWidget(groupDatabase);

    //group auto save
    QLabel *lblAutoSave = new QLabel(groupAutoSave);
    lblAutoSave->setText(tr("Auto Save on every"));

    QSpinBox *sbAutoSave = new QSpinBox(groupAutoSave);
    sbAutoSave->setObjectName("sbAutoSave");
    sbAutoSave->setMinimum(0);
    sbAutoSave->setMaximum(360);
    sbAutoSave->setValue(5);
    sbAutoSave->setSpecialValueText(tr("Disabled"));

    QLabel *lblMinute = new QLabel(this);
    lblMinute->setText(tr("Minute."));

    QHBoxLayout *layoutAutoSave = new QHBoxLayout(groupAutoSave);
    groupAutoSave->setLayout(layoutAutoSave);
    layoutAutoSave->setSpacing(20);

    QSpacerItem *spacer = new QSpacerItem(20,20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    layoutAutoSave->addWidget(lblAutoSave);
    layoutAutoSave->addWidget(sbAutoSave);
    layoutAutoSave->addWidget(lblMinute);
    layoutAutoSave->addItem(spacer);

    //group query editor
    QCheckBox *chcConfirmClosing = new QCheckBox(groupQueryEditor);
    chcConfirmClosing->setObjectName("chcConfirmClosing");
    chcConfirmClosing->setText(tr("Confirm closing of unsaved SQL query documents"));

    QCheckBox *chcKeepContent = new QCheckBox(groupQueryEditor);
    chcKeepContent->setObjectName("chcKeepContent");
    chcKeepContent->setText(tr("On start load last edited SQL queries."));

    QCheckBox *chcShowLineNumber = new QCheckBox(groupQueryEditor);
    chcShowLineNumber->setObjectName("chcShowLineNumber");
    chcShowLineNumber->setText(tr("Show line number"));

    QVBoxLayout *layoutQueryEditor = new QVBoxLayout(groupQueryEditor);
    groupQueryEditor->setLayout(layoutQueryEditor);
    layoutQueryEditor->setSpacing(20);

    layoutQueryEditor->addWidget(chcConfirmClosing);
    layoutQueryEditor->addWidget(chcKeepContent);
    layoutQueryEditor->addWidget(chcShowLineNumber);

    //database
    QLabel *lblRecentlyUsedDatabases = new QLabel(tr("Recently Used Databases"),this);

    QSpinBox *sbRecentlyUsedDatabases = new QSpinBox(this);
    sbRecentlyUsedDatabases->setObjectName("sbRecentlyUsedDatabases");
    sbRecentlyUsedDatabases->setMinimum(1);
    sbRecentlyUsedDatabases->setMaximum(20);
    sbRecentlyUsedDatabases->setValue(5);

    QCheckBox *chcOpenLastDatabasesOnStart = new QCheckBox(tr("Open Last Databases On Start"), this);
    chcOpenLastDatabasesOnStart->setObjectName("chcOpenLastDatabasesOnStart");

    QGridLayout *layoutDatabase = new QGridLayout(groupDatabase);
    groupDatabase->setLayout(layoutDatabase);

    layoutDatabase->addWidget(lblRecentlyUsedDatabases,0,0,1,1);
    layoutDatabase->addWidget(sbRecentlyUsedDatabases,0,1,1,1);
    layoutDatabase->addWidget(chcOpenLastDatabasesOnStart,1,0,1,2);    

    QSpacerItem *hSpacer = new QSpacerItem(0,20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vLayout->addItem(hSpacer);

    QLabel *lblNote = new QLabel(this);
    lblNote->setTextFormat(Qt::RichText);
    lblNote->setText("<span style='font-size:10pt; color:#ff0000;'>NOTE:</span> If multiple instances of KoSQLite is running,<br>auto saves will be in conflict and override each other.");

    vLayout->addWidget(lblNote);
}
