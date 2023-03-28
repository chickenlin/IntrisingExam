#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>

#include <thread>
#include <chrono>

using namespace std;
using std::this_thread::sleep_for;

/*
 *	Description: To receive a string from a socket
 *	Source:		**stackoverflow** Sending and receiving std::string over socket
 *  URL:		https://stackoverflow.com/questions/18670807/sending-and-receiving-stdstring-over-socket
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
void* commandThread(void *arg)
{
    sleep_for(std::chrono::seconds(5));
    
	cout << "Command channel: create socket" << endl;
	// create socket
	int command_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (command_fd == -1)
	{
		cerr << "Command channel: socket creation failed" << endl;
    	pthread_detach(pthread_self());
	}
	
	// connect to remote
	struct sockaddr_in server_addr {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("140.115.52.84");
	server_addr.sin_port = htons(8080);
	
	if (connect(command_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		cerr << "Command channel: connection failed" << endl;
    	pthread_detach(pthread_self());
	}
	
	cout << "Command channel: connect to remote successfully" << endl;

	int current, floor;
	while (1)
	{
		cout << "Command channel: Input your current floor and your destination floor" << endl;
		cout << "Command channel: If you want to end of control, enter `0 0`" << endl;
		cin >> current >> floor;
		
		if (current == 0 && floor == 0)
			break;
		
		if (send(command_fd, &current, sizeof(current), 0) < 0)
		{
			cerr << "Command channel: Error: sending data failed" << endl;
		}
		if (send(command_fd, &floor, sizeof(current), 0) < 0)
		{
			cerr << "Command channel: Error: sending data failed" << endl;
		}
	}
	cout << "Command channel: End of command" << endl;
	return nullptr;
}

int main()
{
	pthread_t command;
	pthread_create(&command, NULL, commandThread, NULL);

    // create socket
    int remote_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (remote_fd == -1)
    {
        cerr << "Display channel: socket creation failed" << endl;
        return -1;
    }

    // 綁定 socket 到指定 IP 和 port
    struct sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(remote_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cerr << "Display channel: bind failed" << endl;
        return -1;
    }

    // 監聽來自 client 的連線請求
    if (listen(remote_fd, 1) < 0)
    {
        cerr << "Display channel: listen failed" << endl;
        return -1;
    }

	cout << "Display channel: Remote server is waiting for elevator to connection..." << endl;

    // 接受 elevator 的連線請求
    int elevator_fd = accept(remote_fd, nullptr, nullptr);
    if (elevator_fd < 0)
    {
        cerr << "Display channel: accept failed" << endl;
        return -1;
    }

    cout << "Display channel: Elevator connected" << endl;

	// 接收 client 傳送過來的 string
	string RxMessage;
	while (1)
	{
		RxMessage = RxStr(elevator_fd);
		cout << "Display channel:" << RxMessage << endl;
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

