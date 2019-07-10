#ifndef FOCUSJOB_H
#define FOCUSJOB_H

#include "modulejob.h"
#include <QString>
#include <QVariant>



class FocusJob : public ModuleJob
{
public:
    FocusJob(ModuleJob *parent);
public:
    virtual QString getDisplayDescription() const override;
};

#endif // FOCUSJOB_H
