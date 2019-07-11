#ifndef _CWIFISERVER_H_
#define _CWIFISERVER_H_

#include <vector> // vector

#include "csocketserver.h"
#include "cinfowifi.h"

class CWifiServer : public CSocketServer
{
		TIndex MaxClientDeconnected;

		std::vector<CInfoWifi> InfoWifis;
		std::vector<CInfoWifi> InfoWifisDeconnected;

		bool RecoverCoordinateOfInfoWifiDeconnected(TCID cid, CCoordinate& coo);

		bool RecoverCoordinateOfInfoWifi(TCID cid, CCoordinate& coo);

	public :

		bool Listen(TIndex maxClientDeconnected);

		TDescriptor Accept();

		bool IsEnable(TIndex index);

		void ShowInfoWifi(TIndex index);

		void CloseClient(TIndex index);

		void CloseAllClient();

		void SendAllOtherClients(TIndex index,const char* data, ssize_t sizeOfData);

		CInfoWifi* GetReferenceOnInfoWifiByCID(TCID cid);

		CInfoWifi* GetReferenceOnInfoWifiDeconnectedByCID(TCID cid);

		CInfoWifi* GetReferenceOnInfoWifiByIndex(TIndex index);

};

#endif
