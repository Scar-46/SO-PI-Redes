/**
 * @file MailBox.h
 * @author Operating systems (Francisco Arroyo)
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @brief C++ class to encapsulate Unix shared memory intrinsic structures and system calls
 * @version 0.1
 * @date 2023-04-11
 * Ref.: https://en.wikipedia.org/wiki/Shared_memory
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MAILBOX_H
#define MAILBOX_H
/**
 * @brief struct message 
 * @struct msg_t
 */
typedef struct message {
  /// @brief message type
  long mtype;
  /// @brief message data (array of integers)
  int mnums[2];
}msg_t;

class MailBox {
 public:
  /**
   * @brief Construct a new Mail Box object
   * 
   */
  MailBox();

  /**
   * @brief Destroy the Mail Box object
   * 
   */
  ~MailBox();

  /**
   * @brief Send a message
   * 
   * @param type message type
   * @param buffer message data
   * @param numBytes message size
   * @return int  0 if success, -1 if error
   */
  int send( long, void * buffer, int capacity );

  /**
   * @brief Receive a message
   * 
   * @param type message type
   * @param buffer message data
   * @param capacity message size
   * @return int 0 if success, -1 if error
   */
  int receive( long, void * buffer, int capacity );

 private:
  /// @brief message queue id
  int id;

};

#endif
