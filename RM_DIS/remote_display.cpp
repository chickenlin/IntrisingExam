#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
using namespace std;

/*
 *	Source:	[stackoverflow]Sending and receiving std::string over socket
 *  URL:	https://stackoverflow.com/questions/18670807/sending-and-receiving-stdstring-over-socket
 */
string RxStr(int fd)
{
	// create the buffer with space for the data
	const unsigned int MAX_BUF_LENGTH = 4096;
	vector<char> buffer(MAX_BUF_LENGTH);
	string rcv;   
	int bytesReceived = 0;
	do {
		bytesReceived = recv(fd, &buffer[0], buffer.size(), 0);
		// append string from buffer.
		if ( bytesReceived == -1 ) { 
		    cerr << "Receive error\n" << endl;
		    break;
		} else {
		    rcv.append( buffer.cbegin(), buffer.cend() );
		}
	} while ( bytesReceived == MAX_BUF_LENGTH );
	return bytesReceived == 0 ? "" : rcv;
}

int main()
{
    // 創建 socket
    int remote_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (remote_fd == -1)
    {
        cerr << "socket creation failed" << endl;
        return -1;
    }

    // 綁定 socket 到指定 IP 和 port
    struct sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(remote_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "bind failed" << endl;
        return -1;
    }

    // 監聽來自 client 的連線請求
    if (listen(remote_fd, 1) < 0)
    {
        cerr << "listen failed" << endl;
        return -1;
    }

	cout << "Remote server is waiting for elevator to connection..." << endl;

    // 接受 elevator 的連線請求
    int elevator_fd = accept(remote_fd, nullptr, nullptr);
    if (elevator_fd < 0)
    {
        cerr << "accept failed" << endl;
        return -1;
    }

    cout << "Elevator connected" << endl;

	// 接收 client 傳送過來的 string
	string RxMessage;
	while (1)
	{
		RxMessage = RxStr(elevator_fd);
		cout << RxMessage << endl;
		if (RxMessage.compare("") == 0)
		{
			break;
		}
	}

	cout << "End of process" << endl;

    // 關閉連線
    close(elevator_fd);
    close(remote_fd);

    return 0;
}

