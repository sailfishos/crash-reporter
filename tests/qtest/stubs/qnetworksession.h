#ifndef QNETWORKSESSION_H
#define QNETWORKSESSION_H

#include <QObject>
#include <QString>

#include "qnetworkconfiguration.h"
#include "qnetworkconfigmanager.h"

// Stub for QNetworkSession
class QNetworkSession : public QObject
{
    Q_OBJECT
public:
    QNetworkSession(const QNetworkConfiguration &connectionConfig, QObject *parent = 0);
    ~QNetworkSession();

    enum SessionError {
        UnknownSessionError = 0,
        SessionAbortedError,
        RoamingError,
        OperationNotSupportedError,
        InvalidConfigurationError
    };

    enum State {
        Invalid = 0,
        NotAvailable,
        Connecting,
        Connected,
        Closing,
        Disconnected,
        Roaming
    };

    QString errorString () const;
    bool isOpen() const;
    State state() const;

public slots:
    void close();
    void open();
    void stop();

signals:
    void stateChanged(QNetworkSession::State state);
    void error(QNetworkSession::SessionError error);
    void sessionOpened();

private:
    QNetworkConfiguration m_configuration;
    State m_state;
};

#endif // QNETWORKSESSION_H
