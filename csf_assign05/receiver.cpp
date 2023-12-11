#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

bool still_running;

void sigintHandler(int sig) {
  still_running = false;
}

std::string getSubstringAfterColon(const std::string& input) {
    size_t colonPos = input.find(':');
    return input.substr(colonPos + 1);
}

int main(int argc, char **argv) {
  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, sigintHandler);
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }
  Connection *conn = new Connection();
  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];


  // TODO: connect to server
  conn->connect(server_hostname, server_port);
  // TODO: send rlogin and join messages (expect a response from
  //       the server for each one)
  Message login_msg(TAG_RLOGIN, username);
  //conn->send(login_msg);
  if (!conn->send(login_msg)) {
    conn->close();
    delete(conn);
    return 1;
  }

  Message response;
  if (!conn->receive(response)) {
    conn->close();
    delete(conn);
    return 1;
  }

  Message join_msg(TAG_JOIN, room_name);
  //conn->send(join_msg);
  if (!conn->send(join_msg)) {
    conn->close();
    delete(conn);
    return 1;
  }

  if (!conn->receive(response)) {
    conn->close();
    delete(conn);
    return 1;
  }
  still_running = true;
  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  while (still_running) {
    Message incoming_msg;
    if (conn->receive(incoming_msg)) {
      if (incoming_msg.tag == TAG_DELIVERY) {
        int colon_position = incoming_msg.data.find(":");
        std::string data = incoming_msg.data.substr(colon_position+1);
        colon_position = data.find(":");
        std::string username = data.substr(0, colon_position);
        std::string message = data.substr(colon_position+1);
        std::cout << username << ": " << message << std::endl;
      } 
    }
  }
  
  conn->close();
  delete(conn);
  return 0;
}
