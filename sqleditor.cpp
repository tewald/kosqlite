#include "sqleditor.h"

SqlEditor::SqlEditor(QWidget *parent) :
    QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("font");
    QFont font;
    font.setFamily(settingsConfig.value("family", "Sans Serif").toString());
    font.setPointSize(settingsConfig.value("size",9).toInt());

    setFont(font);
    settingsConfig.endGroup();

    settingsConfig.beginGroup("text");
    setWrapText( settingsConfig.value("textWrapping", false).toBool() );
    settingsConfig.endGroup();

    settingsConfig.beginGroup("Enviroment");
    showLineNumber = settingsConfig.value("showLineNumber", true).toBool();
    settingsConfig.endGroup();

    this->setUndoRedoEnabled(true);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

    updateLineNumberAreaWidth(0);

    c = new QCompleter(this);

    scl = new SqlCompleterLoader;
    //učitavanje podataka za completer
    QStandardItemModel *model = scl->completerModel();

    //custom popup
    lv = new KoListView(this);
    c->setPopup(lv);

    c->setModel(model);
    c->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    c->setCaseSensitivity(Qt::CaseInsensitive);

    this->setCompleter(c);
}

SqlEditor::~SqlEditor()
{
    delete scl;
}

void SqlEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignCenter, number);//AlignCenter je centriranje linije u widgetu
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int SqlEditor::lineNumberAreaWidth()
{
    if (showLineNumber == true)
    {
        int digits = 1;
        int max = qMax(1, blockCount());
        while (max >= 10) {
            max /= 10;
            ++digits;
        }
        //širina polja gde su linije ispisane
        int space = 10 + (fontMetrics().width(QLatin1Char(this->font().pointSize())) + 3) * digits;

        return space;
    }

    return 0;
}

void SqlEditor::setCompleter(QCompleter *c)
{
    if (c)
        QObject::disconnect(c, 0, this, 0);

    this->c = c;

    if (!this->c)
        return;

    this->c->setWidget(this);
    this->c->setCompletionMode(QCompleter::PopupCompletion);
    this->c->popup()->setMinimumWidth(60);
    //mouse event na overridovan popup
    connect(this->lv,SIGNAL(activated(QString)),this,SLOT(insertCompletion(QString)));
    //keypress/keyrelease event na completeru
    connect(this->c, SIGNAL(activated(QString)), this,SLOT(insertCompletion(QString)));
}

QCompleter *SqlEditor::completer() const
{
    return this->c;
}

