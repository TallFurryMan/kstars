#ifndef CAPTUREJOB_H
#define CAPTUREJOB_H

#include "modulejob.h"
#include <QString>
#include <QVariant>



class CaptureJob : public ModuleJob
{
public:
    CaptureJob(ModuleJob *parent);
public:
    virtual QString getDisplayDescription() const override;
};

#endif // CAPTUREJOB_H
