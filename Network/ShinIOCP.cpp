#include "StdAfx.h"
#include "ShinIOCP.h"

// WSARecv와 WSASend의 Overlapped I/O 작업 처리를 위한 쓰레드
//unsigned int WINAPI CallWorkerThread(LPVOID arg)
DWORD WINAPI CallWorkerThread(LPVOID arg)
{
	CShinIOCP* pCShinIOCP = (CShinIOCP*)arg;
	pCShinIOCP->WorkerThread();
	return 0;
}

// Client의 접속을 받기위한 쓰레드
DWORD WINAPI CallAccepterThread(LPVOID arg)
{
	CShinIOCP* pCShinIOCP = (CShinIOCP*)arg;
	pCShinIOCP->AccepterThread();
	return 0;
}

CShinIOCP::CShinIOCP(void)
{
	m_nMaxWorkerThreadCnt = 0;

	m_hWorkerThread = NULL;
	m_hAccepterThread = NULL;
	m_hIOCP = NULL;
	m_fpErrorPrintFunc = NULL;

	m_bWorkerRun = false;
	m_bAccepterRun = false;
	m_bIsBegined = false;
	m_bIsAccepted = false;
	m_clientCnt = 0;	
	m_socketListen = INVALID_SOCKET;

	m_eErrorMode = eERROR_MODE_OUTPUTDEBUG;

	InitializeCriticalSection( &m_CsClientInfo );
	InitializeCriticalSection( &m_CsServerInfo );
	InitializeCriticalSection( &m_CsServerPacket );
	InitializeCriticalSection( &m_CsUserPacket );
}

CShinIOCP::~CShinIOCP(void)
{
	End();

	DeleteCriticalSection( &m_CsClientInfo );
	DeleteCriticalSection( &m_CsServerInfo );
	DeleteCriticalSection( &m_CsServerPacket );
	DeleteCriticalSection( &m_CsUserPacket );
}

// ------------------------------------------ 
// 소켓 함수 오류 출력 후 종료
// ------------------------------------------ 
void CShinIOCP::Err_Quit(LPCTSTR msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL );



	switch( m_eErrorMode )
	{
	case eERROR_MODE_MESSAGEBOX:
		{
			MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
		}
		break;

	case eERROR_MODE_OUTPUTDEBUG:
		{
			OutputDebug( _T("[ %s ] %s\n WSAGetLastError() Value : %d\n"), msg, lpMsgBuf, WSAGetLastError() );
		}
		break;
	case eERROR_MODE_OUTPUTFILE:
		{
			OutputFile( LOG_FILE_NAME, _T("[ %s ] %s\n WSAGetLastError() Value : %d\n"), msg, lpMsgBuf, WSAGetLastError() );
		}
		break;
	case eERROR_MODE_USERFUNC:
		{
			m_fpErrorPrintFunc( _T("[ %s ] %s\n WSAGetLastError() Value : %d\n"), msg, lpMsgBuf, WSAGetLastError() );
		}
		break;
	}
	LocalFree(lpMsgBuf);

	exit( 1 );
}

// ------------------------------------------ 
// 소켓 함수 오류 출력
// ------------------------------------------ 
void CShinIOCP::Err_Display(LPCTSTR msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL );

	switch( m_eErrorMode )
	{
	case eERROR_MODE_MESSAGEBOX:
		{
			MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
		}
		break;

	case eERROR_MODE_OUTPUTDEBUG:
		{
			OutputDebug( _T("[ %s ] %s\n WSAGetLastError() Value : %d\n"), msg, lpMsgBuf, WSAGetLastError() );
		}
		break;
	case eERROR_MODE_OUTPUTFILE:
		{
			OutputFile( LOG_FILE_NAME, _T("[ %s ] %s\n WSAGetLastError() Value : %d\n"), msg, lpMsgBuf, WSAGetLastError() );
		}
		break;
	case eERROR_MODE_USERFUNC:
		{
			m_fpErrorPrintFunc( _T("[ %s ] %s\n WSAGetLastError() Value : %d\n"), msg, lpMsgBuf, WSAGetLastError() );
		}
		break;
	}

	LocalFree(lpMsgBuf);
}

void CShinIOCP::ShowErrorMsg( LPCTSTR message )
{
	switch( m_eErrorMode )
	{
	case eERROR_MODE_MESSAGEBOX:
		{
			MessageBox( NULL, message, _T("Error !"), MB_OK );
		}
		break;

	case eERROR_MODE_OUTPUTDEBUG:
		{
			OutputDebugString( message );
		}
		break;
	case eERROR_MODE_OUTPUTFILE:
		{
			OutputFile( LOG_FILE_NAME, message );
		}
		break;
	case eERROR_MODE_USERFUNC:
		{
			m_fpErrorPrintFunc( message );
		}
		break;
	}
}

void CShinIOCP::Release()
{
	for( std::map<unsigned long, CShinIOCPClient>::iterator clientiter = m_mapClientInfo.begin() ; clientiter != m_mapClientInfo.end() ; ++clientiter )
	{
		CShinIOCPClient& clientInfo = clientiter->second;

		CloseSocket( clientInfo, true, false );
	}
	m_mapClientInfo.clear();

	if( m_hWorkerThread )
	{
		delete m_hWorkerThread;
		m_hWorkerThread = NULL;
	}

	// 모든 멤버 변수들 초기화
	m_hWorkerThread = NULL;
	m_hAccepterThread = NULL;
	m_hPacketThread = NULL;
	m_hIOCP = NULL;

	m_bWorkerRun = false;
	m_bAccepterRun = false;
	m_bIsBegined = false;
	m_bIsAccepted = false;
	m_clientCnt = 0;	
	m_socketListen = INVALID_SOCKET;

	m_PacketQueFromServer.clear();
	m_PacketQueFromUser.clear();
}

