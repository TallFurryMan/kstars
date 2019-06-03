#ifndef MODULEJOB_H
#define MODULEJOB_H

#include <QString>
#include <QVariant>



class ModuleJob
{
public:
    explicit ModuleJob(const QList<QVariant> &data, ModuleJob * parent = 0);
    ~ModuleJob();
public:
    void appendChild(ModuleJob *);
    int row() const;
public:
    int childCount() const { return m_childItems.count(); }
    int columnCount() const { return m_itemData.count(); }
    QVariant data(int column) const { return m_itemData.value(column); }
    ModuleJob * parentItem() { return m_parentItem; }
    ModuleJob * child(int row) { return m_childItems.value(row); }
public:
    virtual QString getModuleName() const { return QString("-?-"); }
protected:
    QList<ModuleJob*> m_childItems;
    QList<QVariant> m_itemData;
    ModuleJob *m_parentItem;
};

#endif // MODULEJOB_H
