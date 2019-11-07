// ***************************************************************************************** //
// ***************************************************************************************** //
//					최종 수정일 : 2011년 3월 10일
//                  만 든 이    : 신 진 섭 
//					E-Mail      : side1024@nate.com
// ***************************************************************************************** //
// ***************************************************************************************** //

#ifndef CSHINIOCP_H
#define CSHINIOCP_H

#include <windows.h>
#include <winsock2.h>
#include <list>
#include <map>
#include <deque>
#include "IDCreater.h"
#include "ShinIOCPClient.h"

#pragma comment (lib, "ws2_32.lib" )

#define DEFAULT_PACKET_SIZE		256
#define DEFAULT_OPEN_PORT		14400				// 기본 Listen Port
#define START_CONNECTION_INDEX	1					// 접속 Index 시작번호
#define END_CONNECTION_INDEX	ULONG_MAX			// 접속 Index 끝번호

enum enumOperation{
	OP_RECV = 99,
	OP_SEND,
	OP_CONNECTED,
	OP_DISCONNECTED,
};

struct stOverlappedEx{
	WSAOVERLAPPED		m_wsaOverlapped;
	WSABUF				m_wsaBuf;
	char*				m_pBufBegin;			// 버퍼 포인터 저장
	size_t				m_nBufLen;				// 버퍼 크기 저장
	size_t				m_nRemainLen;			// I/O 크기를 저장
	enumOperation		m_eOperation;
};

struct stPacketInfo{
	unsigned long senderID;
	enumOperation opeartion;
	void* pBuf;
	size_t nBufLen;
};

class CShinIOCP
{
protected:
	void				Err_Display(LPCTSTR msg);
	void				Err_Quit(LPCTSTR msg);

	bool				Init();
	void				Release();

	CShinIOCPClient&		GetEmptyClientInfo();

	bool				InitConnectSocket( CShinIOCPClient& clientInfo );							// Connect 소켓을 초기화하는 함수
	void				CloseSocket( CShinIOCPClient& clientInfo, bool bIsForce = false, bool IsEraseClientInfo = true );	// 소켓의 연결을 종료시킨다.

	bool				BindandListen( int nBindPort );											// 서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 소켓을 등록하는 함수
	bool				StartIocp();															// 접속요청을 수락하고 메세지를 받아서 처리하는 함수
	bool				CreateWorkerThread();													// WaitingThread Queue에서 대기할 쓰레드들을 생성
	bool				CreateAccepterThread();													// accept요청을 처리하는 쓰레드 생성
	
	bool				BindIOCP( CShinIOCPClient& clientInfo );									// CompletionPort객체와 소켓과 CompletionKey를 연결시키는 역할을 한다.
	bool				BindRecv( CShinIOCPClient& clientInfo );									// WSARecv

	bool				ReRecv( CShinIOCPClient& clientInfo, stOverlappedEx *pOverlappedEx );		// 재수신
	bool				ReSendMsg( CShinIOCPClient& clientInfo, stOverlappedEx *pOverlappedEx );

	void				DestroyThread();		// 생성되어 있는 쓰레드를 파괴한다.

public:
	class EnterCS{
			CRITICAL_SECTION& mCs;
		public:
			EnterCS( CRITICAL_SECTION& cs ) : mCs(cs){
				EnterCriticalSection( &mCs );
			}
			~EnterCS(){
				LeaveCriticalSection( &mCs );
			}
	};

	CShinIOCP(void);
	virtual ~CShinIOCP(void);

	enum eERROR_MODE
	{
		eERROR_MODE_NONE,
		eERROR_MODE_MESSAGEBOX,
		eERROR_MODE_OUTPUTDEBUG,
		eERROR_MODE_OUTPUTFILE,
		eERROR_MODE_USERFUNC,
	};

	void				SetErrorMode( eERROR_MODE eErrorMode )									{ m_eErrorMode = eErrorMode; }