bool CShinIOCP::Init()
{
	Release();

	// 유효 ConnectionID 설정
	m_IDCreater.Init( START_CONNECTION_INDEX, END_CONNECTION_INDEX );

	return true;
}

bool CShinIOCP::IsConnectedServer()
{
	EnterCS enterCs(m_CsServerInfo);
	return m_setServerInfo.empty() == false;
}

void CShinIOCP::AddServer( unsigned long connectionIdx )
{
	EnterCS enterCs(m_CsServerInfo);
	m_setServerInfo.insert( connectionIdx );
}

void CShinIOCP::RemoveServer( unsigned long connectionIdx )
{
	EnterCS enterCs(m_CsServerInfo);
	m_setServerInfo.erase( connectionIdx );
}

void CShinIOCP::AddServerIP( LPCTSTR ip, u_short port )
{
	EnterCS enterCs(m_CsServerInfo);
	const DWORD hashcode = GetHashCodeFromTxt( ip );
	mServerIP.insert( std::map< DWORD, 	ServerIPInfo >::value_type( hashcode, std::make_pair( ip, port ) ) );
}

bool CShinIOCP::IsServer( unsigned long connectionIdx )
{
	EnterCS enterCs(m_CsServerInfo);
	return m_setServerInfo.find( connectionIdx ) != m_setServerInfo.end();
}

bool CShinIOCP::IsServer( LPCTSTR ip, u_short port )
{
	const DWORD hashcode = GetHashCodeFromTxt( ip );
	std::map< DWORD, ServerIPInfo >::const_iterator serveripiter = mServerIP.find( hashcode );
	if( serveripiter == mServerIP.end() )
	{
		return false;
	}

	const ServerIPInfo& serveripinfo = serveripiter->second;
	const std::basic_string<TCHAR>& savedip = serveripinfo.first;
	const int savedport = serveripinfo.second;
	if( savedip.compare( ip ) != 0 ||
		savedport != port )
	{
		return false;
	}

	return true;
}

bool CShinIOCP::Begin()
{
	if( m_bIsBegined )
	{
		End();
	}

	Init();

	WSADATA wsaData;
	// 윈속을 버젼 2,2로 초기화 한다.
	int nRet = WSAStartup( MAKEWORD(2, 2), &wsaData );
	if( 0 != nRet )
	{
		Err_Quit( _T("WSAStartup()") );
	}	

	// Worker Thread 실행, IOCP 생성
	if( !StartIocp() )
	{
		End();
		return false;
	}

	m_bIsBegined = true;

	return true;
}

bool CShinIOCP::BeginAccept( u_short port )
{
	m_bAccepterRun = true;

	// TCP 소켓 생성
	m_socketListen = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED );
	if( INVALID_SOCKET == m_socketListen )
	{
		End();
		return false;
	}
	BOOL bReUse = TRUE;
	if( setsockopt( m_socketListen, SOL_SOCKET, SO_REUSEADDR, (char*)&bReUse, sizeof(bReUse) ) == SOCKET_ERROR )
	{
		Err_Display( ___FUNCTION___ );
		closesocket( m_socketListen );
		return false;
	}

	
	if( !BindandListen( port ) )
	{
		End();
		return false;
	}

	CreateAccepterThread();
	return true;
}

bool CShinIOCP::End()
{
	if( !m_bIsBegined )
		return false;

	// Thread 종료
	DestroyThread();

	Release();

	// 윈속의 사용을 끝낸다.
	WSACleanup();

	return true;
}
	
bool CShinIOCP::InitConnectSocket( CShinIOCPClient& clientInfo )
{
	// TCP 소켓 생성
	SOCKET socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED );
	if( INVALID_SOCKET == socket )
	{
		return false;
	}

	clientInfo.SetSocket( socket );

	return true;
}

bool CShinIOCP::ConnectServer( LPCTSTR ip, u_short port, unsigned long& connectionIdx )
{
	if( !m_bIsBegined )
	{
		Begin();
	}

	SOCKADDR_IN addr;
	ZeroMemory( &addr, sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_port = htons( port );
	addr.sin_addr.s_addr = inet_addr( WideToMulti( ip ) );

	CShinIOCPClient& clientInfo = GetEmptyClientInfo();
	AddServer( clientInfo.GetConnectionIdx() );

	// TCP 소켓 생성
	SOCKET socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED );
	if( INVALID_SOCKET == socket )
	{
		Err_Display( ___FUNCTION___ );
		CloseSocket( clientInfo );
		return false;
	}

	clientInfo.SetSocket( socket );

	if( connect(clientInfo.GetSocket(), (SOCKADDR*)&addr, sizeof(addr)) )
	{
		Err_Display( ___FUNCTION___ );
		CloseSocket( clientInfo );
		return false;
	}

	//// setsockopt()
	//BOOL bNoDelay = TRUE;
	//if( setsockopt( m_pServerInfo->m_socketClient, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay) ) == SOCKET_ERROR )
	//{
	//	Err_Display( ___FUNCTION___ );
	//}

	// I/O CompletionPort 객체와 소켓을 연결시킨다.
	bool bRet = BindIOCP( clientInfo );
	if( !bRet )
	{
		CloseSocket( clientInfo );
		return false;
	}

	// Recv
	bRet = BindRecv( clientInfo );
	if( !bRet )
	{
		CloseSocket( clientInfo );
		return false;
	}

	connectionIdx = clientInfo.GetConnectionIdx();

	return true;
}

