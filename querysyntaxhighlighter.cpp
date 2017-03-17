#include "querysyntaxhighlighter.h"

QuerySyntaxHighlighter::QuerySyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());

    settingsConfig.beginGroup("highlighter");
    m_colors[Normal]     = settingsConfig.value("normalText", QColor(Qt::black)).value<QColor>();
    m_colors[Comment]    = settingsConfig.value("comment", QColor(Qt::gray)).value<QColor>();
    m_colors[Number]     = settingsConfig.value("number", QColor(Qt::darkYellow)).value<QColor>();
    m_colors[StringDouble]     = settingsConfig.value("stringDouble", QColor(Qt::darkGreen)).value<QColor>();
    m_colors[StringSingle]     = settingsConfig.value("stringSingle", QColor(Qt::darkGreen)).value<QColor>();
    //m_colors[Identifier] = QColor(Qt::yellow);//ovo još ne znam šta je
    m_colors[Keyword]    = settingsConfig.value("keywords", QColor(Qt::darkBlue)).value<QColor>();
    m_colors[Function]   = settingsConfig.value("functions", QColor(Qt::blue)).value<QColor>();
    //m_colors[Marker]     = QColor(Qt::cyan);//ovo još ne znam šta je
    settingsConfig.endGroup();

    QFile keywordFile(":/api/api/keywords.api");
    if (keywordFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&keywordFile);
        while ( !in.atEnd() )
        {
            m_keywords << in.readLine().split(";;;").first();
        }
        keywordFile.close();
    }

    QFile functionFile(":/api/api/functions.api");
    if (functionFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&functionFile);
        while ( !in.atEnd() )
        {
            m_functions << in.readLine().split(";;;").first();
        }
        functionFile.close();
    }
}

void QuerySyntaxHighlighter::changeColors(QList<QColor> colors)
{
    m_colors[Normal]     = colors.at(0);
    m_colors[Comment]    = colors.at(1);
    m_colors[Number]     = colors.at(2);
    m_colors[StringDouble]     = colors.at(3);
    m_colors[StringSingle]     = colors.at(4);
    //m_colors[Identifier] = QColor(Qt::yellow);//ovo još ne znam šta je
    m_colors[Keyword]    = colors.at(5);
    m_colors[Function]   = colors.at(6);
    //m_colors[Marker]     = QColor(Qt::cyan);//ovo još ne znam šta je

    rehighlight();
}

void QuerySyntaxHighlighter::highlightBlock(const QString &text)
{
    QList<int> bracketPositions;

    int blockState = previousBlockState();
    int bracketLevel = blockState >> 6;
    int state = blockState & 15;
    if (blockState < 0)
    {
        bracketLevel = 0;
        state = TypeStart;
    }

    int start = 0;
    int i = 0;

    while (i <= text.length())
    {
        QChar ch = (i < text.length()) ? text.at(i) : QChar();
        QChar next = (i < text.length() - 1) ? text.at(i + 1) : QChar();
        QChar previous = (i > 0) ? text.at(i-1) : QChar();

        switch(state)
        {
        case TypeStart:
            start = i;
            if (ch.isSpace())
            {
                ++i;
            }
            else if (ch.isDigit() || (ch == '.' && previous.isDigit()))
            {
                ++i;
                state = TypeNumber;
            }
            else if (ch.isLetter() || ch == '_')
            {
                ++i;
                state = TypeIdentifier;
            }
            else if (ch == '\'')
            {
                ++i;
                state = TypeStringSingle;
            }
            else if (ch == '\"')
            {
                ++i;
                state = TypeStringDouble;
            }
            else if (ch == '/' && next == '*')//multiple line comment
            {
                ++i;
                ++i;
                state = TypeComment;
            }
            else if (ch == '-' && next == '-'/* && nextOfNext.isSpace()*/)// komenatar '--'
            {
                i = text.length();
                setFormat(start, text.length(), m_colors[Comment]);
            }
            else if (ch == '#') //komentar '#'
            {
                i = text.length();
                setFormat(start, text.length(), m_colors[Comment]);
            }

            else
            {
                if (ch =='{' || ch == '}') {
                    bracketPositions += i;
                    if (ch == '{')
                        bracketLevel++;
                    else
                        bracketLevel--;
                }
                ++i;
                state = TypeStart;
            }
            break;
        case TypeNumber:
            if (ch.isSpace() || !ch.isDigit())
            {
                setFormat(start, i - start, m_colors[Number]);
                state = TypeStart;
            }
            else
                ++i;
            break;
        case TypeIdentifier:
            if (ch.isSpace() || !(ch.isDigit() || ch.isLetter() || ch == '_'))
            {
                QString token = text.mid(start, i - start).trimmed();
                if (m_keywords.contains(token,Qt::CaseInsensitive))
                    setFormat(start, i - start, m_colors[Keyword]);
                else if (m_functions.contains(token,Qt::CaseInsensitive))
                    setFormat(start, i - start, m_colors[Function]);
                else
                    setFormat(start, i - start, m_colors[Normal]);
                state = TypeStart;
            }
            else
            {
                ++i;
            }
            break;
        case TypeStringDouble:
            if (ch == '\"')
            {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();

                if (prev != '\\')
                {
                    ++i;
                    setFormat(start, i - start, m_colors[StringDouble]);
                    state = TypeStart;
                }
                else
                    ++i;
            }
            else
                ++i;
            break;
        case TypeStringSingle:
            if (ch == '\'')
            {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();

                if (prev != '\\')
                {
                    ++i;
                    setFormat(start, i - start, m_colors[StringSingle]);
                    state = TypeStart;
                }
                else
                    ++i;
            }
            else
                ++i;
            break;

        case TypeComment:
            if (ch == '*' && next == '/')
            {
                ++i;
                ++i;
                setFormat(start, i - start, m_colors[Comment]);
                state = TypeStart;
            }
            else
                ++i;
            break;

        default:
            state = TypeStart;
            break;
        }
    }

    if (state == TypeComment)
        setFormat(start, text.length(), m_colors[Comment]);
    else if (state == TypeStringDouble)
        setFormat(start,text.length(), m_colors[StringDouble]);
    else if (state == TypeStringSingle)
        setFormat(start,text.length(), m_colors[StringSingle]);
    else
        state = TypeStart;

    if (!m_markString.isEmpty())
    {
        int pos = 0;
        int len = m_markString.length();
        QTextCharFormat markerFormat;
        markerFormat.setBackground(m_colors[Marker]);
        markerFormat.setForeground(m_colors[Normal]);
        for(;;)
        {
            pos = text.indexOf(m_markString, pos, m_markCaseSensitivity);
            if (pos < 0)
                break;
            setFormat(pos, len, markerFormat);
            ++pos;
        }
    }

    if (!bracketPositions.isEmpty())
    {
        JSBlockData *blockData = reinterpret_cast<JSBlockData*>(currentBlock().userData());
        if (!blockData)
        {
            blockData = new JSBlockData;
            currentBlock().setUserData(blockData);
        }
        blockData->bracketPositions = bracketPositions;
    }

    blockState = (state & 15) | (bracketLevel << 6);
    setCurrentBlockState(blockState);

}
