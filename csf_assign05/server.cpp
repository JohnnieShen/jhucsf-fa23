#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

struct server_connection {
  int connfd;
  Server *server;
  server_connection() {}
};
pthread_attr_t attr;
////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

void chat_with_receiver(server_connection *server_conn, std::string username, Connection *conn) {
  Message loginResponse = Message(TAG_OK,"login ok");
  if (!conn->send(loginResponse)) {
    conn->close();
    return;
  }

  User *receiver = new User(username);
  Message joinMessage;
  conn->receive(joinMessage);
  if (joinMessage.tag == TAG_JOIN) {
    Room *current_room = server_conn->server->find_or_create_room(joinMessage.data);
    current_room->add_member(receiver);
    //std::cout<<"joined room "<<current_room->get_room_name()<<std::endl;
    Message joinResponse(TAG_OK,"join ok");
    conn->send(joinResponse);
  } else {
    Message joinResponse(TAG_ERR,"join error or message doesn't have tag join");
    conn->send(joinResponse);
    return;
  }
  while(true) {
    Message *message = receiver->mqueue.dequeue();
    if (message == nullptr) {
      continue;
    } else if (message->tag == TAG_ERR) {
      Message receiveMessage(TAG_ERR, "receive error");
      conn->send(receiveMessage);
      delete message;
      return;
    } else {
      //Message receiveMessage(message->tag,message->data);
      //std::cout<<"sent to receiver:"<<message->tag<<" "<<message->data<<std::endl;
      conn->send(*message);
      delete message;
    }
  }
}

void chat_with_sender(server_connection *server_conn, std::string username, Connection *conn) {
  Message loginResponse = Message(TAG_OK,"login ok");
  if (!conn->send(loginResponse)) {
    conn->close();
    return;
  }
  Room *current_room;
  User *sender = new User(username);
  bool in_room = false;
  while(true) {
    Message message;
    conn->receive(message);
    if (message.tag == TAG_JOIN) {
      if (in_room) {
        current_room->remove_member(sender);
        //std::cout<<"join left current room "<<std::endl;
      }
      current_room = server_conn->server->find_or_create_room(message.data);
      //std::cout<<"joined room "<<current_room->get_room_name()<<std::endl;
      current_room->add_member(sender);
      Message joinResponse(TAG_OK,"join ok");
      conn->send(joinResponse);
      in_room = true;
    } else if (message.tag == TAG_LEAVE) {
      if (!in_room) {
        Message leaveReponse(TAG_ERR, "currently not in room");
        conn->send(leaveReponse);
      } else {
        current_room->remove_member(sender);
        current_room = nullptr;
        Message leaveReponse(TAG_OK, "left ok");
        conn->send(leaveReponse);
        in_room = false;
      }
    } else if (message.tag == TAG_QUIT) {
      Message quitReponse(TAG_OK, "quit ok");
      conn->send(quitReponse);
      return;
    } else if (message.tag == TAG_SENDALL){
      if (!in_room) {
        Message leaveReponse(TAG_ERR, "currently not in room");
        conn->send(leaveReponse);
      } else {
        current_room->broadcast_message(username, message.data);
        Message messageReponse(TAG_OK, "message ok");
        conn->send(messageReponse);
      }
    } else {
      Message messageReponse(TAG_ERR, "undefined command");
      conn->send(messageReponse);
    }
  }
}

namespace {

void *worker(void *arg) {
  pthread_detach(pthread_self());
  
  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  
  struct server_connection *server_conn = static_cast<server_connection*>(arg);

  Connection conn(server_conn->connfd);
  Message loginMessage;
  if (!conn.receive(loginMessage)) {
    return nullptr;
  }
  std::string username = loginMessage.data;
  Message loginResponse;
  if (loginMessage.tag == TAG_RLOGIN) {
    //std::cout<<"receiver: "<<loginMessage.data<<std::endl;
    chat_with_receiver(server_conn,username, &conn);
  } else if (loginMessage.tag == TAG_SLOGIN) {
    //std::cout<<"sender: "<<loginMessage.data<<std::endl;
    chat_with_sender(server_conn,username, &conn);
  } else {
    loginResponse = Message(TAG_ERR,"need login");
    if (!conn.send(loginResponse)) {
      return nullptr;
    }
    
    // return nullptr;
  }

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)

  // for(delete_room = m_rooms.begin();delete_room!=m_rooms.end();delete_room++) {
  //   delete delete_room->second;
  // }
  conn.close();
  delete server_conn;
  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // TODO: destroy mutex
  pthread_mutex_lock(&m_lock);
  std::map<std::string, Room *>::iterator delete_room;
  for(delete_room = m_rooms.begin();delete_room!=m_rooms.end();delete_room++) {
    delete delete_room->second;
  }
  pthread_mutex_unlock(&m_lock);
  pthread_mutex_destroy(&m_lock);
  // close(m_ssock);
  // close(m_port);
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  this->m_ssock = open_listenfd(std::to_string(this->m_port).c_str());
  return this->m_ssock >= 0;
}

void Server::handle_client_requests() {
  //Guard guard (m_lock);
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  // pthread_attr_init(&attr);
  // pthread_attr_setdetachstate(&attr, 1);
  while(true) {
    int connfd = Accept(m_ssock,nullptr,nullptr);
    if (connfd<0) {
      return;
    }
    pthread_t new_thread;
    struct server_connection *server_conn= new server_connection();
    server_conn->connfd = connfd;
    server_conn->server = this;
    if (pthread_create(&new_thread,&attr,worker,server_conn) !=  0) {
      std::cerr<<"pthread failed"<<std::endl;
      delete server_conn;
      continue;
    }
    //delete server_conn;
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  Guard guard(m_lock);
  std::map<std::string, Room *>::iterator current_room = m_rooms.find(room_name);
  if (current_room == m_rooms.end()) {
    Room *new_room = new Room(room_name);
    m_rooms.insert(std::pair<std::string, Room *> (room_name,new_room));
    return new_room;
  } else {
    return current_room->second;
  }
}

