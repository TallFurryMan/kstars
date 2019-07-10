#include "alignjob.h"

AlignJob::AlignJob(ModuleJob *parent)
    : ModuleJob(parent)
{
    setIcon(QIcon::fromTheme("server-database"));
    setDisplayName("Align");
    setDisplayDescription(getDisplayDescription());
    setDuration(30);
}

QString AlignJob::getDisplayDescription() const
{
    return QString("Offline solver, exposure 8\", filter LPR");
}
