#ifndef SCHEDULER2JOB_H
#define SCHEDULER2JOB_H

#include "modulejob.h"
#include <QAbstractItemModel>
#include <QString>
#include <QVariant>

class SchedulerModel: public QAbstractItemModel
{
public:
    SchedulerModel(const QList<QVariant> &data, ModuleJob *parent);
    ~SchedulerModel();
public:
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const QStringList &lines, ModuleJob *parent);
private:
    ModuleJob *rootItem;
};

#endif // SCHEDULER2JOB_H
