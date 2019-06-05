#include "schedulermodel.h"
#include "scheduler2job.h"

#include <QPushButton>

SchedulerModel::SchedulerModel(const QList<QVariant> &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Job Title" << "Actions" << "Summary";
    rootItem = new ModuleJob(rootData, nullptr);

    QList<QVariant> childData;
    childData << "Observation: M101" << "<Toolbar>" << "M101 RA:2h23'34 DEC:46°34'23";
    rootItem->appendChild(new Scheduler2Job(childData, rootItem));
}

SchedulerModel::~SchedulerModel()
{
    delete rootItem;
}

QModelIndex SchedulerModel::index(int row, int column, const QModelIndex &parent) const
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
        parentItem = static_cast<ModuleJob*>(parent.internalPointer());

    return parentItem->childCount();
}

int SchedulerModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<ModuleJob*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant SchedulerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
         return QVariant();

     if (role != Qt::DisplayRole)
         return QVariant();

     ModuleJob *item = static_cast<ModuleJob*>(index.internalPointer());

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
