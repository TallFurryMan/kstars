#include "scheduler2job.h"
#include "modulejob.h"

Scheduler2Job::Scheduler2Job(const QList<QVariant> &data, ModuleJob *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Title" << "Summary";
    rootItem = new ModuleJob(rootData);
    setupModelData(data.split(QString("\n")), rootItem);
}

QVariant Scheduler2Job::data(const QModelIndex &index, int role) const
{
    delete rootItem;
}

QModelIndex Scheduler2Job::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ModuleJob *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ModuleJob*>(parent.internalPointer());

    ModuleJob *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex SchedulerModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ModuleJob *childItem = static_cast<ModuleJob*>(index.internalPointer());
    ModuleJob *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SchedulerModel::rowCount(const QModelIndex &parent) const
{
    ModuleJob *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int SchedulerModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant SchedulerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
         return QVariant();

     if (role != Qt::DisplayRole)
         return QVariant();

     TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

     return item->data(index.column());
}

Qt::ItemFlags SchedulerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
         return 0;

     return QAbstractItemModel::flags(index);
}


QVariant SchedulerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
