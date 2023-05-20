/**
 * @file main.cpp
 * @brief Main file for the baboons crossing the rope problem
 * @author Oscar Fernández jiménez <oscar.fernandezjimenez>
 * @version 0.1
 * @date 2023-05-20
 * 
 * @copyright Copyright (c) 2023
*/

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>


#include "Monitor.h"

#define ROPE_COUNT 4
#define SAFE_NUMBER_OF_BABOONS 4

int main(int argc, char const *argv[]) {
    Monitor monitor(ROPE_COUNT,SAFE_NUMBER_OF_BABOONS);
    std::vector<std::thread> threads;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0,ROPE_COUNT-1);
    int ropeNumber;
    for(int i = 0; i < 20; i++) {
        ropeNumber = dis(gen);
        threads.push_back(std::thread(&Monitor::crossRope,&monitor,ropeNumber));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    for(int i = 0; i < 20; i++) {
        threads[i].join();
    }
    return 0;
}