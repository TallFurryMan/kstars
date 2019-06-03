#ifndef CAPTUREJOB_H
#define CAPTUREJOB_H

#include "modulejob.h"
#include <QString>
#include <QVariant>



class CaptureJob : public ModuleJob
{
public:
    CaptureJob(const QList<QVariant> &data, ModuleJob *parent);
};

#endif // CAPTUREJOB_H
