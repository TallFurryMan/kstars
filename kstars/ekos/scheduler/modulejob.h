#ifndef MODULEJOB_H
#define MODULEJOB_H

#include <QString>



class ModuleJob
{
public:
    ModuleJob();
public:
    virtual QString getModuleName() const;
};

#endif // MODULEJOB_H
