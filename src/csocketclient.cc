#include <iostream> // cout
#include <cstdio> //perror

#include <arpa/inet.h> // INADDR_ANY
#include <unistd.h> // close

#include "csocketclient.h"

using namespace std;

// ----------------- CSocketClient

CSocketClient::~CSocketClient() 
{
	CSocket::Close();
}

CSocketClient::CSocketClient() : CSocket()
{
	Init();
}

CSocketClient::CSocketClient(TSocket type) : CSocket(type)
{
	Init();
}

void CSocketClient::Init()
{
	IsConnected=false;
	StopTheReconnect=false ;
}

bool CSocketClient::ConnectLoop(struct sockaddr* server, size_t size_of_server)
{
	while ( ! StopTheReconnect )
	{
		//create a master socket
		if( ! Configure() )
		{
			cerr<<"Error : CSocketClient::Connect : Configure"<<endl;
			return false;
		}

		if( ! connect(Master,server,size_of_server) )
		{
			IsConnected=true;

			return true;
		}

		perror("CSocketClient::Connect : connect");
		Close();
		sleep(2);
	}

	// impossible to be here
	return false;
}

ssize_t CSocketClient::Send(const char* data, ssize_t sizeOfData)
{
	if ( IsConnected ){

		int ret = CSocket::Send(Master, data, sizeOfData);

		if( ret > 0 )
			return ret ;
		if(ret < 0 && errno == EWOULDBLOCK )
			return SOCKET_ERROR ;

		// if recv returns<0 and errno!=EWOULDBLOCK -->  then it is something that can be considered as connection-losing
		return SOCKET_DISCONNECT ;

	}
	return SOCKET_ERROR;
}

ssize_t CSocketClient::SendBigData(const char* data, ssize_t sizeOfData)
{
	if ( IsConnected ){

		ssize_t ret = CSocketClient::Send((char*)&sizeOfData, (unsigned)sizeof(sizeOfData));
		switch ( ret  )
		{
			case SOCKET_ERROR : return SOCKET_ERROR;
			case SOCKET_DISCONNECT : return SOCKET_DISCONNECT;
		}

		ret = CSocket::Send(Master, data, sizeOfData);

		if( ret > 0 )
			return ret ;
		if(ret < 0 && errno == EWOULDBLOCK )
			return SOCKET_ERROR ;

		// if recv returns<0 and errno!=EWOULDBLOCK -->  then it is something that can be considered as connection-losing
		return SOCKET_DISCONNECT ;

	}
	return SOCKET_ERROR;
}

ssize_t CSocketClient::Read(char* data, ssize_t sizeOfData)
{
	if ( IsConnected ){
		int ret = CSocket::Read(Master, data, sizeOfData);
		if( ret > 0 )
			return ret ;
		if(ret == 0 )
			return SOCKET_DISCONNECT ;

		if ( ret < 0 && errno == EWOULDBLOCK )
			return SOCKET_ERROR ;

		// if recv returns<0 and errno!=EWOULDBLOCK -->  then it is something that can be considered as EOF
		return SOCKET_DISCONNECT ;
	}
	return SOCKET_ERROR;
}

ssize_t CSocketClient::ReadBigData(char* data, ssize_t sizeOfData)
{
	if ( IsConnected ){
		ssize_t size;

		ssize_t ret_size = CSocketClient::Read((char*)&size, (unsigned)sizeof(size));
		switch ( ret_size  )
		{
			case SOCKET_ERROR : return SOCKET_ERROR;
			case SOCKET_DISCONNECT : return SOCKET_DISCONNECT;
		}

		if ( size > sizeOfData )
		{
			size=sizeOfData;
			cerr<<"Error : CSocketClient::ReadBigData : "<<size<<" > "<<sizeOfData<<endl;
		}

		int ret_data = CSocket::Read(Master, data, size);
		if( ret_data > 0 )
			return ret_data ;
		if(ret_data == 0 )
			return SOCKET_DISCONNECT ;

		if ( ret_data < 0 && errno == EWOULDBLOCK )
			return SOCKET_ERROR ;

		// if recv returns<0 and errno!=EWOULDBLOCK -->  then it is something that can be considered as EOF
		return SOCKET_DISCONNECT ;
	}
	return SOCKET_ERROR;
}

bool CSocketClient::SetBlocking(bool blocking)
{
	return CSocket::SetBlocking(Master,blocking);
}

void CSocketClient::StopReconnect(bool status){

	StopTheReconnect = true ;
}

// ----------------- CSocketClientINET

CSocketClientINET::CSocketClientINET() : CSocketClient(AF_INET) {};

void CSocketClientINET::Init(const char* IP, TPort port)
{
	Server.sin_family = AF_INET;
	Server.sin_addr.s_addr = inet_addr(IP);
	Server.sin_port = htons(port);
}

bool CSocketClientINET::Connect()
{
	return ConnectLoop((struct sockaddr*) &Server, sizeof(Server));
}

// ----------------- CSocketClientVHOST

CSocketClientVHOST::CSocketClientVHOST() : CSocketClient(AF_VSOCK) {};

void CSocketClientVHOST::Init(TPort port)
{
	Server.svm_family = AF_VSOCK;
	Server.svm_reserved1 = 0;
	Server.svm_port = port;
	Server.svm_cid = 2;
}

bool CSocketClientVHOST::Connect()
{
	return ConnectLoop((struct sockaddr*) &Server, sizeof(Server));
}
