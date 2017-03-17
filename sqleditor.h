#ifndef SQLEDITOR_H
#define SQLEDITOR_H

#include <QPlainTextEdit>
#include <QtWidgets>
#include <QtGui>
#include <QtCore>

#include <kolistview.h>
#include <sqlcompleterloader.h>

class SqlEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit SqlEditor(QWidget *parent = 0);
    ~SqlEditor();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

    void setWrapText(bool wrap);

    void setShowLineNumber(bool state);

signals:

public slots:

protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &, int);
    void insertCompletion(const QString &completion);

private:

    QString textUnderCursor() const;

    QWidget *lineNumberArea;

    QCompleter *c;

    KoListView *lv;

    bool showLineNumber;    

    SqlCompleterLoader *scl;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(SqlEditor *editor) : QWidget(editor) {
        queryEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(queryEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        queryEditor->lineNumberAreaPaintEvent(event);
    }

private:
    SqlEditor *queryEditor;

};

#endif // SQLEDITOR_H