bool CShinIOCP::DisConnectServer( unsigned long connectionIdx )
{
	CShinIOCPClient& clientInfo = GetClientInfo( connectionIdx );
	if( clientInfo.GetConnectionIdx() != connectionIdx )
		return false;

	CloseSocket( clientInfo );

	return true;
}

bool CShinIOCP::BindandListen( int nBindPort )
{
	SOCKADDR_IN stServerAddr;
	stServerAddr.sin_family = AF_INET;
	// 서버 포트를 설정한다.
	stServerAddr.sin_port = htons( (u_short)nBindPort );
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 위에서 지정한 서버 주소정보와 CShinIOCP소켓을 연결한다.
	int nRet = bind( m_socketListen, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN) );
	if( nRet != 0 )
	{
		return false;
	}

	// 접속요청을 받아들이기 위해 IOCP에 소켓을 등록하고 접속 대기큐를 5개로 설정한다.
	nRet = listen( m_socketListen, 5 );
	if( nRet != 0 )
	{
		return false;
	}
	return true;
}

bool CShinIOCP::StartIocp()
{
	// CompletionPort객체 생성 요청을 한다.
	m_hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, NULL, 0 );
	if( m_hIOCP == NULL )
	{
		return false;
	}

	m_bWorkerRun = true;
	bool bRet = CreateWorkerThread();
	if( !bRet )
		return false;

	return true;
}

void CShinIOCP::CloseSocket( CShinIOCPClient& clientInfo, bool bIsForce, bool IsEraseClientInfo )
{
	EnterCS enterCs(m_CsClientInfo);

	if( IsClient( clientInfo.GetConnectionIdx() ) == false )
	{
		return;
	}

	// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여
	// 강제종료 시킨다. 주의:데이터 손실이 있을 수 있음
	if( bIsForce )
	{
		struct linger stLinger = { 0, 0 };		// SO_DONTLINGER 로 설정

//		stLinger.l_linger = 30;
//		stLinger.l_onoff = 1;

		// 소켓 옵션을 설정한다.
		setsockopt( clientInfo.GetSocket(), SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger) );

		// socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
		shutdown( clientInfo.GetSocket(), SD_BOTH );
	}

	closesocket( clientInfo.GetSocket() );
	clientInfo.SetSocket( INVALID_SOCKET );

	stPacketInfo packet;
	packet.senderID = clientInfo.GetConnectionIdx();
	packet.pBuf = NULL;
	packet.nBufLen = 0;
	packet.opeartion = OP_DISCONNECTED;

	// 접속 종료 처리
	if( IsServer( clientInfo.GetConnectionIdx() ) )
	{
		EnterCS enterCs(m_CsServerPacket);
		m_PacketQueFromServer.push_back( packet );
		RemoveServer( clientInfo.GetConnectionIdx() );
	}
	else
	{
		EnterCS enterCs(m_CsUserPacket);
		m_PacketQueFromUser.push_back( packet );
		const int cnt = m_clientCnt - 1;
		m_clientCnt = std::max( cnt, 0 );
	}

	// 유효 ID 반납
	m_IDCreater.ReturnID( clientInfo.GetConnectionIdx() );

	clientInfo.Release();

	// 정보 제거
	if( IsEraseClientInfo )
		m_mapClientInfo.erase( clientInfo.GetConnectionIdx() );
}

bool CShinIOCP::CreateWorkerThread()
{
	DWORD uiThreadId = 0;
	// WaitingThread Queue에 대기 상태로 넣을 쓰레드를 생성
	// 권장 되는 갯수 : (cpu갯수 * 2) + 1
	if( m_nMaxWorkerThreadCnt == 0 )
	{
		SYSTEM_INFO si;
		GetSystemInfo( &si );
		m_nMaxWorkerThreadCnt = si.dwNumberOfProcessors * 2 + 1;
	}

	if( m_hWorkerThread )
	{
		delete[] m_hWorkerThread;
		m_hWorkerThread = NULL;
	}

	m_hWorkerThread = new HANDLE[ m_nMaxWorkerThreadCnt ];

	for( int i=0 ; i<m_nMaxWorkerThreadCnt ; i++ )
	{
//		m_hWorkerThread[i] = (HANDLE)_beginthreadex( NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &uiThreadId );
		m_hWorkerThread[i] = CreateThread( NULL, 0, CallWorkerThread, this, CREATE_SUSPENDED, &uiThreadId );
		if( m_hWorkerThread == NULL )
		{
			return false;
		}
		ResumeThread( m_hWorkerThread[i] );
	}

	return true;
}

