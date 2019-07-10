#include "scheduler2job.h"
#include "modulejob.h"
#include "trackjob.h"
#include "focusjob.h"
#include "alignjob.h"
#include "capturejob.h"

Scheduler2Job::Scheduler2Job(QStandardItem *parent, QString const &targetName)
    : ModuleJob(parent), m_targetName(targetName)
{
    setIcon(QIcon::fromTheme("weather-clear-night"));
    setDisplayName(targetName);
    setDisplayDescription(getDisplayDescription());
    setStartTime(QDateTime(QDate(2019,5,13),QTime(23,4,25)));

    new TrackJob(this, SkyPoint(2.53,56.24));
    new FocusJob(this);
    new AlignJob(this);
    new CaptureJob(this);
}

QString Scheduler2Job::getDisplayDescription() const
{
    //return m_targetDescription;
    return QString("13/05 23:04 to 14/05 02:25 - 03:21");
}
