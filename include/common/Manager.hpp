#ifndef _MANAGER_H_
#define _MANAGER_H_ 1

  class Manager{
  public:
    virtual void Initialize()=0;
    virtual void Activate()=0;
    virtual void Loop()=0;
    virtual void Stop()=0;
    virtual void Deactivate()=0;
  };


#endif