bool CShinIOCP::CreateAccepterThread()
{
	DWORD uiThreadId = 0;
	// Client의 접속요청을 받을 쓰레드 생성
//	m_hAccepterThread = (HANDLE)_beginthreadex( NULL, 0, &CallAccepterThread, this, CREATE_SUSPENDED, &uiThreadId );
	m_hAccepterThread = CreateThread( NULL, 0, CallAccepterThread, this, CREATE_SUSPENDED, &uiThreadId );
	if( m_hAccepterThread == NULL )
	{
		return false;
	}

	ResumeThread( m_hAccepterThread );
	return true;
}

bool CShinIOCP::BindIOCP( CShinIOCPClient& clientInfo )
{
	HANDLE hIOCP;
	hIOCP = CreateIoCompletionPort( (HANDLE)clientInfo.GetSocket(), m_hIOCP, (ULONG_PTR)(&clientInfo.GetConnectionIdx()), 0 );
	if( hIOCP == NULL || m_hIOCP != hIOCP )
	{
		return false;
	}
	return true;
}

bool CShinIOCP::BindRecv( CShinIOCPClient& clientInfo )
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;
	char* pBuf = NULL;
	stOverlappedEx* pOverlappedEx = NULL;
	WSABUF wsabuf;

	if( clientInfo.GetSocket() == INVALID_SOCKET )
		return false;

	// OverlappedEx + 메세지버퍼의 크기만큼 링버퍼에서 얻어온다.
	const size_t nAllocBufSize = sizeof(stOverlappedEx) + DEFAULT_PACKET_SIZE;
	pBuf = (char*)clientInfo.GetBuffer( nAllocBufSize );
	if( pBuf == NULL )
		return false;

	// overlappedEx의 시작주소 설정
	pOverlappedEx = reinterpret_cast<stOverlappedEx*>( pBuf ) ;

	// 버퍼의 시작주소 설정
	pBuf = (pBuf + sizeof(stOverlappedEx));

	// 메세지 버퍼 초기화
	ZeroMemory( pBuf, DEFAULT_PACKET_SIZE );

	// Overlapped I/O를 위해 각정보를 셋팅해준다.
	pOverlappedEx->m_wsaBuf.len = DEFAULT_PACKET_SIZE;
	pOverlappedEx->m_nBufLen = DEFAULT_PACKET_SIZE;
	pOverlappedEx->m_wsaBuf.buf = pBuf;
	pOverlappedEx->m_pBufBegin = pBuf;
	pOverlappedEx->m_eOperation = OP_RECV;
	pOverlappedEx->m_nRemainLen = 0;
	wsabuf.buf = pBuf;
	wsabuf.len = DEFAULT_PACKET_SIZE;

	ZeroMemory( &pOverlappedEx->m_wsaOverlapped, sizeof(OVERLAPPED) );

	int nRet = WSARecv( clientInfo.GetSocket(), 
						&wsabuf, 
//						&(pOverlappedEx->m_wsaBuf),
						1, 
						&dwRecvNumBytes, 
						&dwFlag, 
						(LPWSAOVERLAPPED)pOverlappedEx, 
						NULL );

	// socket_error 이면 Client Socket이 끊어진걸로 처리한다.
	if( nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING) )
	{
		Err_Display( ___FUNCTION___ );
		return false;
	}

	return true;
}

bool CShinIOCP::SendMsg( CShinIOCPClient& clientInfo, LPCTSTR pMsg, size_t nLen )
{
	DWORD dwSendNumBytes = 0;
	char* pBegin = NULL;
	stOverlappedEx* pOverlappedEx = NULL;
	WSABUF wsabuf;

	if( clientInfo.GetSocket() == INVALID_SOCKET ||
		pMsg == NULL || 
		nLen <= 0 )
		return false;

	const size_t nPacketSize = nLen + sizeof(size_t);
	const size_t nAllocBufSize = sizeof(stOverlappedEx) + nPacketSize + 1;

	// OverlappedEx + 메세지버퍼의 크기만큼 링버퍼에서 얻어온다.
	pBegin = (char*)clientInfo.GetBuffer( nAllocBufSize );
	if( pBegin == NULL )
		return false;

	// overlappedEx의 시작주소 설정
	pOverlappedEx = reinterpret_cast<stOverlappedEx*>( pBegin );
	
	// 버퍼의 시작주소 설정
	pBegin = (pBegin + sizeof(stOverlappedEx));
	
	// 메세지 헤더 생성 [ 패킷 사이즈(size_t) + Data ]
	memcpy( pBegin, &nPacketSize, sizeof(size_t) );

	// 전송될 메세지를 복사
	CopyMemory( (pBegin + sizeof(size_t)), pMsg, nLen );

	// Overlapped I/O를 위해 각정보를 셋팅해준다.
	pOverlappedEx->m_wsaBuf.len = (u_long)nPacketSize;
	pOverlappedEx->m_nBufLen = nPacketSize;
	pOverlappedEx->m_wsaBuf.buf = pBegin;
	pOverlappedEx->m_pBufBegin = pBegin;
	pOverlappedEx->m_eOperation = OP_SEND;
	pOverlappedEx->m_nRemainLen = 0;

	wsabuf.buf = pBegin;
	wsabuf.len = (u_long)nPacketSize;

	ZeroMemory( &pOverlappedEx->m_wsaOverlapped, sizeof(OVERLAPPED) );

	int nRet = WSASend( clientInfo.GetSocket(), 
						&(wsabuf), 
						1, 
						&dwSendNumBytes, 
						0, 
						(LPWSAOVERLAPPED)pOverlappedEx,
						NULL );

	// socket_error 이면 Client Socket이 끊어진걸로 처리한다.
	if( nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING) )
	{
		Err_Display( ___FUNCTION___ );
		return false;
	}

	return true;
}

