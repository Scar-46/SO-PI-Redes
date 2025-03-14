/**
 * @file Monitor.h
 * @brief Monitor class for threads synchronization
 * @author Oscar Fernández jiménez <oscar.fernandezjimenez>
*/

#include "Monitor.h"

Monitor::Monitor(int ropeCount,int safeNumberOfBaboons) {
    this->ropeCount = ropeCount;
    this->safeNumberOfBaboons = safeNumberOfBaboons;
    this->totalBaboons = 0;
    this->baboonsCrossed = 0;
    this->baboonsPerRope = std::vector<int>(ropeCount,0);
    this->rope = std::vector<std::condition_variable>(ropeCount);
    this->ropeInUse = std::vector<bool>(ropeCount,false);
}

Monitor::~Monitor() {
    this->rope.clear();
    this->baboonsPerRope.clear();
}

void Monitor::crossRope(int ropeNumber) {
    std::unique_lock<std::mutex> lck(this->mtx);
    this->baboonsPerRope[ropeNumber]++;
    this->totalBaboons++;
    int maxBaboons = 0;
    int maxRope = 0;
    if(this->totalBaboons >= this->safeNumberOfBaboons) {
        for(int i = 0; i < this->ropeCount; i++) {
            if(this->baboonsPerRope[i] > maxBaboons) {
                maxBaboons = this->baboonsPerRope[i];
                maxRope = i;
            }
        }
        std::cout << "* Total baboons: " << this->totalBaboons <<" *"<< std::endl;
        std::cout << "Baboons are going to cross rope " << maxRope << std::endl;
        std::cout << "Baboons in rope " << maxRope << ": " << maxBaboons << std::endl;
        baboonsCrossed = maxBaboons;
        this->ropeInUse[maxRope] = true;
        this->rope[maxRope].notify_one();
    }
    this->rope[ropeNumber].wait_for(lck,std::chrono::milliseconds(10000));
    if (this->ropeInUse[ropeNumber]) {  // This is for closing the program only
        std::cout << "--> Baboon crossed rope " << ropeNumber << std::endl;
        baboonsCrossed--;
        this->baboonsPerRope[ropeNumber]--;
        this->totalBaboons--;
        if (baboonsCrossed > 0) {
            this->rope[ropeNumber].notify_one();
        } else {
            std::cout << "All baboons crossed rope " << ropeNumber << std::endl;
            this->ropeInUse[ropeNumber] = false;
        }
    }

}