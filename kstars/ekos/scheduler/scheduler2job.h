#ifndef SCHEDULER2JOB_H
#define SCHEDULER2JOB_H

#include "modulejob.h"

class Scheduler2Job: public ModuleJob
{
public:
    Scheduler2Job(QStandardItem *parent, const QString &targetName);
public:
    virtual QString getDisplayDescription() const override;
protected:
    QString m_targetName {"NoTargetName"};
    QString m_targetDescription {"NoTargetDescription"};
};

#endif // SCHEDULER2JOB_H
