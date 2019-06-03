#ifndef FOCUSJOB_H
#define FOCUSJOB_H

#include "modulejob.h"
#include <QString>
#include <QVariant>



class FocusJob : public ModuleJob
{
public:
    FocusJob(const QList<QVariant> &data, ModuleJob *parent);
};

#endif // FOCUSJOB_H