bool CShinIOCP::SendMsg( unsigned long connectionIdx, LPCTSTR pMsg, size_t nLen )
{
	CShinIOCPClient& clientInfo = GetClientInfo( connectionIdx );

	if( !SendMsg( clientInfo, pMsg, nLen ) )
		return false;

	return true;
}

bool CShinIOCP::Broadcast2Client( LPCTSTR pMsg, size_t nLen )
{
	bool bSuccess = true;
	for( std::map<unsigned long, CShinIOCPClient>::iterator clientiter = m_mapClientInfo.begin() ; clientiter != m_mapClientInfo.end() ; ++clientiter )
	{
		CShinIOCPClient& clientInfo = clientiter->second;
		if( !SendMsg( clientInfo, pMsg, nLen ) )
			bSuccess = false;
	}

	return bSuccess;
}

bool CShinIOCP::Broadcast2Server( LPCTSTR pMsg, size_t nLen )
{
	DWORD dwSendNumBytes = 0;
	char* pBegin = NULL;
	stOverlappedEx* pOverlappedEx = NULL;
	WSABUF wsabuf;

	if( pMsg == NULL || nLen <= 0 )
		return false;

	EnterCS enterCs( m_CsServerInfo );
	std::set<unsigned long> disconnectserver;
	for( std::set<unsigned long>::const_iterator serveriter = m_setServerInfo.begin() ; serveriter != m_setServerInfo.end() ; ++serveriter )
	{
		const unsigned long serverConnectionIdx = *serveriter;
		CShinIOCPClient& serverInfo = GetClientInfo( serverConnectionIdx );

		const size_t nPacketSize = nLen + sizeof(size_t);
		const size_t nAllocBufSize = sizeof(stOverlappedEx) + nPacketSize + 1;

		// OverlappedEx + 메세지버퍼의 크기만큼 링버퍼에서 얻어온다.
		pBegin = (char*)serverInfo.GetBuffer( sizeof(stOverlappedEx) + nPacketSize );
		if( pBegin == NULL )
			return false;

		// overlappedEx의 시작주소 설정
		pOverlappedEx = reinterpret_cast<stOverlappedEx*>( pBegin ) ;

		// 버퍼의 시작주소 설정
		pBegin = (pBegin + sizeof(stOverlappedEx));
		
		// 메세지 헤더 생성 [ 패킷 사이즈(size_t) + Data ]
		memcpy( pBegin, &nPacketSize, sizeof(size_t) );

		// 전송될 메세지를 복사
		CopyMemory( (pBegin + sizeof(size_t)), pMsg, nLen );

		// Overlapped I/O를 위해 각정보를 셋팅해준다.
		pOverlappedEx->m_wsaBuf.len = (u_long)nPacketSize;
		pOverlappedEx->m_nBufLen = nPacketSize;
		pOverlappedEx->m_wsaBuf.buf = pBegin;
		pOverlappedEx->m_pBufBegin = pBegin;
		pOverlappedEx->m_eOperation = OP_SEND;
		pOverlappedEx->m_nRemainLen = 0;


		wsabuf.buf = pBegin;
		wsabuf.len = (u_long)nPacketSize;

		ZeroMemory( &pOverlappedEx->m_wsaOverlapped, sizeof(OVERLAPPED) );

		int nRet = WSASend( serverInfo.GetSocket(), 
			&(wsabuf), 
			1, 
			&dwSendNumBytes, 
			0, 
			(LPWSAOVERLAPPED)pOverlappedEx,
			NULL );

		// socket_error 이면 Client Socket이 끊어진걸로 처리한다.
		if( nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING) )
		{
			Err_Display( ___FUNCTION___ );
			disconnectserver.insert( serverConnectionIdx );
			continue;
		}
	}

	for( std::set<unsigned long>::const_iterator serveriter = disconnectserver.begin() ; serveriter != disconnectserver.end() ; ++serveriter )
	{
		const unsigned long serverConnectionIdx = *serveriter;
		CShinIOCPClient& serverInfo = GetClientInfo( serverConnectionIdx );
		CloseSocket( serverInfo );
	}

	return true;

}

bool CShinIOCP::ReRecv( CShinIOCPClient& clientInfo, stOverlappedEx *pOverlappedEx )
{
	if( pOverlappedEx == NULL ||
		pOverlappedEx->m_wsaBuf.buf == NULL ||
		pOverlappedEx->m_wsaBuf.len <= 0 ||
		clientInfo.GetSocket() == INVALID_SOCKET )
		return false;

	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;
	WSABUF wsabuf;

	wsabuf.buf = pOverlappedEx->m_wsaBuf.buf;
	wsabuf.len = pOverlappedEx->m_wsaBuf.len;

	ZeroMemory( &pOverlappedEx->m_wsaOverlapped, sizeof(OVERLAPPED) );

	int nRet = WSARecv( clientInfo.GetSocket(), 
						&wsabuf, 
//						&(pOverlappedEx->m_wsaBuf),
						1, 
						&dwRecvNumBytes, 
						&dwFlag, 
						(LPWSAOVERLAPPED)pOverlappedEx, 
						NULL );

	// socket_error 이면 Client Socket이 끊어진걸로 처리한다.
	if( nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING) )
	{
		return false;
	}

	return true;
}

