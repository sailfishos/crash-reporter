#include <DuiControlPanelIf>

bool gDcpAppletPageCalled;
QString gDcpRequestedAppletPage;
bool gDcpSuccess;

DuiControlPanelIf::DuiControlPanelIf(const QString &preferredService, QObject *parent) :
    MServiceFwBaseIf(preferredService, parent)
{

}

bool DuiControlPanelIf::appletPage(const QString &appletPage)
{
    gDcpAppletPageCalled = true;
    gDcpRequestedAppletPage = appletPage;
    return gDcpSuccess;
}
