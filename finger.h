#ifndef FINGER_H
#define FINGER_H

#include "ns3/gu-application.h"
#include "ns3/gu-chord-message.h"
#include "ns3/ping-request.h"

#include "ns3/ipv4-address.h"
#include <map>
#include <set>
#include <vector>
#include <string>
#include "ns3/socket.h"
#include "ns3/nstime.h"
#include "ns3/timer.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"

using namespace ns3;

class Finger
{
	private:
		std::string fingerID;
		Ipv4Address fingerAddress;
	
	public:
		Finger();
		void setFinger(std::string fingerNum, Ipv4Address fingerIP);
		void printFinger();
		std::string getFingerID() const;
		Ipv4Address getFingerAddr() const;

		friend bool operator== (Finger &finger1, Finger &finger2);
		friend bool operator< (Finger &finger1, Finger &finger2);
		friend bool operator> (Finger &finger1, Finger &finger2);
};



#endif
