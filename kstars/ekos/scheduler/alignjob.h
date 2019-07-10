#ifndef ALIGNJOB_H
#define ALIGNJOB_H

#include "modulejob.h"



class AlignJob : public ModuleJob
{
public:
    AlignJob(ModuleJob *parent);
public:
    virtual QString getDisplayDescription() const override;
};

#endif // ALIGNJOB_H
