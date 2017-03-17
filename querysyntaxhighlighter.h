#ifndef QUERYSYNTAXHIGHLIGHTER_H
#define QUERYSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QApplication>
#include <QDebug>

class QuerySyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    QuerySyntaxHighlighter(QTextDocument *parent = 0);

public slots:
    void changeColors(QList<QColor> colors);

protected:
    void highlightBlock(const QString &text);

private:
    typedef enum {
        Background,
        Normal,
        Comment,
        Number,
        StringDouble,
        StringSingle,
        Identifier,
        Keyword,
        Function,
        Sidebar,
        LineNumber,
        Cursor,
        Marker,
        BracketMatch,
        BracketError,
        FoldIndicator
    } ColorComponent;

    enum
    {
        TypeStart = 0,
        TypeNumber = 1,
        TypeIdentifier = 2,
        TypeStringSingle = 3,
        TypeStringDouble = 4,
        TypeComment = 5
    };

    QHash<ColorComponent, QColor> m_colors;
    QString m_markString;
    Qt::CaseSensitivity m_markCaseSensitivity;
    QStringList m_keywords;
    QStringList m_functions;

};

class JSBlockData: public QTextBlockUserData
{
public:
    QList<int> bracketPositions;
};

#endif // QUERYSYNTAXHIGHLIGHTER_H
