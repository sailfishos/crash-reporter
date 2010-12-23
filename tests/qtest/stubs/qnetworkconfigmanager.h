#ifndef QNETWORKCONFIGMANAGER_H
#define QNETWORKCONFIGMANAGER_H

#include <QObject>

#include "qnetworkconfiguration.h"

// Stub for QNetworkConfigurationManager
class QNetworkConfigurationManager : public QObject
{
	Q_OBJECT
public:
	QNetworkConfigurationManager() {}
	~QNetworkConfigurationManager() {}

	QNetworkConfiguration defaultConfiguration() const { 
		return m_defaultConfiguration;
	}

	void updateConfigurations() {}

private:
	QNetworkConfiguration m_defaultConfiguration;
};

#endif	// QNETWORKCONFIGMANAGER_H
