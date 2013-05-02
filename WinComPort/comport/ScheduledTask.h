#pragma once
 
#include <Windows.h>
#include <map>
 
/* Generic scheduled task class
    Allows object instances to perform a given task at at scheduled interval.
    Note that the interval is a minimum in this implementation, it's not a fixed interval,
    delays will happen based on how busy the operating thread is */

// http://www.greengingerwine.com/index.php/2011/05/using-the-windows-settimer-api-with-cpp-objects/

class ScheduledTask
{
    public:
        ScheduledTask(const UINT &aMilliseconds, const bool &aAutoStart = false);
        virtual ~ScheduledTask(void);
 
    private:
        UINT_PTR mTaskId;
        UINT mInterval; // In milliseconds
 
    protected:
        // Derived classes must implement this, it will be called upon to perform the actual task
        virtual void PerformTask(void) = 0;
 
    public:
        // Gets the interval (in milliseconds) that this task will be performed at
        const UINT& GetInterval(void) const;
 
        // Sets the interval (in milliseconds) that this task will be performed at
        void SetInterval(const UINT &aMilliseconds);
 
        // Returns true if this scheduled task is currently running
        bool IsRunning(void) const;
 
        // Suspend (stop) this scheduled task
        void Suspend(void);
 
        // Resume (start) this scheduled task
        void Resume(void);
 
    private:
        static std::map<UINT_PTR, ScheduledTask*> mTimerMap; // Map allowing us to delegate scheduled tasks to object instances
        static void CALLBACK TimerProc(HWND aHwnd, UINT aMessage, UINT_PTR aTimerId, DWORD aTime); // Static timer proc, used to call object instances
};

