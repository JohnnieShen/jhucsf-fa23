#include <iostream>
#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  pthread_mutex_init(&lock, NULL);
}

Room::~Room() {
  // TODO: destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // TODO: add User to the room
  Guard guard(lock);
  members.insert(user);
}

void Room::remove_member(User *user) {
  // TODO: remove User from the room
  Guard guard(lock);
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // TODO: send a message to every (receiver) User in the room
  Guard guard(lock);
  std::string roomname = get_room_name();
  std::string message;
  message = roomname+":"+sender_username+":"+message_text;
  for (User *current_user:members) {
    if (current_user->username != sender_username) {
      Message broadcast_message = Message(TAG_DELIVERY,message);
      //std::cout<<broadcast_message.data<<std::endl;
      current_user->mqueue.enqueue(&broadcast_message);
    }
  }
}

