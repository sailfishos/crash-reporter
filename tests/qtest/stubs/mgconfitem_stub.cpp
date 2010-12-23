#include <MGConfItem>

// MGConfItem stubs
struct MGConfItemPrivate
{
    QString key;
    QVariant value;
};

QMap<QString, MGConfItem*> gMGConfItems;

MGConfItem::MGConfItem(const QString &key, QObject *parent)
    : QObject (parent)
{
    priv = new MGConfItemPrivate;
    priv->key = key;
    priv->value = QVariant(false);

    gMGConfItems.insert(key, this);
}

MGConfItem::~MGConfItem()
{
    gMGConfItems.remove(priv->key);
    delete priv;
}

QList<QString> MGConfItem::listEntries() const
{
    return QList<QString>();
}

QVariant MGConfItem::value() const
{
    return priv->value;
}

void MGConfItem::set(const QVariant &val)
{
    priv->value = val;
    emit valueChanged();
}

void MGConfItem::unset()
{
    priv->value = QVariant();
}
