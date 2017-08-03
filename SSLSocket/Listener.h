/////////////////////////////////////////////////////////////////////////////
// 
// Listener
//
// Original idea:
// David Maw: https://www.codeproject.com/Articles/1000189/A-Working-TCP-Client-and-Server-With-SSL
// License:   https://www.codeproject.com/info/cpol10.aspx
//
#pragma once
#include "StdAfx.h"
#include <functional>
#include <wincrypt.h>
#include "SocketStream.h"

class Listener
{
public:
	enum ErrorType
  {
		NoError,
		UnknownError,
		SocketInuse,
		SocketUnusable
	};
	Listener();
 ~Listener();

public:
  // Initialize the listener
  ErrorType Initialize(int p_tcpListenPort);

  void BeginListening(std::function<void(SocketStream* p_serverSocket)> actualwork);
  void EndListening(void);

  std::function<SECURITY_STATUS(PCCERT_CONTEXT & pCertContext,LPCTSTR pszSubjectName)> m_selectServerCert;
  std::function<bool(PCCERT_CONTEXT pCertContext,const bool trusted)> m_clientCertAcceptable;

  int     m_workerThreadCount;
  CEvent  m_stopEvent;
private:
  static UINT __cdecl Worker(LPVOID);
  static UINT __cdecl ListenerWorker(LPVOID);
  void                Listen(void);

  std::function<void(SocketStream* p_streamSock)> m_actualwork;
  SOCKET            m_listenSockets[FD_SETSIZE];
  HANDLE            m_hSocketEvents[FD_SETSIZE];
  int               m_numListenSockets;
  CCriticalSection  m_workerThreadLock;
  CWinThread*       m_listenerThread;
};

