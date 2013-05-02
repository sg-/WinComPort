
#include "scheduledtask.h"
 
// Static data init
std::map<UINT_PTR, ScheduledTask*> ScheduledTask::mTimerMap;
 
ScheduledTask::ScheduledTask(const UINT &aMilliseconds, const bool &aAutoStart /*= false*/)
    : mTaskId(0)
    , mInterval(aMilliseconds)
{
    if (aAutoStart)
        Resume();
}
 
ScheduledTask::~ScheduledTask(void)
{
    Suspend();
}
 
const UINT& ScheduledTask::GetInterval(void) const
// Gets the interval (in milliseconds) that this task will be performed at
{
    return mInterval;
}
 
void ScheduledTask::SetInterval(const UINT &aMilliseconds)
// Sets the interval (in milliseconds) that this task will be performed at
{
    if (aMilliseconds != mInterval)
    {
        Suspend();
        mInterval = aMilliseconds;
        Resume();
    }
}
 
bool ScheduledTask::IsRunning(void) const
// Returns true if this scheduled task is currently running
{
    return mTaskId != 0;
}
 
void ScheduledTask::Suspend(void)
// Suspend (stop) this scheduled task
{
    if (mTaskId != 0)
    {
        ::KillTimer(NULL, mTaskId);
 
        // NB: this if() is required to allow this object to be used by auto_ptr's 
        // (as we don't have control over what will be destructed first, the auto_ptr or our static map)
        if (mTimerMap.size() > 0)
            mTimerMap[mTaskId] = NULL;
 
        mTaskId = 0;
    }
}
 
void ScheduledTask::Resume(void)
// Resume (start) this scheduled task
{
    Suspend();
    mTaskId = ::SetTimer(NULL, 0, GetInterval(), TimerProc);
    mTimerMap[mTaskId] = this;
}
 
void CALLBACK ScheduledTask::TimerProc(HWND /*aHwnd*/, UINT /*aMessage*/, UINT_PTR aTimerId, DWORD /*aTime*/)
// Static timer proc, used to call object instances
{
    // Do we have an object associated with the task ID? We should!
    ScheduledTask *lTask = mTimerMap[aTimerId];
    if (!lTask)
        return; // Should not happen!
 
    lTask->PerformTask();
}