bool CShinIOCP::ReSendMsg( CShinIOCPClient& clientInfo, stOverlappedEx *pOverlappedEx )
{
	if( pOverlappedEx == NULL ||
		pOverlappedEx->m_wsaBuf.buf == NULL ||
		pOverlappedEx->m_wsaBuf.len <= 0 ||
		clientInfo.GetSocket() == INVALID_SOCKET )
		return false;

	DWORD dwSendNumBytes = 0;
	WSABUF wsabuf;

	wsabuf.buf = pOverlappedEx->m_wsaBuf.buf;
	wsabuf.len = pOverlappedEx->m_wsaBuf.len;

	ZeroMemory( &pOverlappedEx->m_wsaOverlapped, sizeof(OVERLAPPED) );

	int nRet = WSASend( clientInfo.GetSocket(), 
						&wsabuf, 
						1, 
						&dwSendNumBytes, 
						0, 
						(LPWSAOVERLAPPED)pOverlappedEx,
						NULL );

	// socket_error 이면 Client Socket이 끊어진걸로 처리한다.
	if( nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING) )
	{
		return false;
	}

	return true;
}

CShinIOCPClient& CShinIOCP::GetEmptyClientInfo()
{
	EnterCS enterCs(m_CsClientInfo);

	// 유효 ConnectionID 부여
	unsigned long newID = m_IDCreater.GetNewID();
	if( newID == (unsigned long)-1 )
	{
		static CShinIOCPClient emptyClientInfo;
		ShowErrorMsg( ___FUNCTION___ );
		return emptyClientInfo;
	}

	CShinIOCPClient& clientInfo = m_mapClientInfo[ newID ];
	clientInfo.Initialize( newID );

	return clientInfo;
}

CShinIOCPClient& CShinIOCP::GetClientInfo( unsigned long connectionIdx )
{
	static CShinIOCPClient emptyClientInfo;
	std::map<unsigned long, CShinIOCPClient>::iterator clientiter = m_mapClientInfo.find( connectionIdx );
	if( clientiter == m_mapClientInfo.end() )
	{
		return emptyClientInfo;
	}

	return clientiter->second;
}

bool CShinIOCP::IsClient( unsigned long connectionIdx )
{
	return m_mapClientInfo.find( connectionIdx ) != m_mapClientInfo.end();
}

void CShinIOCP::SetWorkerThreadCount( int threadCount )
{
	m_nMaxWorkerThreadCnt = std::max( threadCount, 1 );
}

void CShinIOCP::SetErrorPrintFunc( void (*printfunc)(LPCTSTR, ...) )
{
	m_fpErrorPrintFunc = printfunc;
}

const stPacketInfo CShinIOCP::GetPacketFromServer()
{
	if( m_PacketQueFromServer.empty() )
	{
		static stPacketInfo emptyPacket;
		return emptyPacket;
	}

	EnterCS enterCs(m_CsServerPacket);
	const stPacketInfo& packet = m_PacketQueFromServer.front();

	return packet;
}

const stPacketInfo	CShinIOCP::GetPacketFromUser()
{
	if( m_PacketQueFromUser.empty() )
	{
		static stPacketInfo emptyPacket;
		return emptyPacket;
	}

	EnterCS enterCs(m_CsUserPacket);
	const stPacketInfo& packet = m_PacketQueFromUser.front();
	return packet;
}

void CShinIOCP::PopPacketFromServer()
{
	if( m_PacketQueFromServer.empty() )
		return;

	EnterCS enterCs(m_CsServerPacket);
	stPacketInfo& packet = m_PacketQueFromServer.front();
	delete[] (char*)packet.pBuf;
	m_PacketQueFromServer.pop_front();
}

void CShinIOCP::PopPacketFromUser()
{
	if( m_PacketQueFromUser.empty() )
		return;

	EnterCS enterCs(m_CsUserPacket);
	stPacketInfo& packet = m_PacketQueFromUser.front();
	delete [] (char*)packet.pBuf;
	m_PacketQueFromUser.pop_front();
}

void CShinIOCP::DestroyThread()
{	
	if( m_hIOCP != NULL )
	{
		for( int i=0 ; i<m_nMaxWorkerThreadCnt ; i++ )
		{
			PostQueuedCompletionStatus( m_hIOCP, 0, 0, NULL );
			if( m_hWorkerThread[i] != NULL )
			{
				WaitForSingleObject( m_hWorkerThread[i], 200 );
				// 쓰레드 핸들을 닫고 쓰레드가 종료될 때까지 기다린다.
				CloseHandle( m_hWorkerThread[i] );
				m_hWorkerThread[i] = NULL;
			}
		}
	}

	if( m_bAccepterRun )
	{
		m_bAccepterRun = false;
		closesocket( m_socketListen );
		WaitForSingleObject( m_hAccepterThread, 200 );
		CloseHandle( m_hAccepterThread );
		// 쓰레드 종료를 기다린다.

		m_hAccepterThread = NULL;
	}
}

