#ifndef SCHEDULER2_H
#define SCHEDULER2_H

#include "ui_scheduler2.h"

namespace Ekos
{
class Scheduler2 : public QWidget, public Ui::Scheduler2
{
public:
    Scheduler2();
    ~Scheduler2() = default;
};
}

#endif // SCHEDULER2_H
