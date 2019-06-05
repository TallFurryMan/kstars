#ifndef SCHEDULER2JOB_H
#define SCHEDULER2JOB_H

#include "modulejob.h"

class Scheduler2Job: public ModuleJob
{
public:
    Scheduler2Job(const QList<QVariant> &data, ModuleJob *parent);
};

#endif // SCHEDULER2JOB_H
