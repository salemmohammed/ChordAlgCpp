/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 University of Pennsylvania
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "gu-chord.h"

#include "ns3/random-variable.h"
#include "ns3/inet-socket-address.h"
//#include <openssl/sha.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace ns3;

TypeId
GUChord::GetTypeId ()
{
  static TypeId tid = TypeId ("GUChord")
    .SetParent<GUApplication> ()
    .AddConstructor<GUChord> ()
    .AddAttribute ("AppPort",
                   "Listening port for Application",
                   UintegerValue (10001),
                   MakeUintegerAccessor (&GUChord::m_appPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PingTimeout",
                   "Timeout value for PING_REQ in milliseconds",
                   TimeValue (MilliSeconds (2000)),
                   MakeTimeAccessor (&GUChord::m_pingTimeout),
                   MakeTimeChecker ())
    ;
  return tid;
}

GUChord::GUChord ()
  : m_auditPingsTimer (Timer::CANCEL_ON_DESTROY)
{
  RandomVariable random;
  SeedManager::SetSeed (time (NULL));
  random = UniformVariable (0x00000000, 0xFFFFFFFF);
  m_currentTransactionId = random.GetInteger ();
}

GUChord::~GUChord ()
{

}

void
GUChord::DoDispose ()
{
  StopApplication ();
  GUApplication::DoDispose ();
}

void
GUChord::StartApplication (void)
{
  if (m_socket == 0)
    { 
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny(), m_appPort);
      m_socket->Bind (local);
      m_socket->SetRecvCallback (MakeCallback (&GUChord::RecvMessage, this));
    }
   
   m_mainAddress = GetMainInterface();
   nodeID = getNodeID(m_mainAddress, 8);
   is_landmark = false;  
  // Configure timers
  m_auditPingsTimer.SetFunction (&GUChord::AuditPings, this);
  // Start timers
  m_auditPingsTimer.Schedule (m_pingTimeout);
}

void
GUChord::StopApplication (void)
{
  // Close socket
  if (m_socket)
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
      m_socket = 0;
    }

  // Cancel timers
  m_auditPingsTimer.Cancel ();

  m_pingTracker.clear ();
}

/***********************************************************************************/

void
GUChord::ProcessCommand (std::vector<std::string> tokens)
{
  std::vector<std::string>::iterator iterator = tokens.begin();
  std::string command = *iterator;

  //ADDED COMMANDS IN ASSIGNMENT SHEET

  if (command == "JOIN")
    {
      if (tokens.size() < 2)
        {
          ERROR_LOG ("Insufficient CHORD params..."); 
          return;
        }
      iterator++;
      std::istringstream sin (*iterator);
      uint32_t nodeNumber;
      sin >> nodeNumber;

      std::string thisNodeNum = GetNodeNumber();

      std::stringstream sd;
      sd << nodeNumber;
      std::string str = "";
      sd >> str;

      if( thisNodeNum == str ){
                SetSelfToLandmark();
      }else{
                std::cout<<"sending join request."<<std::endl;
                std::string landmark = "1";
                Ipv4Address landmarkIP = ResolveNodeIpAddress(landmark);
                SendJoinRequest(landmarkIP, m_mainAddress);
      }
  }else if (command == "LEAVE"){

      std::cout<<"I'm out."<<std::endl;

  }else if (command == "RINGSTATE"){

        std::cout<<"Printing ring state: " << std::endl;

        CHORD_LOG ("Network Node: " << ReverseLookup(GetMainInterface()) << " Node ID: " << nodeID << " Successor: " << successor << " Predecessor: " << predecessor );

        SendRingStateMessage(succIP, nodeID);
  }


}


std::string
GUChord::GetNodeNumber(){

      std::stringstream ss;
      ss << GetNode()->GetId();
      std::string str;
      ss >> str;

      return str;

}
Ipv4Address
GUChord::GetMainInterface()
{
      return ResolveNodeIpAddress(GetNodeNumber());
}

std::string
GUChord::getNodeID( Ipv4Address addr, uint32_t n ){

        uint8_t seperateBytes[4];

        addr.Serialize(seperateBytes);
        uint32_t totalValue = seperateBytes[0] + seperateBytes[1] + seperateBytes[2] + seperateBytes[3];
        totalValue = totalValue % n;

        std::stringstream ss;
        ss << totalValue;
        std::string str;
        ss >> str;
        
        return str;

      //Test SHA 1 Hashing (doesn't work yet)
      /*unsigned char *message = "yourmom";
      unsigned char *result;
      uint32_t messagelength = sizeof(char);
      

      SHA1(message, messagelength, result);
      */
}

//Set local node to be landmark node by changing boolean values and succ, predecessor
void
GUChord::SetSelfToLandmark(){

        //pseudocode: predecessor = nil
        successor = nodeID;
        succIP = m_mainAddress;
        is_landmark = true;

} 

