#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"
#include <chrono>


// forward declarations to avoid include cycle
class Vehicle;

// (8/6) FP.1 DONE: rafficLightPhase is an enum that can be either „red“ or „green“.
enum TrafficLightPhase {red = 0, green};

// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:

    // (8/10) DONE: 
    void send(T &&msg);
    T receive();
    std::deque<TrafficLightPhase> _queue;
   

private:
    // (8/10) DONE: make infinite loop using condition_variable 
    std::condition_variable _condition;
    std::mutex _mutex;
    
};

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
// The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor

    // getters / setters

    // typical behaviour methods

    // (8/6) FP.1 DONE: public methods ,,void waitForGreen() and ,,void simulate()
    void waitForGreen();
    void simulate();

    // (8/6) FP.1 DONE: „TrafficLightPhase getCurrentPhase()
    TrafficLightPhase getCurrentPhase();
    

private:
   
         
     // typical behaviour  metho ds

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.

    std::condition_variable _condition;
    std::mutex _mutex;
    
    // (8/6) FP.1 DONE: private member _currentPhase, cycleThroughPhase()
    TrafficLightPhase _currentPhase;
    void cycleThroughPhases();
    MessageQueue<TrafficLightPhase> _messageQueue;
    
};

#endif 
           
 
         
          
 
       