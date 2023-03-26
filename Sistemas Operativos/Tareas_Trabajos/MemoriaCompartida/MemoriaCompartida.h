/**
 * @file MemoriaCompartida.h
 * @author Oscar Fernández Jiménez - C12840
 * @brief 
 * @version 0.1
 * @date 2023-03-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef MEMORIACOMPARTIDA_H
#define MEMORIACOMPARTIDA_H

class MemoriaCompartida {
  private:
    int id;
    void* ptrDatos;
  public:
    MemoriaCompartida();
    ~MemoriaCompartida();
    int crear(const char *archivo, char letra);
    int get();
    void* getPtrDatos();
    void liberar();
};

#endif
