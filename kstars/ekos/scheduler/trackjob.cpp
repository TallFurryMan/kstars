#include "trackjob.h"

TrackJob::TrackJob(Scheduler2Job *parent, SkyPoint const &target)
    : ModuleJob(parent), m_target(target)
{
    setIcon(QIcon::fromTheme("transform-move"));
    setDisplayName("Track");
    setDisplayDescription(getDisplayDescription());
    setDuration(60);

    setChild(0,0,new QStandardItem("RA"));
    setChild(0,1,new QStandardItem(m_target.ra().toHMSString()));
    setChild(1,0,new QStandardItem("DEC"));
    setChild(1,1,new QStandardItem(m_target.dec().toDMSString()));
    setChild(2,0,new QStandardItem("Rate"));
    setChild(2,1,new QStandardItem("Sidereal"));
}

QString TrackJob::getDisplayDescription() const
{
    return QString("RA %1 DEC %2").arg(m_target.ra().toHMSString()).arg(m_target.dec().toDMSString());
}


