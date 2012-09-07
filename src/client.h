#ifndef __CANIBUSD_CLIENT_H__
#define __CANIBUSD_CLIENT_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "sessionobject.h"

class HackSession;
class Socket;

class Client : public SessionObject
{
public:
	Client(Socket *socket, int id);
	virtual ~Client();

	void sendClientMsg();
	void setHackSession(HackSession *hax);

	void ioWrite(std::string data);
        void ioWrite(const char *, ...);
        void ioInfo(const char *data, ...);
        void ioInfo(const std::string data);
        void ioError(const char *data, ...);
        void ioError(const std::string data);
	void ioNoSuchCmd(const std::string data);

	void closeSocket();
	void setSocket(Socket *socket);
	Socket *socket() { return m_socket; }
	void setSession(HackSession *session) { m_session = session; }
	HackSession *session() { return m_session; }
	void setRequestedUpdate(const bool request) { m_requestedUpdate = request; }
	bool requestUpdate() { return m_requestedUpdate; }
private:
	bool m_requestedUpdate;
	Socket *m_socket;
	HackSession *m_session;
};

#endif
