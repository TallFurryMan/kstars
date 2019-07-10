#include "modulejob.h"

ModuleJob::ModuleJob(QStandardItem *parent)
{
    if (nullptr != parent)
    {
        QList<QStandardItem*> l;
        l << this;
        for (unsigned int i = 1; i < CF_COUNT; i++)
            l << new QStandardItem();
        parent->appendRow(l);
    }
}

void ModuleJob::setDisplayName(const QString &name)
{
    setText(name);
}

void ModuleJob::setDisplayDescription(QString const &s)
{
    setCellField(CF_DESC, s);
}

void ModuleJob::setStartTime(const QDateTime &d)
{
    setCellField(CF_START, d.toString(QString("25°%1 MM/DD HH:mm").arg(QChar(0x2193))));
}

void ModuleJob::setAltitude(const double &a)
{
    setCellField(CF_ALT, QString("%1").arg(a, 1, 'f'));
}

void ModuleJob::setEndTime(const QDateTime &d)
{
    setCellField(CF_END, d.toString("MM/dd HH:mm"));
}

void ModuleJob::setDuration(const double &d)
{
    setCellField(CF_DUR, QTime::fromMSecsSinceStartOfDay(d*1000).toString("HH:mm:ss"));
}

void ModuleJob::setCellField(ModuleJob::CellFields cf, const QString &s)
{
    QStandardItem * const parent = this->parent();
    if (parent != nullptr)
    {
        int const index = static_cast<int> (cf);
        if (0 <= index && index < parent->columnCount())
            parent->child(this->row(), cf)->setText(s);
    }
}
