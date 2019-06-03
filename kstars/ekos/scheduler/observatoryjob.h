#ifndef OBSERVATORYJOB_H
#define OBSERVATORYJOB_H

#include "modulejob.h"
#include <QString>
#include <QVariant>



class ObservatoryJob : public ModuleJob
{
public:
    ObservatoryJob(const QList<QVariant> &data, ModuleJob *parent);
public:
    virtual QString getModuleName() const;
};

#endif // OBSERVATORYJOB_H
