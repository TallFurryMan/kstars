#ifndef ALIGNJOB_H
#define ALIGNJOB_H

#include "modulejob.h"



class AlignJob : public ModuleJob
{
public:
    AlignJob(const QList<QVariant> &, ModuleJob *);
};

#endif // ALIGNJOB_H
