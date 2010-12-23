#include "qnetworkaccessmanager.h"
#include "qnetworkreply.h"
#include <QAuthenticator>

QNetworkReply *QNetworkAccessManager::post(const QNetworkRequest &request, 
					   const QByteArray &data)
{
    Q_UNUSED(request);
    Q_UNUSED(data);

    return new QNetworkReply(this);
}


QNetworkReply *QNetworkAccessManager::put(const QNetworkRequest &request, 
					  const QByteArray &data)
{
    Q_UNUSED(request);
    Q_UNUSED(data);

    return new QNetworkReply(this);
}

void QNetworkAccessManager::emitAuthenticationRequired(QNetworkReply *reply)
{
    emit authenticationRequired(reply, new QAuthenticator());
}

