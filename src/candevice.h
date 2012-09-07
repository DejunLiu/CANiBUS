#ifndef __CANIBUSD_CANDEVICE_H__
#define __CANIBUSD_CANDEVICE_H__

#include "sessionobject.h"

#include "vector"

class HackSession;

class CanDevice : public SessionObject
{
public:
	CanDevice(int id);
	~CanDevice();
	std::vector<HackSession *>sessions() { return m_sessions; }
	void addSession(HackSession *session);
private:
	std::vector<HackSession *>m_sessions;
};

class CanbusSimulator : public CanDevice
{
public:
	CanbusSimulator(int id);
};

#endif
