#include "ns3/finger.h"

using namespace ns3;

Finger::Finger()
{
	// Do Nothing
}
void Finger::setFinger(std::string fingerNum, Ipv4Address fingerIP){
	fingerID = fingerNum;
	fingerAddress = fingerIP;
}
std::string Finger::getFingerID() const{
	return fingerID;
}
Ipv4Address Finger::getFingerAddr() const{
	return fingerAddress;
}
void Finger::printFinger(){
	std::string spaceSize = "	";
	std::cout<<fingerID<<spaceSize<<fingerAddress<<std::endl;
}
bool operator== (Finger &finger1, Finger &finger2){
	return finger1.fingerID == finger2.fingerID;
}
bool operator< (Finger &finger1, Finger &finger2){
	return finger1.fingerID < finger2.fingerID;
}
bool operator> (Finger &finger1, Finger &finger2){
	return finger1.fingerID > finger2.fingerID;
}
