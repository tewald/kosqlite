#ifndef SQLFILE_H
#define SQLFILE_H

#include <QString>
#include <QFileInfo>

class SQLFile
{
public:
    SQLFile();
    SQLFile(QString file);
    SQLFile(const SQLFile &of);

    void setFile(QString file);
    void setFileName(QString fileName);
    void setFilePath(QString filePath);

    QString getFile() const;
    QString getFileName() const;
    QString getFilePath() const;

    SQLFile &operator=(const SQLFile &of);
    bool operator <(const SQLFile &of);

private:
    QString file;
    QString fileName;
    QString filePath;


};

#endif // SQLFILE_H
