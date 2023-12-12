Sample README.txt

Thread synchronization approach
1. Server.cpp
Inside the server.cpp file, the server is used to communicate with multiple clients at the same time. As a result, most
server client communication operations would require thread synchronization:

Server's find or create room operation:
Server's find or create room operation first tries to find if a room exists with the given room name. If it does, it
would return the pointer of the room through the m_room map in the server. If it is not found in the server, it would
create a new room and adding one to the m_room map in the server. The process requires synchronization. The m_room map
is a Map that stores all the rooms that are used to communicate with clients in the server. Let's think of a case where
two threads call the operation at the same time. They will both access this common map resource at the same time. As a
result, we will have to use mutex to create synchronization. Our planned approach is using guard as there are return
return statements inside the critical sections, and manual mutex lock & unlock may create errors where the program
returned before unlocking the mutex.


2. Room.cpp
The room struct is a class that keeps tracks of all the receivers in the chat room. As a result, the operations in the
class may be called from multiple different threads if they are planning to all modify the receivers at the same time.
For instance, the room stores all the receiver clients in a set called members; when multiple threads adds/removes
elements in member class at the same time, we would again need thread synchronization to prevent errors. As mutex guard
is the safest approach to achieve synchronization, we used mutex guard for all operations inside the room.cpp.

3. message_queue.cpp
The message queue struct is one that represents a queue of Messages waiting to be delivered to the receiver. Similar to
room.cpp, the queue is message_queue.cpp is a data structure that can be and will be accessed by all the threads. The
queue stores all the messages that are planned on being sent to the receiver clients. As a result, both the enqueue and
dequeue operations are the critical sections of the struct. We again achieved thread synchronization through have mutex
guard on enqueue and dequeue.

The mutex alone is not sufficient because while it can ensure that the queue is accessed by only one thread at a time,
it does not have a way to efficiently manage the availability of messages in the queue. Without the semaphore, consumer
threads might have to continuously check (busy-wait) if there are messages in the queue, which is highly inefficient.


