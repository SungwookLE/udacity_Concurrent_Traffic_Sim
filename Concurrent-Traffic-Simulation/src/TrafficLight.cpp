#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

    // (8/10) DONE: monitor pattern(?) condition variable used!
    std::unique_lock<std::mutex> _ulock(_mutex);
    _condition.wait(_ulock, [this] { return !_queue.empty();});
    T v = std::move(_queue.back());
    _queue.pop_back();

    return v;
}


template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    
    // (8/10) DONE: monitor pattern(?) condition variable used!
    // simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<std::mutex> _ulock(_mutex);
    // add vector to queue
    _queue.push_back(std::move(msg));
    _condition.notify_one(); // notify client after pushing new Vehicle into vector
        
}



/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}


void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.

    // (8/12) DONE: 
    while(true){
        _currentPhase=_messageQueue.receive();

        if (_currentPhase == TrafficLightPhase::green)
        {
            break;
        }
    }

    return;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread
    // when the public method „simulate“ is called. To do this, use the thread queue in the base class. 

    // (8/10) DONE: thread
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread

void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

    // (8/10) DONE: 

    double cycle_duration = rand() % 3 + 4;
    
    std::chrono::system_clock::time_point Start= std::chrono::system_clock::now();
    std::chrono::system_clock::time_point End= std::chrono::system_clock::now();
    std::chrono::duration<double> sec = (End-Start);
    
    while (true)
    {
        
        if ( sec.count() > cycle_duration ) {
            if (_currentPhase == TrafficLightPhase::red){ 
                _currentPhase=TrafficLightPhase::green;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                Start = std::chrono::system_clock::now();
            }
            else{
                _currentPhase=TrafficLightPhase::red;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                Start = std::chrono::system_clock::now();
            }
        }
        
        End = std::chrono::system_clock::now();
        sec = (End-Start);
        _messageQueue.send(std::move(_currentPhase));
    }
}