void SqlEditor::setWrapText(bool wrap)
{
    if (wrap == true)
    {
        this->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        this->setWordWrapMode(QTextOption::WordWrap);//ovo je potrebno samo ako je lineWrapMode widgetwidth
    }
    else
    {
        this->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void SqlEditor::setShowLineNumber(bool state)
{
    showLineNumber = state;

    updateLineNumberAreaWidth(0);
}

void SqlEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void SqlEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->isAccepted())
    {
        switch (event->key())
        {
        case Qt::Key_Slash:
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                QTextCursor tCursor = this->textCursor();

                tCursor.beginEditBlock();

                if (tCursor.hasSelection())
                {
                    QString selectedText = tCursor.selectedText();

                    selectedText.insert(0,"/*");
                    selectedText.insert(selectedText.count(), "*/");
                    tCursor.insertText(selectedText);
                }
                else
                {
                    int cursorPosition = tCursor.position();

                    //pomeranje na početak linije
                    tCursor.movePosition(QTextCursor::StartOfLine);
                    //selektovanje prvog karaktera
                    tCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

                    //provera da lije prvi karakter #, ako jeste onda je potrebno obrisati #, ako ne onda dodati #
                    if (tCursor.selectedText() == "#")
                    {
                        int startOfLinePosition = tCursor.position();
                        tCursor.removeSelectedText();
                        tCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, cursorPosition-startOfLinePosition);//vraćanje kursora na poziciju gde je i bio
                    }
                    else
                    {
                        tCursor.movePosition(QTextCursor::StartOfLine);
                        int startOfLinePosition = tCursor.position();
                        tCursor.insertText("#");//na početak linije se ubacuje taraba koja služi za komentar
                        tCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, cursorPosition-startOfLinePosition);//vraćanje kursora na poziciju gde je i bio
                    }
                }

                tCursor.endEditBlock();

                this->setTextCursor(tCursor);
            }
            else
                QPlainTextEdit::keyPressEvent(event);

            event->accept();
            return;
        }
            break;

        case Qt::Key_QuoteDbl:
        {
            QTextCursor tCursor = this->textCursor();

            if (tCursor.selectedText().isEmpty())
            {
                tCursor.insertText("\"");
                //tCursor.insertText("\"\"");
                //tCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
            }
            else
            {
                QString selectedText = tCursor.selectedText();
                selectedText = selectedText.insert(0, "\"");
                selectedText = selectedText.insert(selectedText.count(), "\"");
                tCursor.insertText(selectedText);
            }

            this->setTextCursor(tCursor);

            event->accept();
            return;
        }
            break;

        case Qt::Key_Apostrophe:
        {
            QTextCursor tCursor = this->textCursor();

            if (tCursor.selectedText().isEmpty())
            {
                tCursor.insertText("'");
                //tCursor.insertText("''");
                //tCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
            }
            else
            {
                QString selectedText = tCursor.selectedText();
                selectedText = selectedText.insert(0, "'");
                selectedText = selectedText.insert(selectedText.count(), "'");
                tCursor.insertText(selectedText);
            }

            this->setTextCursor(tCursor);

            event->accept();
            return;
        }
            break;

        case Qt::Key_ParenLeft:
        {
            QTextCursor tCursor = this->textCursor();

            if (tCursor.selectedText().isEmpty())
            {
                tCursor.insertText("(");
                //tCursor.insertText("()");
                //tCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);
            }
            else
            {
                QString selectedText = tCursor.selectedText();
                selectedText = selectedText.insert(selectedText.count(), ")");
                selectedText = selectedText.insert(0, "(");
                tCursor.insertText(selectedText);
            }

            this->setTextCursor(tCursor);

            event->accept();
            return;
        }
            break;

        case Qt::Key_Delete:
        {
            if (event->modifiers() & Qt::ShiftModifier)
            {
                QTextCursor tCursor = this->textCursor();

                if (!tCursor.hasSelection())
                {
                    tCursor.beginEditBlock();

                    if (tCursor.blockNumber() != this->blockCount() - 1)//ukoliko nije poslednje
                    {
                        tCursor.movePosition(QTextCursor::StartOfLine);
                        tCursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                        QApplication::clipboard()->setText(tCursor.selectedText());//postavka selektovanog teksta u clipbord
                        tCursor.removeSelectedText();
                    }
                    else if (tCursor.blockNumber() == this->blockCount() - 1 && tCursor.blockNumber() != 0)//ukoliko je poslednje, ali nije prvo
                    {
                        tCursor.movePosition(QTextCursor::PreviousBlock);
                        tCursor.movePosition(QTextCursor::EndOfBlock);
                        tCursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                        tCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                        QApplication::clipboard()->setText(tCursor.selectedText());//postavka selektovanog teksta u clipbord
                        tCursor.removeSelectedText();
                        tCursor.movePosition(QTextCursor::StartOfLine);
                    }
                    else //ukolio je prvo
                    {
                        tCursor.movePosition(QTextCursor::StartOfLine);
                        tCursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                        QApplication::clipboard()->setText(tCursor.selectedText());//postavka selektovanog teksta u clipbord
                        tCursor.removeSelectedText();
                    }

                    tCursor.endEditBlock();

                    this->setTextCursor(tCursor);
                }
                else
                    QPlainTextEdit::keyPressEvent(event);
            }
            else
            {
                QPlainTextEdit::keyPressEvent(event);
            }

            event->accept();
            return;
        }
            break;
        case Qt::Key_Home:
        {
            QTextCursor tCursor = this->textCursor();

            //ako je cursor na početku reda onda nema potrebe stavljati ga na početak
            if (!tCursor.atStart())
            {
                //pozicija odakle kreće kursor
                int orgPosition = tCursor.position();
                //ukoliko već ima selekcija gde je kraj taj selekcije
                int endSelection = -1;

                if (!(event->modifiers() & Qt::ShiftModifier))//shift nije stisnut
                    endSelection = orgPosition;
                else
                    endSelection = tCursor.selectionEnd();

                tCursor.clearSelection();

                //selektujem zato što mi treba tekst od te linije da ispipam da li na početku ima taba ili ne
                tCursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);

                //početak linije
                int startPosition = tCursor.position();

                QString textInLine = tCursor.selectedText();

                int br = 0;

                //sve dok je tab računaj
                for (int i = 0; i < textInLine.count(); i++)
                {
                    if ('\t' == textInLine.at(i))
                    {
                        br++;
                    }
                    else
                        break;
                }

                //ukoliko je stisnut shift onda se mora odraditi i selekcija
                if (event->modifiers() & Qt::ShiftModifier)
                {
                    tCursor.clearSelection();
                    tCursor.setPosition(endSelection);

                    //ukoliko je brojač i veličina stringa koja treba da se prebroji ista to znači da je pokazivač ispred teksta, tj. npr. samo \t\t\t je ispred
                    if (br == textInLine.count())
                        tCursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
                    else//ako ima taba, onda kursor treba da se pojavi startna pozivija plus broj tabova
                        tCursor.setPosition( startPosition + br, QTextCursor::KeepAnchor);
                    //pošto se selekcija ovde čisti kursor vraćam na originalnu poziciju i postavljam je startna + brojač
                }
                else
                {
                    tCursor.clearSelection();
                    //ukoliko je brojač i veličina stringa koja treba da se prebroji ista to znači da je pokazivač ispred teksta, tj. npr. samo \t\t\t je ispred
                    if (br == textInLine.count())
                        tCursor.movePosition(QTextCursor::StartOfLine);
                    else
                        tCursor.setPosition( startPosition + br );
                }

                this->setTextCursor(tCursor);
            }

            event->accept();
            return;
        }
            break;
        default://sve ostale funkcionalnosti koje je Qt ugradio ostaju (SEM HOME DUGMETA) , I QCOMPLETER
        {
            if (c && c->popup()->isVisible())
            {
                // The following keys are forwarded by the completer to the widget
                switch (event->key())
                {
                case Qt::Key_Return:
                case Qt::Key_Escape:
                case Qt::Key_Tab:
                case Qt::Key_Backtab:
                    event->ignore();// let the completer do default behavior
                    return;
                default:
                    break;
                }
            }

            //briše se polje unazad, pa se proverava tekst ispod kursora, ukoliko je prazan onda se completer zatvara (pod uslovom da je već bio otvoren)
            //ako je tekst ispod kursora prazno ili ne postoji nijedno poklapanje u kompleteru kompleter se takođe zatvara, a ako ima poklapanja onda kompleter
            //ostaje prikazan
            if ( (event->key() == Qt::Key_Backspace ? true : false) == true )
            {
                QPlainTextEdit::keyPressEvent(event);

                if (c->popup()->isVisible())
                {
                    QString completionPrefix = textUnderCursor();

                    if (completionPrefix.isEmpty())
                        c->popup()->close();
                    else
                    {
                        c->setCompletionPrefix(completionPrefix);
                        if (c->completionCount() > 0)
                            c->popup()->adjustSize();
                        else
                            c->popup()->close();
                    }
                }
                return;
            }

            bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Space); // CTRL+Space

            if (!c || !isShortcut) // dont process the shortcut when we have a completer
                QPlainTextEdit::keyPressEvent(event);
            //We also handle other modifiers and shortcuts for which we do not want the completer to respond to.

            //ako je bilo koja kombinacija sa ctrl ili shift stisnuta completer verovatno je prečica, ne radi ništa
            const bool ctrlModifier = event->modifiers() & (Qt::ControlModifier/* | Qt::ShiftModifier*/);
            if (!c || (ctrlModifier && event->key() != Qt::Key_Space))
            //if (!c || (ctrlOrShift && event->text().isEmpty())
                return;

            const bool shiftModifier = event->modifiers() & (Qt::ShiftModifier);

            ///ovde su tasteri na kojima iskoči kompleter a ne bi trebao            
            switch (event->key())
            {
            case Qt::Key_Backtab:
            {
                //shift + tab
                QTextCursor tCursor = this->textCursor();
                tCursor.clearSelection();
                tCursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

                if (tCursor.selectedText() == "\t")
                    tCursor.removeSelectedText();

                event->accept();
                return;
            }
            break;
            case Qt::Key_Tab:
            case Qt::Key_Shift:
                event->accept();
                return;
                break;
            default:
                break;
            }

            static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
            bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlModifier;
            QString completionPrefix = textUnderCursor();

            if (!isShortcut && (hasModifier || event->text().isEmpty() || completionPrefix.length() < 1 || eow.contains(event->text().right(1))) && shiftModifier == false)
            {
                c->popup()->hide();
                return;
            }

            /////////@brief --ovo je tu da bi kompleter mogao dase prikaze i slučaju je ispred neki specijalan znak NPR . , ( i slično
            if (completionPrefix != c->completionPrefix())
            {
                c->setCompletionPrefix(completionPrefix);
                c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
            }

            ///@brief --provera da li je kompleter pozvan pomoću prečice
            if (isShortcut == false)
            {
                /////////@brief --ukoliko se completionPrefix poklopa sa trenutnim completion-om i ako je naravno poslednji, onda se completer ne prikazuje
                if (c->completionCount() == 1 && completionPrefix == c->currentCompletion())
                {
                    c->popup()->hide();
                    return;
                }
                else
                {
                    ////////@brief --provera da li je kucano nešto što completer može da završi, ako nije onda se ni ne prikazuje
                    int brojacPogodaka = 0;
                    for (int i = 0; c->setCurrentRow(i); i++)
                    {
                        ///iako je postavljeno da je completer case insensitive, moram ovde navesti casinsensitive jer upoređivanje je casesensitive
                        if (c->currentCompletion().startsWith(completionPrefix,Qt::CaseInsensitive))
                            brojacPogodaka++;
                    }

                    if (brojacPogodaka == 0)
                    {
                        c->popup()->hide();
                        return;
                    }
                }
            }///@brief --kompleter nije pozvan pomoću prečice
            else
            {
                ///@brief --poslednji karakter je eow i u tom slučaju kompleter se popunjava
                if (eow.contains(completionPrefix.right(1)))
                {
                    c->setCompletionPrefix("");
                    c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
                }
                else///poslednji karakter nije eow
                {
                    ///@brief --provera da li je ta reč već popunjena tj postoji
                    int brojacPogodaka = 0;
                    for (int i = 0; c->setCurrentRow(i); i++)
                    {
                        ///iako je postavljeno da je completer case insensitive, moram ovde navesti casinsensitive jer upoređivanje je casesensitive
                        if (c->currentCompletion().startsWith(completionPrefix,Qt::CaseInsensitive))
                            brojacPogodaka++;
                    }
                    ///@brief --nema pogodaka znači da je kucano nešto bezveze
                    if (brojacPogodaka == 0)
                    {
                        c->popup()->hide();
                        return;
                    }///@brief --ima pogodaka, ali je to unikatan pogodak pa se ne prikazuej kompleter
                    else if (c->completionCount() == 1 && completionPrefix == c->currentCompletion())
                    {
                        c->popup()->hide();
                        return;
                    }
                }
            }

            ////////@brief -- proračun gde će se kompleter prikazati na sqlEditoru
            QRect cr = cursorRect();
            QFontMetrics fm(this->font());

            cr.setLeft(cr.left() - fm.width(completionPrefix) + 6);
            cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());

            c->complete(cr);
            c->popup()->adjustSize();
        }
            break;
        }
    }
}

void SqlEditor::focusInEvent(QFocusEvent *event)
{
    if (this->c)
        this->c->setWidget(this);
    QPlainTextEdit::focusInEvent(event);
}

void SqlEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void SqlEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void SqlEditor::insertCompletion(const QString &completion)
{
    if (this->c->widget() != this)
        return;

    QTextCursor tc = textCursor();
    int extra = completion.length() - c->completionPrefix().length();
    //tc.movePosition(QTextCursor::Left); //ovo je višak u odnosu na primer koji mi je dat
    //tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));

    //if (completion.contains("()"))//ukoliko completer sadrži zagrade () tj. funkcija je u pitanju onda kursor ubaciti između zagrada
        //tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1);

    setTextCursor(tc);
}

QString SqlEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();

    if (!tc.atBlockStart())
        tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

    if (tc.selectedText() == " ")
        return "";
    else
        tc.select(QTextCursor::WordUnderCursor);

    return tc.selectedText();
}
