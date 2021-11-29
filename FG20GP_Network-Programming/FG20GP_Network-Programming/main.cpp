#include <stdio.h>
#include <WinSock2.h>
#include <windows.h>

DWORD client_loop( void* ptr )
{
	SOCKET client = reinterpret_cast<SOCKET>(ptr);

	char username[512];
	int name_size = recv( client, username, 512, 0 );
	printf( "User %.*s has connected\n", name_size, username );

	while ( true )
	{
		char buffer[1024];
		int recv_size = recv( client, buffer, 1024, 0 );
		if ( recv_size == -1 )
		{
			printf( "User %.*s has disconnected!\n", name_size, username );
			return 0;
		}

		printf( "%.*s: ", name_size, username );
		printf( "%.*s\n", recv_size, buffer );
	}
	return 0;
}

int server_main()
{
	printf_s( "port: " );
	
	unsigned short port;
	scanf_s( "%hu", &port );

	SOCKET listen_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	sockaddr_in bind_addr;
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = INADDR_ANY;
	bind_addr.sin_port = htons( port );

	int bind_result = bind( listen_sock, (sockaddr*)&bind_addr, sizeof( bind_addr ) );
	if ( bind_result )
	{
		printf( "bind() failed: %d\n", WSAGetLastError() );
		return 1;
	}

	listen( listen_sock, 5 );

	while ( true )
	{
		SOCKET client = accept( listen_sock, NULL, NULL );
		CreateThread(
			nullptr,
			0,
			client_loop,
			(void*)client,
			0,
			nullptr
		);
	}
}

int client_main()
{
	SOCKET sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( sock == INVALID_SOCKET )
	{
		printf( "socket() failed! whoopsies..." );
		return 1;
	}

	printf_s( "port: " );

	unsigned short port;
	scanf_s( "%hu", &port );

	// unsigned char ip_bytes[] = { 46, 39, 122, 74 };
	unsigned char ip_bytes[] = { 127, 0, 0, 1 };
	unsigned int ip = *(unsigned int*)ip_bytes;

	sockaddr_in conn_addr;
	conn_addr.sin_family = AF_INET;
	conn_addr.sin_addr.s_addr = ip;
	// conn_addr.sin_port = htons( 35001 );
	conn_addr.sin_port = htons( port );

	int connect_result = connect( sock, (sockaddr*)&conn_addr, sizeof( conn_addr ) );
	if ( connect_result )
	{
		printf( "connect() failed: %d\n", WSAGetLastError() );
		return 1;
	}

	printf_s( "enter username: " );

	while ( true )
	{
		char send_buffer[1024];
		gets_s( send_buffer, 1024 );

		send( sock, send_buffer, strlen(send_buffer), 0 );
	}


	/*static const char* my_message = "If you're wonder who said that, it was me... Smaug. tihi";
	send( sock, my_message, strlen( my_message ), 0 );

	char buffer[1024];
	int recv_size = recv( sock, buffer, 1024, 0 );
	buffer[recv_size] = 0;

	printf( "I received: '%s'!\n", buffer );*/
	return 0;
}

int main()
{
	int ec = 0;
	WSAData wsa_data;
	WSAStartup( MAKEWORD( 2, 2 ), &wsa_data );

	printf( "What mode? [c]lient : [s]erver\n" );

	char selector[5];
	scanf_s( "%s", selector, 5 );

	if ( selector[0] == 'c' )
	{
		ec = client_main();
	}
	else if ( selector[0] == 's' )
	{
		ec = server_main();
	}
	
	return ec;
}