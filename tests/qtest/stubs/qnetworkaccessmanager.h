#ifndef QNETWORKACCESSMANAGER_H
#define QNETWORKACCESSMANAGER_H

#include <QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)

class QIODevice;
class QAbstractNetworkCache;
class QAuthenticator;
class QByteArray;
template<typename T> class QList;
class QNetworkRequest;
class QNetworkReply;
class QNetworkProxy;
class QNetworkProxyFactory;
class QSslError;

class QNetworkReplyImplPrivate;
class QNetworkAccessManagerPrivate;
// Stub for QNetworkAccessManager
class Q_NETWORK_EXPORT QNetworkAccessManager: public QObject
{
    Q_OBJECT
public:
    explicit QNetworkAccessManager(QObject *parent = 0);
    ~QNetworkAccessManager();

    void setProxy(const QNetworkProxy &proxy);
    QNetworkReply *post(const QNetworkRequest &request, const QByteArray &data);
    QNetworkReply *put(const QNetworkRequest &request, const QByteArray &data);

    void emitAuthenticationRequired(QNetworkReply *reply);

    void test();
Q_SIGNALS:
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void finished(QNetworkReply *reply);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

};

QT_END_NAMESPACE

QT_END_HEADER

#endif
