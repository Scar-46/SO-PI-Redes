/**
 * @file Monitor.h
 * @brief Monitor class for threads synchronization
 * @author Oscar Fernández jiménez <oscar.fernandezjimenez>
 * @version 0.1
 * @date 2023-05-20
 * 
 * @copyright Copyright (c) 2023
*/

#ifndef MONITOR_H
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <vector>

class Monitor {
    private:
        std::mutex mtx;
        std::vector<std::condition_variable> rope;
        std::vector<int> baboonsPerRope;
        std::vector<bool> ropeInUse;  //Only for closing the program
        int ropeCount;
        int safeNumberOfBaboons;
        int totalBaboons;
        int baboonsCrossed;

    public:
        /**
         * @brief Construct a new Monitor object
         * @param ropeCount Number of ropes
         * @param safeNumberOfBaboons Number of baboons that should be in order to cross the rope
         * @param totalBaboons Total number of baboons
        */
        Monitor(int ropeCount,int safeNumberOfBaboons);

        /**
         * @brief Destroy the Monitor object
        */
        ~Monitor();

        /**
         * @brief Ables several baboons to cross the rope if the conditions are met
         * @param ropeNumber Number of the rope
        */
        void crossRope(int ropeNumber);
};

#endif // MONITOR_H
