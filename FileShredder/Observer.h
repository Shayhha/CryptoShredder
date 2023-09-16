#ifndef _Observer_H
#define _Observer_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <vector>

using namespace std;

class Observable; //declaration for Observable class

/// <summary>
/// Observer class 
/// </summary>
class Observer {
public:
    virtual void update(Observable* observable) = 0; //method for observers to implement

    virtual ~Observer() {} //dtor for observer
};


/// <summary>
/// Observable class
/// </summary>
class Observable {
protected:
    vector<Observer*> observersList; //observers list

public:
    /// <summary>
    /// Method to add obervers to observers list
    /// </summary>
    /// <param name="observer"></param>
    void addObserver(Observer* observer) { 
        observersList.push_back(observer); //add the observer to the end of the vector 
    }

    /// <summary>
    /// Method to remove observer from observers list
    /// </summary>
    /// <param name="observer"></param>
    void removeObserver(Observer* observer) {
        auto observerIt = find(observersList.begin(), observersList.end(), observer); //find the index of the object in vector and get its iterator 
        // If the observer was found, remove it from observers list
        if (observerIt != observersList.end()) {
            observersList.erase(observerIt); //remove observer from observers list
        }
    }

    /// <summary>
    /// Method for notifing the observers when needed
    /// </summary>
    void notify() const {
        for (Observer* observer : observersList) {
            observer->update(const_cast<Observable*>(this)); //call update method for each observer
        }
    }

    virtual ~Observable() {} //dtor for observable
};
#endif