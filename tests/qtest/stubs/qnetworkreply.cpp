#include "qnetworkreply.h"

QT_BEGIN_NAMESPACE

QNetworkReply::QNetworkReply(QObject *parent)
{
    Q_UNUSED(parent);
    testError = NoError;
}

QNetworkReply::~QNetworkReply()
{
}

void QNetworkReply::emitUploadProgress (qint64 prog, qint64 total)
{
    emit uploadProgress (prog, total);
}

void QNetworkReply::emitFinished ()
{
    emit finished ();
}

void QNetworkReply::abort ()
{
    return;
}

const QString QNetworkReply::errorString()
{
    return "test error";
}

QNetworkReply::NetworkError QNetworkReply::error() const
{
    return testError;
}

void QNetworkReply::emitSslErrors (QList<QSslError> list_)
{
    emit sslErrors (list_);
}

void QNetworkReply::emitError (QNetworkReply::NetworkError error_)
{
    testError = error_;
    emit error (error_);
}



QT_END_NAMESPACE
