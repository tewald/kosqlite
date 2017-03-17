#include "sqlfile.h"
#include <QDebug>

SQLFile::SQLFile()
{
    this->file = "";
    this->fileName = "";
    this->filePath = "";
}

SQLFile::SQLFile(QString file)
{
    this->file = file;
    this->fileName = QFileInfo(file).fileName();
    this->filePath = QFileInfo(file).path();
}

SQLFile::SQLFile(const SQLFile &of)
{
    this->file = of.file;
    this->fileName = of.fileName;
    this->filePath = of.filePath;
}

void SQLFile::setFile(QString file)
{
    this->file = file;
    this->fileName = QFileInfo(file).fileName();
    this->filePath = QFileInfo(file).path();
}

void SQLFile::setFileName(QString fileName)
{
    this->fileName = QFileInfo(fileName).fileName();
}

void SQLFile::setFilePath(QString filePath)
{
    this->filePath = QFileInfo(filePath).path();
}

QString SQLFile::getFile() const
{
    return this->file;
}

QString SQLFile::getFileName() const
{
    return this->fileName;
}

QString SQLFile::getFilePath() const
{
    return this->filePath;
}

SQLFile &SQLFile::operator=(const SQLFile &of)
{
    file = of.file;
    fileName = of.fileName;
    filePath = of.filePath;

    return *this;
}

bool SQLFile::operator <(const SQLFile &of)
{
    if (file != of.file)
        return file < of.file;
    else if (fileName != of.fileName)
        return fileName < of.fileName;

    return filePath < of.filePath;

}
