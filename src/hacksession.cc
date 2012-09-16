#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "client.h"
#include "candevice.h"
#include "hacksession.h"

HackSession::HackSession(int id) : SessionObject(id, SHack)
{
	m_status = Config;
}

HackSession::~HackSession()
{

}

unsigned int HackSession::clients()
{
	return m_clients.size();
}

void HackSession::ioWrite(const char *fmt, ...)
{
        int n, size = 256;
        char *buf = new char[size];
        static std::string ioStr;
        va_list arg;

        buf[0] = 0;

        while (1)
        {
                va_start(arg, fmt);
                n = vsnprintf(buf, size, fmt, arg);
                va_end(arg);

                if (n > -1 && n < size)
                {
                        ioStr = buf;
                        delete[] buf;
                        ioWrite(ioStr);
                        return;
                }

                if (n > -1)
                        size = n+1;
                else
                        size *= 2;

                delete[] buf;
                buf = new char[size];
        }
}

void HackSession::ioWrite(const std::string data)
{
        Client *pTmp = 0;
        for(std::vector<Client *>::iterator it = m_clients.begin(); it != m_clients.end() && (pTmp = *it) ; ++it)
                pTmp->ioWrite(data);
}

void HackSession::ioInfo(const char *data, ...)
{
        va_list arg;
        char buf[2048];
        
        va_start(arg, data);
        vsnprintf(buf, sizeof(buf)-1, data, arg);
        va_end(arg);
        buf[sizeof(buf)-1] = 0;

        ioWrite("<canibusd><msg type=\"info\" value=\"%s\"/></canibusd>\n", buf);
}

void HackSession::ioInfo(const std::string data)
{
        ioWrite("<canibusd><msg type=\"info\" value=\"" + data + "\"/></canibusd>\n");
}

void HackSession::ioError(const std::string data)
{
        ioWrite("<canibusd><msg type=\"error\" value=\"" + data + "\"/></canibusd>\n");
}

const std::string HackSession::statusLabel()
{
	switch(m_status)
	{
	case Config:
		return "config";
	case Init:
		return "init";
	case Run:
		return "run";
	case End:
		return "end";
	default:
		return "default";
	}
}

void HackSession::sendStatus(Client *client)
{
	client->ioWrite("<canibusd><hacksessionupdate sessionid=\"%d\" status=\"%s\"/></canibusd>\n", m_id, statusLabel().c_str());
}

void HackSession::sendClientList(Client *client)
{
	Client *cTmp = 0;
	for(std::vector<Client *>::iterator it = m_clients.begin() ; it != m_clients.end() && (cTmp = *it) ; ++it)
	{
		client->ioWrite("<clientupdate clientid=\"%d\" name=\"%s\"/>", cTmp->id(), cTmp->name().c_str());
		client->ioWrite(cTmp->oldXMLUpdate(client, true));
	}
}

void HackSession::sendFullUpdate(Client *client, bool userRequest)
{
	if (client->requestUpdate())
	{
		// To prevent race condition
		return;
	}
	if (userRequest)
		client->setRequestedUpdate(true);

	sendStatus(client);

	if (m_status == Config)
		sendConfiguration(client);
	else if (m_status == Init || m_status == Run)
	{
		client->ioWrite("<canibusd>");
		sendClientList(client);
		client->ioWrite("</canibusd>\n");
	}
}

void HackSession::sendConfiguration(Client *client)
{
	bool edit = (client == m_master);
	client->ioWrite("<canibusd>");
	client->ioWrite("<configupdate sessionid=\"%d\"><option type=\"bool\" title=\"Read only mode\" command=\".seR\" value=\"%d\" edit=\"%d\"/></configupdate>", m_id, getBoolProperty("readonly"), edit);

	for(std::vector<SessionObject *>::iterator it = m_configOptions.begin(); it != m_configOptions.end() && (*it) ; ++it)
		client->ioWrite( (*it)->oldXMLUpdate(client, true) );
	client->ioWrite("</canibusd>\n");
}

bool HackSession::sendChildXMLUpdate(Client *cOutput, bool updateEmpty)
{
	// Send updates about config options
	SessionObject *configOption = 0;
	for(std::vector<SessionObject *>::iterator it = m_configOptions.begin() ; it != m_configOptions.end() && (configOption = *it) ; ++it)
	{
		// .. but only when changed and in scope
		std::string updateXML = configOption->oldXMLUpdate(cOutput);
		if (updateXML.size())
		{
			if (updateEmpty)
			{
				cOutput->ioWrite("<canibusd> ");
				updateEmpty = false;
			}
			cOutput->ioWrite("%s", updateXML.c_str());
		}
	}
	return updateEmpty;
}

void HackSession::unsetChildProperties()
{
	// Reset config options
	for(std::vector<SessionObject *>::iterator it = m_configOptions.begin(); it != m_configOptions.end() && (*it); ++it)
		(*it)->unsetPropertiesChanged();
}

void HackSession::delClient(Client *client)
{
	Client *target = 0;
	fprintf(stderr, "DEBUG: delClients() m_clients.size()==%ld\n", m_clients.size());
	for(std::vector<Client *>::iterator it = m_clients.begin() ; it != m_clients.end() && (target = *it) ; ++it)
		if(target == client) {
			if (m_master == client) {
				m_master = 0;
				setProperty("master", 0);
			}
			client->setBoolProperty("master", false, this);
			client->setSession(0);
			m_clients.erase(it);
		}
	setProperty("clients", m_clients.size() );
}

Client *HackSession::addClient(Client *client, bool isMaster)
{
	m_clients.push_back(client);
	setProperty("clients", m_clients.size() );

	if( m_clients.size() == getBoolProperty("maxclients") )
		setBoolProperty("private", true);

	client->setSession(this);
	addToScope(client);

	client->setBoolProperty("master", isMaster, this);

	if(isMaster)
	{
		m_master = client;
		setProperty("master", client->id());
	}

	if (m_status == Run)
	{
		m_status = Init;
		sendFullUpdate(client);
		m_status = Run;
	} else {
		sendFullUpdate(client);
	}

	return client;
}

void HackSession::electNewMaster()
{
	m_master = m_clients.front();
	setProperty("master", m_master->id());
		
}
