#ifndef _Observer_H
#define _Observer_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Observable; //declaration for Observable class

/**
 * @brief Observer class .
 */
class Observer {
public:
    virtual void update(Observable* observable) = 0; //method for observers to implement

    virtual ~Observer() {} //dtor for observer
};


/**
 * @brief Observable class.
 */
class Observable {
protected:
    vector<Observer*> observersList; //observers list

public:
    /**
     * @brief Method to add observers to observers list.
     * @param Observer* observer
     */
    void addObserver(Observer* observer) { 
        observersList.push_back(observer); //add the observer to the end of the vector 
    }

    /**
     * @brief Method to remove observer from observers list.
     * @param Observer* observer
     */
    void removeObserver(Observer* observer) {
        auto observerIt = find(observersList.begin(), observersList.end(), observer); //find the index of the object in vector and get its iterator 
        // If the observer was found, remove it from observers list
        if (observerIt != observersList.end()) {
            observersList.erase(observerIt); //remove observer from observers list
        }
    }

    /**
     * @brief Method for notifying the observers when needed.
     */
    void notify() const {
        for (Observer* observer : observersList) {
            observer->update(const_cast<Observable*>(this)); //call update method for each observer
        }
    }

    /**
     * @brief destructor for observable class.
     */
    virtual ~Observable() {} 
};
#endif