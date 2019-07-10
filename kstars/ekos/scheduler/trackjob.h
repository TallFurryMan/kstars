#ifndef TRACKJOB_H
#define TRACKJOB_H

#include "modulejob.h"
#include "scheduler2job.h"
#include <skypoint.h>

class TrackJob: public ModuleJob
{
public:
    TrackJob(Scheduler2Job *parent, const SkyPoint &target);
public:
    virtual QString getDisplayDescription() const;
protected:
    SkyPoint m_target;
};

#endif // TRACKJOB_H
