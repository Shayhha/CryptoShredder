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
    /**
	 * @brief Method that observers need to implement to get notified.
	 * @param Observable* observable
	 * @param bool state
     */
    virtual void Update(Observable* observable, bool state = true) = 0; //method for observers to implement


    /**
	 * @brief Destructor for Observer class.
     */
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
    void AddObserver(Observer* observer) { 
        observersList.push_back(observer); //add the observer to the end of the vector 
    }

    /**
     * @brief Method to remove observer from observers list.
     * @param Observer* observer
     */
    void RemoveObserver(Observer* observer) {
        auto observerIt = find(observersList.begin(), observersList.end(), observer); //find the index of the object in vector and get its iterator 
        // If the observer was found, remove it from observers list
        if (observerIt != observersList.end()) {
            observersList.erase(observerIt); //remove observer from observers list
        }
    }

    /**
     * @brief Method for notifying the observers when needed.
	 * @param bool state
     */
    void Notify(bool state = true) const {
        for (Observer* observer : observersList) {
            observer->Update(const_cast<Observable*>(this), state); //call update method for each observer
        }
    }

    /**
     * @brief Destructor for observable class.
     */
    virtual ~Observable() {} 
};
#endif