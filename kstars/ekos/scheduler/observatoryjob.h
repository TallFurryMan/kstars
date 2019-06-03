#ifndef OBSERVATORYJOB_H
#define OBSERVATORYJOB_H

#include "modulejob.h"



class ObservatoryJob : public ModuleJob
{
public:
    ObservatoryJob();
public:
    virtual QString getModuleName() const;
};

#endif // OBSERVATORYJOB_H
