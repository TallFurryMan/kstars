#include "scheduler2.h"
#include "schedulermodel.h"

namespace Ekos
{

Scheduler2::Scheduler2()
{
    setupUi(this);

    queueTable->setModel(new SchedulerModel());
    queueTable->expandAll();
    queueTable->resizeColumnToContents(0);
    queueTable->resizeColumnToContents(1);
    queueTable->resizeColumnToContents(2);
}

}
