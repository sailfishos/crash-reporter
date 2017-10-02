#include "qnetworksession.h"

QNetworkSession::QNetworkSession(const QNetworkConfiguration &connectionConfig, QObject *parent) :
    m_configuration(connectionConfig),
    m_state(QNetworkSession::Disconnected)
{
    Q_UNUSED(parent);
}

QNetworkSession::~QNetworkSession()
{
}

QString QNetworkSession::errorString () const
{
    return "QNetworkSession dummy error string";
}

bool QNetworkSession::isOpen() const
{
    return m_state == QNetworkSession::Connected;
}

QNetworkSession::State QNetworkSession::state() const
{
    return m_state;
}

void QNetworkSession::close()
{
    m_state = QNetworkSession::Disconnected;
    emit stateChanged(QNetworkSession::Disconnected);
}

void QNetworkSession::stop()
{
    m_state = QNetworkSession::Disconnected;
    emit stateChanged(QNetworkSession::Disconnected);
}

void QNetworkSession::open()
{
    m_state = Connected;
    emit sessionOpened();
    emit stateChanged(QNetworkSession::Connected);
}
