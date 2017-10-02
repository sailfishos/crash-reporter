#include "qnetworkconfiguration.h"

QNetworkConfiguration::BearerType QNetworkConfiguration::bearerType() const
{
    return BearerWLAN;
}

QNetworkConfiguration::StateFlags QNetworkConfiguration::state() const
{
    return Active;
}
