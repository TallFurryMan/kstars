#ifndef TRACKJOB_H
#define TRACKJOB_H

#include "modulejob.h"
#include <QString>
#include <QVariant>



class TrackJob : public ModuleJob
{
public:
    TrackJob(const QList<QVariant> &data, ModuleJob *parent);
};

#endif // TRACKJOB_H
