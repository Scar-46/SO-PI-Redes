/** 
 * @brief class to encapsulate Unix semaphore intrinsic structures and system calls
 * @author Operating systems (Francisco Arroyo)
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @version 2023/Mar/15
 * 
 * @copyright Copyright (c) 2023
 * 
 * Ref.: https://en.wikipedia.org/wiki/Semaphore_(programming)
 *
 **/

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

class Semaphore {
 public:
  /**
   * @brief Construct a new Semaphore object
   * @param key  Key to create semaphore
   * @param initialValue Initial value to set semaphore
   */
  Semaphore(int key, int initialValue);

  /**
   * @brief Destroy the Semaphore object
   */
  ~Semaphore();

  /**
   * @brief Signal method
   * 
   * @return int  0 if success, -1 if error
   */
  int Signal();  // Is the same as V method in Dijkstra definition

  /**
   * @brief Wait method
   * 
   * @return int  0 if success, -1 if error
   */
  int Wait();  // Is the same as P method in Dijkstra definition

 private:
  /// @brief Semaphore indentifier
  int id;
};

/**
 * @brief Contains the semaphores used by the program
 * @struct sem_a_t
 */
typedef struct semaphores {
  /// @brief semaphore to control the player turn
  Semaphore* canPlay;
}sem_a_t;

#endif  // SEMAPHORE_H
