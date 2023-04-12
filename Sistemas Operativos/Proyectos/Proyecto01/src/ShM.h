/** 
 * @file ShM.h
 * @brief class to encapsulate Unix shared memory intrinsic structures and system calls
 * 
 * @author Operating systems (Francisco Arroyo)
 * @author Oscar Fernández Jiménez - oscar.fernandezjimenez@ucr.ac.cr
 * @version 2023/Mar/15
 * 
 * @copyright Copyright (c) 2023
 *
 * Ref.: https://en.wikipedia.org/wiki/Shared_memory
 *
 **/
# ifndef SHM_H
# define SHM_H

/**
 * @brief Contains the message used by the program
 * @struct msg_t
 */
typedef struct public_msg {
  /// @brief potato's value
  int potato;
  /// @brief number of players
  int playerCount;
  /// @brief number of players out
  int playersOut;
}msg_t;

class ShM {
 public:
  /**
   * @brief Construct a new ShM object
   * 
   * @param size  Size of shared memory area
   */
  ShM(int size);

  /**
   * @brief Destroy the ShM object
   * 
   */
  ~ShM();

  /**
   * @brief Attach method
   * 
   * @return void*  Pointer to shared memory area
   */
  void * attach();

  /**
   * @brief Detach method
   * 
   * @return int  0 if success, -1 if error
   */
  int detach();

 private:
  /// @brief Shared memory indentifier
  int id;
  /// @brief Pointer to shared memory area
  void * area;
};

# endif  // SHM_H
