#include "frmpragmas.h"
#include "ui_frmpragmas.h"

FrmPragmas::FrmPragmas(QString connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FrmPragmas)
{
    ui->setupUi(this);
    this->connection = connection;
    ui->tblPragmaStatements->setColumnCount(2);
    ui->tblPragmaStatements->setHorizontalHeaderLabels(QStringList() << tr("Pragma Statement") << tr("Value"));

    loadPragmaStatements();
}

FrmPragmas::~FrmPragmas()
{
    delete ui;
}

void FrmPragmas::closeEvent(QCloseEvent *event)
{
    emit finished();

    QDialog::closeEvent(event);
}

void FrmPragmas::on_btnReload_clicked()
{
    loadPragmaStatements();
}

void FrmPragmas::on_btnClose_clicked()
{
    this->close();
}

void FrmPragmas::loadPragmaStatements()
{
    QStringList pragmaList;
    pragmaList << "pragma application_id;"
               << "pragma auto_vacuum;"
               << "pragma automatic_index;"
               << "pragma busy_timeout;"
               << "pragma cache_size;"
               << "pragma cache_spill;"
               << "pragma checkpoint_fullfsync;"
               << "pragma defer_foreign_keys;"
               << "pragma encoding;"
               << "pragma foreign_key_check;"
               << "pragma foreign_keys;"
               << "pragma freelist_count;"
               << "pragma fullfsync;"
               << "pragma integrity_check;"
               << "pragma journal_mode;"
               << "pragma journal_size_limit;"
               << "pragma legacy_file_format;"
               << "pragma locking_mode;"
               << "pragma max_page_count;"
               << "pragma mmap_size;"
               << "pragma page_count;"
               << "pragma page_size;"
               << "pragma query_only;"
               << "pragma quick_check;"
               << "pragma read_uncommitted;"
               << "pragma recursive_triggers;"
               << "pragma reverse_unordered_selects;"
               << "pragma schema_version;"
               << "pragma secure_delete;"
               << "pragma soft_heap_limit;"
               << "pragma synchronous;"
               << "pragma temp_store;"
               << "pragma user_version;"
               << "pragma wal_autocheckpoint;"
               << "pragma writable_schema;"
               << "pragma count_changes;"///depricated
               << "pragma default_cache_size;"///depricated
               << "pragma empty_result_callbacks;"///depricated
               << "pragma full_column_names;"///depricated
               << "pragma short_column_names;";///depricated

    QStringList depricated = QStringList() << "count_changes" << "default_cache_size" << "empty_result_callbacks"<< "full_column_names" << "short_column_names";

    QSqlDatabase db = QSqlDatabase::database(connection);

    if (db.isOpen())
    {
        for (int i = ui->tblPragmaStatements->rowCount()-1; i >= 0; i--)
            ui->tblPragmaStatements->removeRow(i);

        QSqlQuery upit(db);

        foreach (QString str, pragmaList)
        {
            upit.exec(str);

            while (upit.next())
            {
                ui->tblPragmaStatements->insertRow(ui->tblPragmaStatements->rowCount());

                str.remove("pragma ").remove(";");

                if (depricated.contains(str))
                    str.append("  (depricated)");

                ui->tblPragmaStatements->setItem(ui->tblPragmaStatements->rowCount() - 1, 0, new QTableWidgetItem(str));
                ui->tblPragmaStatements->setItem(ui->tblPragmaStatements->rowCount() - 1, 1, new QTableWidgetItem(upit.value(0).toString()));

                for (int i = 0; i < ui->tblPragmaStatements->columnCount(); i++)
                    ui->tblPragmaStatements->item(ui->tblPragmaStatements->rowCount()-1, i)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
        }
    }

    ui->tblPragmaStatements->resizeColumnsToContents();
}
