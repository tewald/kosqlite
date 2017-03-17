#include "frmfindandreplace.h"
#include "ui_frmfindandreplace.h"

FrmFindAndReplace::FrmFindAndReplace(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmFindAndReplace)
{
    ui->setupUi(this);
    txtFindLineEdit = new FindLineEdit(this);

    menuSearchOptions = new QMenu(this);
    //postaviti ikone, i da su checkable
    actionCaseSensitive = new QAction(tr("Case Sensitive"), this);
    actionCaseSensitive->setCheckable(true);
    actionWholeWordsOnly = new QAction(tr("Whole Words Only"), this);
    actionWholeWordsOnly->setCheckable(true);

    menuSearchOptions->addAction(actionCaseSensitive);
    menuSearchOptions->addAction(actionWholeWordsOnly);

    txtFindLineEdit->searchButtonContextMenu()->setMenu(menuSearchOptions);

    connect(txtFindLineEdit,SIGNAL(textChanged(QString)),this,SLOT(txtFindLineEdit_textChanged(QString)));
    connect(actionWholeWordsOnly,SIGNAL(triggered()),this,SLOT(actionWholeWordsOnly_triggered()));
    connect(actionCaseSensitive,SIGNAL(triggered()),this,SLOT(actionCaseSensitive_triggered()));

    ui->gridLayout_2->addWidget(txtFindLineEdit, 0,1,1,1);

    txtFindLineEdit->setFocus();
}

FrmFindAndReplace::~FrmFindAndReplace()
{
    delete ui;
}

void FrmFindAndReplace::findReplace(QString wordUnderCursor)
{
    txtFindLineEdit->setText(wordUnderCursor);
}

void FrmFindAndReplace::closeEvent(QCloseEvent *event)
{
   //iako na prvi pogled ovo nema smisla, ovo je tu potrebno, da bi se selekcija očistila
    txtFindLineEdit->clear();
    ui->txtReplace->clear();

    //ovo treba false da bi deaktivirao prozor
    emit finished(false);
    emit finished();

    QDialog::closeEvent(event);
}

void FrmFindAndReplace::on_btnPrevious_clicked()
{
    QTextDocument::FindFlags findFlags;

    if (actionCaseSensitive->isChecked())
        findFlags |= QTextDocument::FindCaseSensitively;
    if (actionWholeWordsOnly->isChecked())
        findFlags |= QTextDocument::FindWholeWords;

    findFlags |= QTextDocument::FindBackward;//PREVIOUS pretraga ide unatraške

    emit btnPrevious(txtFindLineEdit->text(), findFlags);
}

void FrmFindAndReplace::on_btnNext_clicked()
{
    QTextDocument::FindFlags findFlags;

    if (actionCaseSensitive->isChecked())
        findFlags |= QTextDocument::FindCaseSensitively;
    if (actionWholeWordsOnly->isChecked())
        findFlags |= QTextDocument::FindWholeWords;

    emit btnNext(txtFindLineEdit->text(), findFlags);
}

void FrmFindAndReplace::on_btnReplace_clicked()
{
    QTextDocument::FindFlags findFlags;

    if (actionCaseSensitive->isChecked())
        findFlags |= QTextDocument::FindCaseSensitively;
    if (actionWholeWordsOnly->isChecked())
        findFlags |= QTextDocument::FindWholeWords;

    emit btnReplace(txtFindLineEdit->text(), ui->txtReplace->text(), findFlags);
}

void FrmFindAndReplace::on_btnReplaceAll_clicked()
{
    QTextDocument::FindFlags findFlags;

    if (actionCaseSensitive->isChecked())
        findFlags |= QTextDocument::FindCaseSensitively;
    if (actionWholeWordsOnly->isChecked())
        findFlags |= QTextDocument::FindWholeWords;

    emit btnReplaceAll(txtFindLineEdit->text(), ui->txtReplace->text(), findFlags);
}

void FrmFindAndReplace::txtFindLineEdit_textChanged(const QString &arg1)
{
    QTextDocument::FindFlags findFlags;

    if (actionCaseSensitive->isChecked())
        findFlags |= QTextDocument::FindCaseSensitively;
    if (actionWholeWordsOnly->isChecked())
        findFlags |= QTextDocument::FindWholeWords;
    emit txtFind(arg1, findFlags);
}

void FrmFindAndReplace::actionWholeWordsOnly_triggered()
{
    QTextDocument::FindFlags findFlags;

    if (actionCaseSensitive->isChecked())
        findFlags |= QTextDocument::FindCaseSensitively;
    if (actionWholeWordsOnly->isChecked())
        findFlags |= QTextDocument::FindWholeWords;

    emit txtFind(txtFindLineEdit->text(), findFlags);
}

void FrmFindAndReplace::actionCaseSensitive_triggered()
{
    QTextDocument::FindFlags findFlags;

    if (actionCaseSensitive->isChecked())
        findFlags |= QTextDocument::FindCaseSensitively;
    if (actionWholeWordsOnly->isChecked())
        findFlags |= QTextDocument::FindWholeWords;

    emit txtFind(txtFindLineEdit->text(), findFlags);
}

void FrmFindAndReplace::textChanged()
{
    QTextDocument::FindFlags findFlags;

    if (actionCaseSensitive->isChecked())
        findFlags |= QTextDocument::FindCaseSensitively;
    if (actionWholeWordsOnly->isChecked())
        findFlags |= QTextDocument::FindWholeWords;

    emit txtFind(txtFindLineEdit->text(), findFlags);
}

void FrmFindAndReplace::on_btnClose_clicked()
{
    this->close();
}
