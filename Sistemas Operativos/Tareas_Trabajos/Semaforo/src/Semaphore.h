/**
 * @file Semaphore.h
 * @author Oscar Fernández Jiménez
 * @brief  Encapsulates a semaphore in a class for easy use
 * @version 0.1
 * @date 2023-03-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef SHEMAPHORE_H
#define SHEMAPHORE_H

class Semaphore
{
public:
  Semaphore(int count = 0);
  ~Semaphore();
  void Wait();
  void Signal();
private:
  int semid;
};
#endif
