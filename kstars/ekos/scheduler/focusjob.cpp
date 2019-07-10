#include "focusjob.h"

FocusJob::FocusJob(ModuleJob *parent)
    : ModuleJob(parent)
{
    setIcon(QIcon::fromTheme("edit-find"));
    setDisplayName("Focus");
    setDisplayDescription(getDisplayDescription());
    setStartTime(QDateTime(QDate(2019,5,13),QTime(23,6,25)));
    setDuration(120);

    setChild(0,0,new QStandardItem("Exposure"));
    setChild(0,1,new QStandardItem("6\""));
    setChild(1,0,new QStandardItem("Span"));
    setChild(1,1,new QStandardItem("Full-frame"));
    setChild(2,0,new QStandardItem("Algorithm"));
    setChild(2,1,new QStandardItem("Gradient"));
    setChild(2,0,new QStandardItem("Settle"));
    setChild(2,1,new QStandardItem("1000"));
}

QString FocusJob::getDisplayDescription() const
{
    return QString("Full-frame, exposure 6\", iterative");
}
