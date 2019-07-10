#include "capturejob.h"

CaptureJob::CaptureJob(ModuleJob *parent)
    : ModuleJob(parent)
{
    setIcon(QIcon::fromTheme("camera-photo"));
    setDisplayName("Capture");
    setDisplayDescription(getDisplayDescription());
    setStartTime(QDateTime(QDate(2019,5,13),QTime(23,4,25).addSecs(30+120+60)));
    setDuration(5*300+5*150+5*150+5*150);
    setEndTime(QDateTime(QDate(2019,5,13),QTime(23,4,25).addSecs(30+120+60).addSecs(5*300+5*150+5*150+5*150)));
}

QString CaptureJob::getDisplayDescription() const
{
    return QString("L:5x300s R:5x150s G:5x150s B:5x150s");
}
