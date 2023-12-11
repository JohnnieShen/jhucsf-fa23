#include <sstream>
#include <cctype>
#include <cassert>
#include <iostream>
#include "csapp.h"
#include "message.h"
#include "connection.h"

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object
  Rio_readinitb(&m_fdbuf, fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  // TODO: call rio_readinitb to initialize the rio_t object
  std::string port_str = std::to_string(port);

  // Open client file descriptor
  m_fd = open_clientfd(hostname.c_str(), port_str.c_str());
  if (m_fd < 0) {
    std::cerr<<"connection failed"<<std::endl;
    close();
    exit(1);
  } else {
    m_last_result = SUCCESS;
    Rio_readinitb(&m_fdbuf, m_fd);
  }
}

Connection::~Connection() {
  // TODO: close the socket if it is open
  if (m_fd >= 0) {
    Close(m_fd);
  }
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
  return m_fd >= 0;
}

void Connection::close() {
  // TODO: close the connection if it is open
  if (is_open()) {
    Close(m_fd);
    m_fd = -1;
  }
}

bool Connection::send(const Message &msg) {
  // TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  std::string msg_string = msg.tag + ":" + msg.data + "\n";
  ssize_t bytes_written = rio_writen(m_fd, msg_string.c_str(), msg_string.length());  
  if (bytes_written != (ssize_t)msg_string.length()) {
    m_last_result = EOF_OR_ERROR;
    //std::cerr<<"Send failed"<<std::endl;
    return false;
  }

  m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
  char buffer[Message::MAX_LEN + 1];

  ssize_t bytes_read = Rio_readlineb(&m_fdbuf, buffer, Message::MAX_LEN);

  if (bytes_read <= 0) {
    m_last_result = EOF_OR_ERROR;
    std::cerr<<"Receive failed"<<std::endl;
    return false;
  }

  buffer[bytes_read] = '\0';

  
  if (msg.from_string(buffer) == -1) {
    m_last_result = INVALID_MSG;
    std::cerr<<buffer<<std::endl;
    return false;
  } else if (msg.tag == TAG_ERR) {
    m_last_result = EOF_OR_ERROR;
    std::cerr<<msg.data<<std::endl;
    return false;
  }

  m_last_result = SUCCESS;
  return true;
}
