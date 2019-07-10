#ifndef OBSERVATORYJOB_H
#define OBSERVATORYJOB_H

#include "modulejob.h"
#include <QString>
#include <QVariant>



class ObservatoryJob : public ModuleJob
{
public:
    ObservatoryJob(ModuleJob *parent);
public:
    virtual QString getDisplayDescription() const override;
};

#endif // OBSERVATORYJOB_H