	bool				IsBegined() const { return m_bIsBegined; }
	bool				IsAccepted() const { return m_bIsAccepted; }
	bool				IsConnectedServer();	// 서버와의 접속여부
	void				AddServer( unsigned long connectionIdx );
	void				RemoveServer( unsigned long connectionIdx );
	void				AddServerIP( LPCTSTR ip, u_short port );
	bool				IsServer( unsigned long connectionIdx );
	bool				IsServer( LPCTSTR ip, u_short port );

	unsigned long		GetClientCnt() const { return m_clientCnt; }
	CShinIOCPClient&	GetClientInfo( unsigned long connectionIdx );
	bool				IsClient( unsigned long connectionIdx );

	bool				Begin();																// 윈속 라이브러리 로드, Worker Thread 생성
	bool				BeginAccept( u_short port = 9000 );										// Listen 소켓 생성->Bind->Listen->Accept 실행
	bool				End();

	bool				ConnectServer( LPCTSTR ip, u_short port, unsigned long& connectionIdx );	// 서버에 접속
	bool				DisConnectServer( unsigned long connectionIdx );						// 서버와 접속해제
	
	bool				SendMsg( CShinIOCPClient& clientInfo, LPCTSTR pMsg, size_t nLen );		// WSASend
	bool				SendMsg( unsigned long connectionIdx, LPCTSTR pMsg, size_t nLen );	// 특정 Client에게 데이터 전송
	bool				Broadcast2Client( LPCTSTR pMsg, size_t nLen );						// 모든 Client에게 데이터 전송
	bool				Broadcast2Server( LPCTSTR pMsg, size_t nLen );
	
	void				WorkerThread();			// 작업자 쓰레드
	void				AccepterThread();		// 접속을 받는 쓰레드

	// Worker Thread 갯수 설정
	void				SetWorkerThreadCount( int threadCount );

	// 함수 포인터 설정
	void				SetErrorPrintFunc( void (*printfunc)(LPCTSTR, ...) );

	// 패킷
	bool				EmptyPacketFromServer()			{ return m_PacketQueFromServer.empty(); }
	bool				EmptyPacketFromUser()			{ return m_PacketQueFromUser.empty(); }
	const stPacketInfo	GetPacketFromServer();
	const stPacketInfo	GetPacketFromUser();
	void				PopPacketFromServer();
	void				PopPacketFromUser();


protected:
	CRITICAL_SECTION				m_CsClientInfo;
	CRITICAL_SECTION				m_CsServerInfo;
	CRITICAL_SECTION				m_CsServerPacket;
	CRITICAL_SECTION				m_CsUserPacket;
	
	CIDCreater						m_IDCreater;
	typedef int ServerPort;
	typedef std::pair< std::basic_string<TCHAR>, ServerPort > ServerIPInfo;
	std::map< DWORD, ServerIPInfo > mServerIP;													// Server IP 정보
	std::map<unsigned long, CShinIOCPClient>	m_mapClientInfo;								// Client 정보
	std::set<unsigned long>			m_setServerInfo;

	std::deque<stPacketInfo>		m_PacketQueFromServer;										// Server 패킷 Que
	std::deque<stPacketInfo>		m_PacketQueFromUser;										// User 패킷 Que

	int					m_nMaxWorkerThreadCnt;

	SOCKET				m_socketListen;															// 클라이언트의 접속을 받기위한 Listen 소켓	
	unsigned long		m_clientCnt;															// 접속되어 있는 클라이언트 수

	HANDLE*				m_hWorkerThread;														// 작업 Thread 핸들
	HANDLE				m_hAccepterThread;														// 접속 Thread 핸들	
	HANDLE				m_hPacketThread;														// 패킷처리 Thread 핸들	
	HANDLE				m_hIOCP;																// IOCP 핸들

	bool				m_bWorkerRun;															// 작업 쓰레드 동작 플래그
	bool				m_bAccepterRun;															// 접속 쓰레드 동작 플래그

	bool				m_bIsBegined;
	bool				m_bIsAccepted;

	eERROR_MODE			m_eErrorMode;

	void				(*m_fpErrorPrintFunc)(LPCTSTR, ...);
	void				ShowErrorMsg( LPCTSTR Msg );

};

#endif