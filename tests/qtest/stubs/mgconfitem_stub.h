#ifndef MGCONFITEM_STUB_H
#define MGCONFITEM_STUB_H

#include <QVariant>
#include <QStringList>
#include <QObject>

class MGConfItem : public QObject
{
    Q_OBJECT

 public:

    explicit MGConfItem(const QString &key, QObject *parent = 0);

    virtual ~MGConfItem();

    QString key() const;

    QVariant value() const;

    QVariant value(const QVariant &def) const;

    void set(const QVariant &val);

    void unset();

    QList<QString> listDirs() const;

    QList<QString> listEntries() const;

 Q_SIGNALS:

    void valueChanged();

 private:
    friend struct MGConfItemPrivate;
    struct MGConfItemPrivate *priv;

    void update_value(bool emit_signal);
};

#endif // MGCONFITEM_STUB_H
