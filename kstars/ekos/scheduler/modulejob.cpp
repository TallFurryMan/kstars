#include "modulejob.h"

ModuleJob::ModuleJob(const QList<QVariant> &data, ModuleJob *parent)
    : m_parentItem(parent), m_itemData(data)
{

}

ModuleJob::~ModuleJob()
{
    qDeleteAll(m_childItems);
}

void ModuleJob::appendChild(ModuleJob *child)
{
    m_childItems.append(child);
}

int ModuleJob::row() const
{
    if (nullptr != m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModuleJob*>(this));

    return 0;
}