//Send a Join Message to attempt to join a Chord Network
void
GUChord::SendJoinRequest(Ipv4Address destAddress, Ipv4Address srcAddress)
{

if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending CHORD_JOIN to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::CHORD_JOIN, transactionId);
      
      message.SetChordJoin (nodeID, srcAddress);
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      std::cout<<"JOIN REQUEST FAILED" <<std::endl;
    }
}

void
GUChord::SendJoinResponse(Ipv4Address destAddress, Ipv4Address successor, Ipv4Address predecessor )
{

        if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending CHORD_JOIN_RSP to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::CHORD_JOIN_RSP, transactionId);
      
      message.SetChordJoinRsp (successor, predecessor);
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      std::cout<<"JOIN RESPONSE FAILED" <<std::endl;
    }

}

void
GUChord::SendRingStateMessage(Ipv4Address destAddress, std::string srcNodeID){

        if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending RING_STATE to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::RING_STATE, transactionId);
      
      message.SetRingState (srcNodeID);
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      std::cout<<"RING STATE FAILED" <<std::endl;
    }
}

void
GUChord::RecvMessage (Ptr<Socket> socket)
{
  Address sourceAddr;
  Ptr<Packet> packet = socket->RecvFrom (sourceAddr);
  InetSocketAddress inetSocketAddr = InetSocketAddress::ConvertFrom (sourceAddr);
  Ipv4Address sourceAddress = inetSocketAddr.GetIpv4 ();
  uint16_t sourcePort = inetSocketAddr.GetPort ();
  GUChordMessage message;
  packet->RemoveHeader (message);

  switch (message.GetMessageType ())
    {
      case GUChordMessage::PING_REQ:
        ProcessPingReq (message, sourceAddress, sourcePort);
        break;
      case GUChordMessage::PING_RSP:
        ProcessPingRsp (message, sourceAddress, sourcePort);
        break;
      case GUChordMessage::CHORD_JOIN:
        ProcessChordJoin(message, sourceAddress, sourcePort);
        break;
      case GUChordMessage::CHORD_JOIN_RSP:
        ProcessChordJoinRsp(message, sourceAddress, sourcePort);
        break;
      case GUChordMessage::RING_STATE:
        PrintRingState(message, sourceAddress, sourcePort);
        break;
      default:
        ERROR_LOG ("Unknown Message Type!");
        break;
    }
}

/* Implementation of Chord */
void
GUChord::FindSuccessor(){

}
void
GUChord::FindPredecessor(){

}
void
GUChord::ClosestPrecedingFinger(){

}
void    //Process Chord Join message and set landmark/successors
GUChord::ProcessChordJoin (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort)
{
        
        std::string messageNodeID = message.GetChordJoin().requesterID;
        Ipv4Address originAddress = message.GetChordJoin().originatorAddress;

        std::cout<<"Recieved join request message with messageNodeID: "<< messageNodeID << "mainAddress: " << m_mainAddress << " originAddress: "<< originAddress << " node ID: "<< nodeID << " Successor: " << successor << " Pred: " << predecessor << std::endl;
        

        if( successor == nodeID ){
                std::cout<<"only one node in the network"<<std::endl;
                
                //Send a CHORD_JOIN_RSP message back to join requestor with self as the successor
                SendJoinResponse(originAddress, succIP, m_mainAddress);
                
                //Set successor and predeccesor for this node
                succIP = predIP = originAddress;
                successor = predecessor = getNodeID(originAddress, 8);

                std::cout<<"Recieved join request message with messageNodeID: "<< messageNodeID << "mainAddress: " << m_mainAddress << " originAddress: "<< originAddress << " node ID: "<< nodeID << " Successor: " << successor << " Pred: " << predecessor << std::endl;

        }else if ( (messageNodeID > nodeID && messageNodeID < successor) || successor < nodeID ){

                

                //Change successor's predecessor
                //Send a CHORD_JOIN_RSP message back to join requestor with this node's successor as join requestor's successor
                std::cout<<"Recieved join request message with messageNodeID: "<< messageNodeID << "mainAddress: " << m_mainAddress << " originAddress: "<< originAddress << " node ID: "<< nodeID << " Successor: " << successor << " Pred: " << predecessor << std::endl;
                SendJoinResponse(originAddress, succIP, m_mainAddress);

                succIP = originAddress;
                successor = messageNodeID;

        }else if( messageNodeID < nodeID || messageNodeID > successor ){

                
                //Create a CHORD_JOIN message and send it to Ipv4Address succIP
                std::cout << "NodeID needs to be passed to next successor" << std::endl;
                SendJoinRequest(succIP, originAddress);
        }else{
                std::cout<<"Fuckme"<<std::endl;

        }


       

        
        //std::cout<< GetMainInterface() <<" - This is a join request from node: " << ReverseLookup(sourceAddress) << " ID: " << message.GetChordJoin().requesterID << " IP: " << message.GetChordJoin().originatorAddress << std::endl;
}
void
GUChord::ProcessChordJoinRsp (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort)
{

        
        succIP = message.GetChordJoinRsp().successorVal;
        predIP = message.GetChordJoinRsp().predecessorVal;        

        successor = getNodeID( succIP, 8 );
        predecessor = getNodeID( predIP, 8);

        std::cout<< ReverseLookup(GetMainInterface()) << " recieved a join response. New successor is: " << successor << ". New predecessor is: "<< predecessor << ".\n" << std::endl;


}