void CShinIOCP::AccepterThread()
{
	SOCKADDR_IN stClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);

	m_bIsAccepted = true;
	
	while( m_bAccepterRun )
	{
		// 클라이언트 접속 요청이 들어올 때까지 기다린다.
		SOCKET acceptSocket = accept( m_socketListen, (SOCKADDR*)&stClientAddr, &nAddrLen );
		if( acceptSocket == INVALID_SOCKET )
			continue;

		CShinIOCPClient& clientInfo = GetEmptyClientInfo();
		clientInfo.SetSocket( acceptSocket );

		TCHAR connectedip[MAX_PATH]={0};
		const char* ipaddr = inet_ntoa(stClientAddr.sin_addr);
		const u_short portaddr = ntohs(stClientAddr.sin_port);
		_tcsncpy_s( connectedip, _countof(connectedip), MultiToWide(ipaddr), strlen(ipaddr) );
		if( m_fpErrorPrintFunc )
			m_fpErrorPrintFunc( _T("(%s:%d) 접속"), connectedip, portaddr );

		//// setsockopt()
		//BOOL bNoDelay = TRUE;
		//if( setsockopt( pClientInfo->m_socketClient, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(bNoDelay) ) == SOCKET_ERROR )
		//{
		//	Err_Display( ___FUNCTION___ );
		//	CloseSocket( pClientInfo );
		//	continue;
		//}

		// I/O CompletionPort 객체와 소켓을 연결시킨다.
		bool bRet = BindIOCP( clientInfo );
		if( !bRet )
		{
			ShowErrorMsg( _T("AccepterThread() - BindIOCP()") );
			CloseSocket( clientInfo );
			continue;
		}

		// Recv
		bRet = BindRecv( clientInfo );
		if( !bRet )
		{
			ShowErrorMsg( _T("AccepterThread() - BindRecv()") );
			CloseSocket( clientInfo );
			continue;
		}

		// 클라이언트 갯수 증가
		m_clientCnt++;

		stPacketInfo packet;
		packet.senderID = clientInfo.GetConnectionIdx();
		packet.pBuf = NULL;
		packet.nBufLen = 0;
		packet.opeartion = OP_CONNECTED;

		if( IsServer( connectedip, portaddr ) == true )
		{
			AddServer( clientInfo.GetConnectionIdx() );
			EnterCS enterCs(m_CsServerPacket);
			m_PacketQueFromServer.push_back( packet );
		}
		else
		{
			EnterCS enterCs(m_CsUserPacket);
			m_PacketQueFromUser.push_back( packet );
		}
	}

	m_bIsAccepted = false;
}

