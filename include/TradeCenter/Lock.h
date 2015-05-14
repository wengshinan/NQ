#ifndef NQ_LOCK_H
#define NQ_LOCK_H
#include <mutex>

class CLock
{
public:
	CLock(CRITICAL_SECTION* pCs)
		: m_pCs(pCs)
	{
		EnterCriticalSection(m_pCs);
	}
	~CLock()
	{
		LeaveCriticalSection(m_pCs);
	}
private:
	CRITICAL_SECTION* m_pCs;
};

#endif // !NQ_LOCK_H

