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

class ShM {
 public:
  /**
   * @brief Construct a new ShM object
   * 
   * @param int  Size of shared memory segment
   */
  ShM(int = 0);  // Parameter represent segment size

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
