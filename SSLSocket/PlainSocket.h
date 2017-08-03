/////////////////////////////////////////////////////////////////////////////
// 
// PlainSocket
//
// Original idea:
// David Maw: https://www.codeproject.com/Articles/1000189/A-Working-TCP-Client-and-Server-With-SSL
// License:   https://www.codeproject.com/info/cpol10.aspx
//
#pragma once
#include <atltime.h>
#include "SocketStream.h"
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

// Keep-alive times must be a minimum of 1 second
#define MINIMUM_KEEPALIVE 1000


class PlainSocket : public SocketStream
{
public:
  // Constructor for an active connection, socket created later
	PlainSocket(HANDLE p_stopEvent);
  // Constructor for an already connected socket
  PlainSocket(SOCKET p_socket,HANDLE p_stopEvent);
  // Destructor
	virtual ~PlainSocket();

  // Initialize the socket for connection. Can throw
  void  Initialize();

  // Make a connection to a host/port number combination
	bool  Connect(LPCTSTR p_hostName,USHORT p_portNumber);

  // Receives exactly p_length bytes of data and returns the amount received - or SOCKET_ERROR if it times out
  int   RecvMsg    (LPVOID p_buffer, const ULONG p_length) override;
  // Sends    exactly p_length bytes of data and returns the amount sent     - or SOCKET_ERROR if it times out
  int   SendMsg    (LPCVOID p_buffer,const ULONG p_length) override;
  // Receives up to   p_length bytes of data and returns the amount received - or SOCKET_ERROR if it times out
	int   RecvPartial(LPVOID p_buffer, const ULONG p_length) override;
  // Sends up to      p_length bytes of data and returns the amount sent     - or SOCKET_ERROR if it times out
	int   SendPartial(LPCVOID p_buffer,const ULONG p_length) override;

  // Set up SSL/TLS state for this connection: NEVER USED ON PLAIN SOCKETS! Only on derived classes!!
  HRESULT InitializeSSL(const void* p_buffer = nullptr,const int p_length = 0) override;

  // Returns true if the close worked for both sides
  bool  Close(void) override;
  // Shutdown both sides or just one of the sides
  int   Disconnect(int p_how = SD_BOTH) override;
  // Last error state
	DWORD GetLastError() override;

  // SETTERS
  void  SetRecvTimeoutSeconds(int  p_newTimeoutSeconds);
  void  SetSendTimeoutSeconds(int  p_newTimeoutSeconds);
  bool  SetUseKeepAlive      (bool p_keepalive);
  bool  SetKeepaliveTime     (int  p_time);
  bool  SetKeepaliveInterval (int  p_interval);

  // GETTERS
  int   GetRecvTimeoutSeconds()   { return m_recvTimeoutSeconds;  };
  int   GetSendTimeoutSeconds()   { return m_sendTimeoutSeconds;  };
  bool  GetUseKeepalive()         { return m_useKeepalive;        };
  int   GetKeepaliveTime()        { return m_keepaliveTime;       };
  int   GetKeepaliveInterval()    { return m_keepaliveInterval;   };

protected:
	DWORD   m_lastError  { 0       };  // Last WSA socket error or OS error
  CString m_hostName;                // Connected to this host
  USHORT  m_portNumber { 0       };  // Connected to this port

private:
  // Activate keep-alive and keep-alive times
  bool  ActivateKeepalive();

  static WSADATA  m_wsaData;
  bool            m_initDone            { false   };
  bool            m_active              { true    };  // Active = true means clientside socket, passive is serverside
	CTime           m_recvEndTime         { 0       };
	CTime           m_sendEndTime         { 0       };
	WSAEVENT        m_write_event         { nullptr };
	WSAEVENT        m_read_event          { nullptr };
	WSAOVERLAPPED   m_os                  { 0       };
	bool            m_recvInitiated       { false   };
	SOCKET          m_actualSocket        { NULL    };
  int             m_sendTimeoutSeconds  { 1       };
  int             m_recvTimeoutSeconds  { 1       };
	HANDLE          m_stopEvent           { nullptr };
  // Keep alive
  bool            m_useKeepalive        { false   };
  int             m_keepaliveTime       { 5000    };
  int             m_keepaliveInterval   { 2500    };
};

