#include "frmmainwindow.h"
#include <QApplication>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString text;
    switch (type) {
    case QtDebugMsg:
        text = QString("Debug %5 %1 %2 %3 %4").arg(msg).arg(context.file).arg(context.line).arg(context.function).arg(QDateTime::currentDateTime().toString());
        break;
    case QtWarningMsg:
        text = QString("Warning %5 %1 %2 %3 %4").arg(msg).arg(context.file).arg(context.line).arg(context.function).arg(QDateTime::currentDateTime().toString());
        break;
    case QtCriticalMsg:
        text = QString("Critical %5 %1 %2 %3 %4").arg(msg).arg(context.file).arg(context.line).arg(context.function).arg(QDateTime::currentDateTime().toString());
        break;
    case QtFatalMsg:
        text = QString("Fatal %5 %1 %2 %3 %4").arg(msg).arg(context.file).arg(context.line).arg(context.function).arg(QDateTime::currentDateTime().toString());
        abort();
    }

    QFile file("log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream out(&file);
    out << text << endl;
    file.flush();
    file.close();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //qInstallMessageHandler(myMessageOutput);

    QApplication::setApplicationDisplayName("KoSQLite");
    QApplication::setApplicationName("KoSQLite");
    QApplication::setApplicationVersion("0.1.0");
    QApplication::setOrganizationDomain("karoljkocmaros.blogspot.com");
    QApplication::setOrganizationName("Karolj Kocmaros");

    QTranslator qTranslator;
    QString locale = QLocale::system().name();

    QSettings settingsConfig(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName());
    settingsConfig.beginGroup("language");
    QString loSettings = settingsConfig.value("language").toString();
    settingsConfig.endGroup();

    if (loSettings != "")
        locale = loSettings;

#ifdef Q_OS_LINUX //na linux su na ovoj specifičnoj putanji instalirani prevodi, ako se ne instalira na to mesto, onda nema prevoda
    if ( qTranslator.load(QString("/usr/share/kosqlite/i18n/%1.qm").arg(locale)) )
        a.installTranslator(&qTranslator);
#endif
#ifdef Q_OS_WIN //na windows su prevodi instalirani na C:/Program Files/KoSQLite/i18n
    if ( qTranslator.load(QString("i18n/%1.qm").arg(locale)) )
        a.installTranslator(&qTranslator);
#endif

    FrmMainWindow w;
    w.show();    

    return a.exec();
}
