#ifndef MODULEJOB_H
#define MODULEJOB_H

#include <QDateTime>
#include <QStandardItem>
#include <QString>
#include <QVariant>



class ModuleJob: public QStandardItem
{
public:
    ModuleJob(QStandardItem *parent = nullptr);
    virtual ~ModuleJob() {}
public:
    enum CellFields {
        CF_NAME = 0,
        CF_DESC = 1,
        CF_START = 2,
        CF_ALT = 3,
        CF_END = 4,
        CF_DUR = 5,
        CF_COUNT = 6
    };
public:
    virtual QString getDisplayDescription() const { return QString("-"); }
protected:
    void setDisplayName(QString const &name);
    void setDisplayDescription(QString const &);
    void setStartTime(QDateTime const &);
    void setAltitude(double const &);
    void setEndTime(QDateTime const &);
    void setDuration(double const &);
protected:
    void setCellField(enum CellFields, QString const &);
};

#endif // MODULEJOB_H