void 
GUChord::PrintRingState(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

        std::string origin = message.GetRingState().originatorNodeID;

        if(  origin != nodeID ){
                CHORD_LOG ("Network Node: " << ReverseLookup(GetMainInterface()) << " Node ID: " << nodeID << " Successor: " << successor << " Predecessor: " << predecessor );
                
                SendRingStateMessage(succIP, origin);

        }

}








/********************************************************************************************/



void
GUChord::SendPing (Ipv4Address destAddress, std::string pingMessage)
{
  if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending PING_REQ to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " Message: " << pingMessage << " transactionId: " << transactionId);
      Ptr<PingRequest> pingRequest = Create<PingRequest> (transactionId, Simulator::Now(), destAddress, pingMessage);
      // Add to ping-tracker
      m_pingTracker.insert (std::make_pair (transactionId, pingRequest));
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::PING_REQ, transactionId);
      message.SetPingReq (pingMessage);
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      m_pingFailureFn (destAddress, pingMessage);
    }
}


void
GUChord::ProcessPingReq (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort)
{

    // Use reverse lookup for ease of debug
    std::string fromNode = ReverseLookup (sourceAddress);
    CHORD_LOG ("Received PING_REQ, From Node: " << fromNode << ", Message: " << message.GetPingReq().pingMessage);
    // Send Ping Response
    GUChordMessage resp = GUChordMessage (GUChordMessage::PING_RSP, message.GetTransactionId());
    resp.SetPingRsp (message.GetPingReq().pingMessage);
    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (resp);
    m_socket->SendTo (packet, 0 , InetSocketAddress (sourceAddress, sourcePort));
    // Send indication to application layer
    m_pingRecvFn (sourceAddress, message.GetPingReq().pingMessage);
}

void
GUChord::ProcessPingRsp (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort)
{
  // Remove from pingTracker
  std::map<uint32_t, Ptr<PingRequest> >::iterator iter;
  iter = m_pingTracker.find (message.GetTransactionId ());
  if (iter != m_pingTracker.end ())
    {
      std::string fromNode = ReverseLookup (sourceAddress);
      CHORD_LOG ("Received PING_RSP, From Node: " << fromNode << ", Message: " << message.GetPingRsp().pingMessage);
      m_pingTracker.erase (iter);
      // Send indication to application layer
      m_pingSuccessFn (sourceAddress, message.GetPingRsp().pingMessage);
    }
  else
    {
      DEBUG_LOG ("Received invalid PING_RSP!");
    }
}

void
GUChord::AuditPings ()
{
  std::map<uint32_t, Ptr<PingRequest> >::iterator iter;
  for (iter = m_pingTracker.begin () ; iter != m_pingTracker.end();)
    {
      Ptr<PingRequest> pingRequest = iter->second;
      if (pingRequest->GetTimestamp().GetMilliSeconds() + m_pingTimeout.GetMilliSeconds() <= Simulator::Now().GetMilliSeconds())
        {
          DEBUG_LOG ("Ping expired. Message: " << pingRequest->GetPingMessage () << " Timestamp: " << pingRequest->GetTimestamp().GetMilliSeconds () << " CurrentTime: " << Simulator::Now().GetMilliSeconds ());
          // Remove stale entries
          m_pingTracker.erase (iter++);
          // Send indication to application layer
          m_pingFailureFn (pingRequest->GetDestinationAddress(), pingRequest->GetPingMessage ());
        }
      else
        {
          ++iter;
        }
    }
  // Rechedule timer
  m_auditPingsTimer.Schedule (m_pingTimeout); 
}

uint32_t
GUChord::GetNextTransactionId ()
{
  return m_currentTransactionId++;
}

void
GUChord::StopChord ()
{
  StopApplication ();
}

void
GUChord::SetPingSuccessCallback (Callback <void, Ipv4Address, std::string> pingSuccessFn)
{
  m_pingSuccessFn = pingSuccessFn;
}


void
GUChord::SetPingFailureCallback (Callback <void, Ipv4Address, std::string> pingFailureFn)
{
  m_pingFailureFn = pingFailureFn;
}

void
GUChord::SetPingRecvCallback (Callback <void, Ipv4Address, std::string> pingRecvFn)
{
  m_pingRecvFn = pingRecvFn;
}


