#ifndef _DRIVER_H_
#define _DRIVER_H_

#include<common//Manager.hpp>
#include<kernel//system//core//hardware//interrupts.hpp>


class Driver
{

public:
    Driver()=default;
    ~Driver()=default;

    virtual void Activate()=0;
    virtual void Reset()=0;
    virtual void Deactivate()=0;
};

class DriverManager:public Manager
{
public:
    Driver* drivers[265];
    int numDrivers=0;
    InterruptManager isrManager;
    virtual void reset()final{}
    virtual void Stop()final{}
    virtual void Loop()final{}
    virtual void Deactivate()final{}
public:
    DriverManager(InterruptManager& im):isrManager{im}{}
    virtual void addDriver(Driver*drv)final{
        drivers[numDrivers] = drv;
        numDrivers++;
    }

    virtual const InterruptManager& getInterruptManager()const final{
      return  isrManager;
    }


    virtual void Activate() final{
        for(int i = 0; i < numDrivers; i++)
            drivers[i]->Activate();
    }

    virtual void Initialize()final{}


};

#endif