void CShinIOCP::WorkerThread()
{
	// CompletionKey를 받을 포인터 변수
	unsigned long* connectID = 0;
	// 함수 호출 성공 여부
	BOOL bSuccess = TRUE;
	// Overlapped I/O작업에서 전송된 데이터 크기
	DWORD dwIoSize = 0;
	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
	LPOVERLAPPED lpOverlapped = NULL;

	while( m_bWorkerRun )
	{
		bSuccess = GetQueuedCompletionStatus( m_hIOCP,
											&dwIoSize,
											(PULONG_PTR)&connectID,
											&lpOverlapped,
											INFINITE );
		// Failed..
		if( !bSuccess )
		{
			// Error..
			if( lpOverlapped == NULL || connectID == NULL )
			{
				Err_Display( ___FUNCTION___ );
				continue;
			}
			else
			{
				CShinIOCPClient& clientInfo = GetClientInfo( *connectID );
				CloseSocket( clientInfo );
				continue;
			}
		}

		if( dwIoSize == 0 )
		{
			// 사용자 쓰레드 종료 메세지 처리
			if( lpOverlapped == NULL && connectID == NULL )
			{
				m_bWorkerRun = false;
				break;
			}
			// client가 접속을 끊었을 경우
			else if( IsClient( *connectID ) )
			{
				CShinIOCPClient& clientInfo = GetClientInfo( *connectID );
				CloseSocket( clientInfo );
				continue;
			}
		}

		CShinIOCPClient& clientInfo = GetClientInfo( *connectID );

		stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

		// Overlapped I/O Recv작업 결과 뒤 처리
		// 데이터를 수신한 경우
		if( pOverlappedEx->m_eOperation == OP_RECV )
		{
			pOverlappedEx->m_nRemainLen += dwIoSize;

			//////////////////////
			while(1)
			{
				// I/O 크기가 size_t보다 작은경우 재수신
				if( pOverlappedEx->m_nRemainLen < sizeof(size_t) )
				{
					// 재 수신
					pOverlappedEx->m_wsaBuf.buf = pOverlappedEx->m_pBufBegin + pOverlappedEx->m_nRemainLen;
					pOverlappedEx->m_wsaBuf.len = (u_long)(pOverlappedEx->m_nBufLen - pOverlappedEx->m_nRemainLen);
					ReRecv( clientInfo, pOverlappedEx );
					break;
				}

				// 패킷 크기 확인
				size_t nPacketSize;
				memcpy( &nPacketSize, pOverlappedEx->m_pBufBegin, sizeof(size_t) );

				// 패킷 크기가 기본버퍼보다 큰경우
				if( nPacketSize > clientInfo.GetBufferSize( pOverlappedEx ) )
				{
					// 버퍼 재할당
					const size_t allocBufSize = sizeof(stOverlappedEx) + nPacketSize;
					stOverlappedEx* pNewOverlappedEx = (stOverlappedEx*)clientInfo.GetBuffer( allocBufSize );
					char* pBeginBuf = (char*)pNewOverlappedEx + sizeof(stOverlappedEx);

					pNewOverlappedEx->m_eOperation = OP_RECV;
					pNewOverlappedEx->m_nRemainLen = pOverlappedEx->m_nRemainLen;
					pNewOverlappedEx->m_pBufBegin = pBeginBuf;
					pNewOverlappedEx->m_nBufLen = nPacketSize;
					pNewOverlappedEx->m_wsaBuf.buf = pBeginBuf + pOverlappedEx->m_nRemainLen;
					pNewOverlappedEx->m_wsaBuf.len = (u_long)nPacketSize - pOverlappedEx->m_nRemainLen;
					memcpy( pBeginBuf, pOverlappedEx->m_pBufBegin, pOverlappedEx->m_nRemainLen );
					clientInfo.ReleaseBuffer( pOverlappedEx );
					pOverlappedEx = pNewOverlappedEx;

					// 재 수신
					ReRecv( clientInfo, pOverlappedEx );
					break;
				}

				// 전부 수신된 경우
				if( nPacketSize <= pOverlappedEx->m_nRemainLen )
				{
					stPacketInfo packet;
					packet.senderID = clientInfo.GetConnectionIdx();
					packet.nBufLen = nPacketSize - sizeof(size_t);
					packet.pBuf = (void*)new char[ packet.nBufLen ];
					packet.opeartion = OP_RECV;
					if( packet.pBuf == NULL )
					{
						ShowErrorMsg( _T("Worker - GetBuf() Error") );
						break;
					}
					// 데이터 복사
					CopyMemory( packet.pBuf, pOverlappedEx->m_pBufBegin + sizeof(size_t), packet.nBufLen );
					

					// 패킷 추가
					if( IsServer( clientInfo.GetConnectionIdx() ) == true )
					{
						EnterCS enterCs(m_CsServerPacket);
						m_PacketQueFromServer.push_back( packet );
					}
					else
					{
						EnterCS enterCs(m_CsUserPacket);
						m_PacketQueFromUser.push_back( packet );
					}

					// 한개 이상의 패킷이 온경우
					if( pOverlappedEx->m_nRemainLen > nPacketSize )
					{
						// 버퍼 설정, 복사
						stOverlappedEx* pTempOverlappedEx = NULL;
						pTempOverlappedEx = (stOverlappedEx*)( clientInfo.GetBuffer( (DEFAULT_PACKET_SIZE - nPacketSize) + sizeof(stOverlappedEx) ) );
						CopyMemory( pTempOverlappedEx, pOverlappedEx, sizeof(stOverlappedEx) );

						pTempOverlappedEx->m_pBufBegin = (char*)(pTempOverlappedEx) + sizeof(stOverlappedEx);						
						pTempOverlappedEx->m_nRemainLen -= nPacketSize;

						CopyMemory( pTempOverlappedEx->m_pBufBegin, (pOverlappedEx->m_pBufBegin + nPacketSize), pTempOverlappedEx->m_nRemainLen);
						pTempOverlappedEx->m_wsaBuf.buf = pTempOverlappedEx->m_pBufBegin;
						pTempOverlappedEx->m_wsaBuf.len = (u_long)pTempOverlappedEx->m_nRemainLen;
						pTempOverlappedEx->m_nBufLen = DEFAULT_PACKET_SIZE;

						clientInfo.ReleaseBuffer( (void*)pOverlappedEx );

						pOverlappedEx = pTempOverlappedEx;

						continue;
					}
					// 한개만 온 경우
					else
					{
						clientInfo.ReleaseBuffer( (void*)pOverlappedEx );

						if( !BindRecv( clientInfo ) )
						{
							// 수신 실패 접속을 끝는다
							CloseSocket( clientInfo );

							break;
						}

						break;
					}
				}
				else
				{
					// 재 수신
					pOverlappedEx->m_wsaBuf.buf = pOverlappedEx->m_pBufBegin + pOverlappedEx->m_nRemainLen;
					pOverlappedEx->m_wsaBuf.len = (u_long)(pOverlappedEx->m_nBufLen - pOverlappedEx->m_nRemainLen);
					ReRecv( clientInfo, pOverlappedEx );
					break;
				}
			}// End while(1)
			//////////////////////
		}
		// 데이터를 전송한 경우
		else if( pOverlappedEx->m_eOperation == OP_SEND )
		{
			pOverlappedEx->m_nRemainLen += dwIoSize;
			// 전부 전송된 경우
			if( pOverlappedEx->m_nBufLen <= pOverlappedEx->m_nRemainLen )
			{
				// RingBuffer Release
				clientInfo.ReleaseBuffer( (void*)pOverlappedEx );
			}
			else
			{
				// 재 전송
				pOverlappedEx->m_wsaBuf.buf = pOverlappedEx->m_pBufBegin + pOverlappedEx->m_nRemainLen;
				pOverlappedEx->m_wsaBuf.len = (u_long)(pOverlappedEx->m_nBufLen - pOverlappedEx->m_nRemainLen);
				ReSendMsg( clientInfo, pOverlappedEx );
			}
		}
	}
}