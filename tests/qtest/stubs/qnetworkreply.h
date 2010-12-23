#ifndef QNETWORKREPLY_H
#define QNETWORKREPLY_H

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Network)


class QNetworkReplyPrivate;
// Stub for QNetworkReply
class Q_NETWORK_EXPORT QNetworkReply: public QObject
{
    Q_OBJECT
    Q_ENUMS(NetworkError)

public:
    enum NetworkError {
        NoError = 0,

        // network layer errors [relating to the destination server] (1-99):
        ConnectionRefusedError = 1,
        RemoteHostClosedError,
        HostNotFoundError,
        TimeoutError,
        OperationCanceledError,
        SslHandshakeFailedError,
        UnknownNetworkError = 99,

        // proxy errors (101-199):
        ProxyConnectionRefusedError = 101,
        ProxyConnectionClosedError,
        ProxyNotFoundError,
        ProxyTimeoutError,
        ProxyAuthenticationRequiredError,
        UnknownProxyError = 199,

        // content errors (201-299):
        ContentAccessDenied = 201,
        ContentOperationNotPermittedError,
        ContentNotFoundError,
        AuthenticationRequiredError,
        UnknownContentError = 299,

        // protocol errors
        ProtocolUnknownError = 301,
        ProtocolInvalidOperationError,
        ProtocolFailure = 399
    };
    QNetworkReply(QObject *parent);
    ~QNetworkReply();
    void emitUploadProgress (qint64 bytesSent, qint64 bytesTotal);
    void emitFinished ();
    void emitSslErrors (QList<QSslError>);
    void emitError (QNetworkReply::NetworkError);
    void abort();
    const QString errorString();
    NetworkError error() const;

public Q_SLOTS:
    void ignoreSslErrors();

Q_SIGNALS:
    void metaDataChanged();
    void finished();
    void error(QNetworkReply::NetworkError);
    void sslErrors(const QList<QSslError> &errors);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

protected:

private:
    NetworkError testError;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
