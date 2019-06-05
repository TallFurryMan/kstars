#include "scheduler2.h"
#include "schedulermodel.h"

namespace Ekos
{

Scheduler2::Scheduler2()
{
    setupUi(this);

    QList<QVariant> list;
    list << "One";
    list << "Two";
    queueTable->setModel(new SchedulerModel(list));
}

}
