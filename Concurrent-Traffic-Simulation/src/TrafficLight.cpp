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
    // (8/12) debugging : it works ( pop_back() --> clear() )
    _queue.clear();

    return v;
}


template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    
    // (8/10) DONE: monitor pattern(?) condition variable used!
    
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
        if ( _currentPhase == TrafficLightPhase::green) {
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

    // (8/13) CODE REVIEW (REQUIRED)    
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(4000, 6000);
    int cycle_duration = distr(eng); //Duration of a single simulation cycle in seconds, is randomly chosen
    std::chrono::system_clock::time_point Start = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point End = std::chrono::system_clock::now();
    std::chrono::milliseconds msec = std::chrono::duration_cast<std::chrono::milliseconds>(End - Start);
    
    while (true)
    {
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (msec.count() > cycle_duration)
        {
            //std::cout <<"      duration: "<< cycle_duration << std::endl;
            if (_currentPhase == TrafficLightPhase::red)
            {
                _currentPhase = TrafficLightPhase::green;
                Start = std::chrono::system_clock::now();
            }
            else
            {
                _currentPhase=TrafficLightPhase::red;
                Start = std::chrono::system_clock::now();
            }
        }

        /* (8/13) CODE REVIEW: Randomly choose the cycle duration for the next cycle */
        cycle_duration = distr(eng);
       
        End = std::chrono::system_clock::now();
        msec = std::chrono::duration_cast<std::chrono::milliseconds>(End - Start);
        _messageQueue.send(std::move(_currentPhase));
    }
}