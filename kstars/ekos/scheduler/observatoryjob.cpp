#include "observatoryjob.h"
#include "modulejob.h"

ObservatoryJob::ObservatoryJob(ModuleJob *parent)
    : ModuleJob(parent)
{
    setIcon(QIcon::fromTheme("edit-find"));
    setDisplayName("Observatory");
    setDisplayDescription(getDisplayDescription());
}

QString ObservatoryJob::getDisplayDescription() const
{
    return QString("CCD 5°C");
}
