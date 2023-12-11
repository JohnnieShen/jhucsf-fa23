#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

bool still_running;

void sigintHandler(int sig) {
  still_running = false;
}

int main(int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigintHandler);
    Connection *connection = new Connection();
    if (argc != 4) {
        std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
        return 1;
    }

    std::string server_hostname;
    int server_port;
    std::string username;

    server_hostname = argv[1];
    server_port = std::stoi(argv[2]);
    username = argv[3];



    // TODO: connect to server

    connection->connect(server_hostname, server_port);

    // TODO: send slogin message
    Message sendSlogInMessage(TAG_SLOGIN, username);
    connection->send(sendSlogInMessage);


    Message receiveOkStatus;
    bool status;
    status = connection->receive(receiveOkStatus);
    if (status == false) {
        connection->close();
        delete(connection);
        return 1;
    }
    else if (status ==  true && receiveOkStatus.tag != TAG_OK){
        std::cerr << receiveOkStatus.data<<std::endl;
        connection->close();
        delete(connection);
        return 1;
    }


    // TODO: loop reading commands from user, sending messages to
    //       server as appropriate

    std::string currentRoomName;
    still_running = true;
    Message* msg;
    while(still_running){
        std::string inputLine;
        std::getline(std::cin, inputLine);
        
        std::istringstream iss(inputLine);
        std::string firstMessage;
        iss >> firstMessage;

        if(firstMessage == "/join"){
            iss >> currentRoomName;
            msg = new Message(TAG_JOIN, currentRoomName);
        }
        else if(firstMessage == "/leave"){
            msg = new Message(TAG_LEAVE, "bye");
        }
        else if(firstMessage == "/quit"){
            Message quit_msg(TAG_QUIT, "bye");
            connection->send(quit_msg);

            Message quitStatus;
            connection->receive(quitStatus);
            connection->close();
            delete(connection);
            delete(msg);
            return 0;
        } else if (firstMessage.find('/') != std::string::npos) {
            std::cerr<<("undefined command")<<std::endl;
            continue;
        }
        else{
            msg = new Message(TAG_SENDALL, inputLine);
            
        }
        connection->send(*msg);

        status = connection->receive(receiveOkStatus);
        if(status == true && receiveOkStatus.tag != TAG_OK){
            std::cerr << receiveOkStatus.data<<std::endl;
        }

    }
    connection->close();
    delete(connection);
    delete(msg);
    return 0;
}