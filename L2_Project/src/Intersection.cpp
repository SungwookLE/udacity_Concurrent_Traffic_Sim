#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <random>

#include "Street.h"
#include "Intersection.h"
#include "Vehicle.h"

/* Implementation of class "WaitingVehicles" */

int WaitingVehicles::getSize()
{
    return _vehicles.size();
}

void WaitingVehicles::pushBack(std::shared_ptr<Vehicle> vehicle, std::promise<void> &&promise)
{
    _vehicles.push_back(vehicle);
    _promises.push_back(std::move(promise));
}

void WaitingVehicles::permitEntryToFirstInQueue()
{
    // L2.3 : First, get the entries from the front of _promises and _vehicles. 
    // Then, fulfill promise and send signal back that permission to enter has been granted.
    // Finally, remove the front elements from both queues.

    // (8/3) DONE:
    // get entries from the front of both queues
    auto Veh_begin = _vehicles.begin();
    auto Prom_begin = _promises.begin();

    // fulfill promise and send signal back that permission to enter has been granted
    Prom_begin->get_future();

    // remove the begin element from both queue
    _vehicles.erase(Veh_begin);
    _promises.erase(Prom_begin);
    
}

/* Implementation of class "Intersection" */

Intersection::Intersection()
{
    _type = ObjectType::objectIntersection;
    _isBlocked = false;
}

void Intersection::addStreet(std::shared_ptr<Street> street)
{
    _streets.push_back(street);
}

std::vector<std::shared_ptr<Street>> Intersection::queryStreets(std::shared_ptr<Street> incoming)
{
    // store all outgoing streets in a vector ...
    std::vector<std::shared_ptr<Street>> outgoings;
    for (auto it : _streets)
    {
        if (incoming->getID() != it->getID()) // ... except the street making the inquiry
        {
            outgoings.push_back(it);
        }
    }

    return outgoings;
}

// adds a new vehicle to the queue and returns once the vehicle is allowed to enter
void Intersection::addVehicleToQueue(std::shared_ptr<Vehicle> vehicle)
{
    std::cout << "Intersection #" << _id << "::addVehicleToQueue: thread id = " << std::this_thread::get_id() << std::endl;

    // L2.2 : First, add the new vehicle to the waiting line by creating a promise, a corresponding future and then adding both to _waitingVehicles. 
    // Then, wait until the vehicle has been granted entry.

    // (8/3) DONE: (READ FOLLOW THREE LINE)
    std::promise<void> prms_new_vehicle_allowed_enter;
    // In the futre, void return will be granted through the promise
    std::future<void> ftrs_new_vehicle_allowed_enter = prms_new_vehicle_allowed_enter.get_future();
    // member pushBack(), the promise is taken
    _waitingVehicles.pushBack(vehicle, std::move(prms_new_vehicle_allowed_enter));
    // wait until the member pushBack() is finished
    ftrs_new_vehicle_allowed_enter.wait();

    std::cout << "Intersection #" << _id << ": Vehicle #" << vehicle->getID() << " is granted entry." << std::endl;
}

void Intersection::vehicleHasLeft(std::shared_ptr<Vehicle> vehicle)
{
    //std::cout << "Intersection #" << _id << ": Vehicle #" << vehicle->getID() << " has left." << std::endl;

    // unblock queue processing
    this->setIsBlocked(false);
}

void Intersection::setIsBlocked(bool isBlocked)
{
    _isBlocked = isBlocked;
    //std::cout << "Intersection #" << _id << " isBlocked=" << isBlocked << std::endl;
}

// virtual function which is executed in a thread
void Intersection::simulate() // using threads + promises/futures + exceptions
{
    // launch vehicle queue processing in a thread
    threads.emplace_back(std::thread(&Intersection::processVehicleQueue, this));
}

void Intersection::processVehicleQueue()
{
    // print id of the current thread
    //std::cout << "Intersection #" << _id << "::processVehicleQueue: thread id = " << std::this_thread::get_id() << std::endl;

    // continuously process the vehicle queue
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // only proceed when at least one vehicle is waiting in the queue
        if (_waitingVehicles.getSize() > 0 && !_isBlocked)
        {
            // set intersection to "blocked" to prevent other vehicles from entering
            this->setIsBlocked(true);

            // permit entry to first vehicle in the queue (FIFO)
            _waitingVehicles.permitEntryToFirstInQueue();
        }
    }
}
