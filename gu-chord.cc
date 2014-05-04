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
#include <openssl/sha.h>
#include <stdio.h>
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
    .AddAttribute ("SendStableMessageTimeout",
                 "Timeout value for STABLE_REQ in milliseconds",
                 TimeValue (MilliSeconds (10000)),
                 MakeTimeAccessor (&GUChord::m_sendStableTimeout),
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
   nodeID = getNodeID(m_mainAddress);
   predecessor = "";
  // Configure timers
  m_auditPingsTimer.SetFunction (&GUChord::AuditPings, this);
  m_sendStableTimer.SetFunction (&GUChord::startSendingStableReq, this);
  // Start timers
  m_auditPingsTimer.Schedule (m_pingTimeout);
  //m_sendStableTimer.Schedule (m_sendStableTimeout);
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
                SendJoinRequest(ResolveNodeIpAddress(str), m_mainAddress, ResolveNodeIpAddress(str), nodeID);
      }
  }else if (command == "LEAVE"){

      //send leave requests to successor and predecessor
      SendLeaveRequest(succIP, succIP, predIP);    
      SendLeaveRequest(predIP, succIP, predIP);

  }else if (command == "RINGSTATE"){

        std::cout<<"Chord ID: " << nodeID << "  Succ ID: " << successor << std::endl;

        CHORD_LOG ("Network Node: " << ReverseLookup(GetMainInterface()) << " Node ID: " << nodeID << " Successor: " << successor << " Predecessor: " << predecessor );

        SendRingStateMessage(succIP, nodeID);
  }else if (command == "STABILIZE"){
        
                SendStableReq(succIP);
        
  }


}

void
GUChord::startSendingStableReq(){

        
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
GUChord::getNodeID( Ipv4Address addr ){

        uint8_t seperateBytes[5];
        char value1[32], value2[32], value3[32], value4[32];
        char period[2];
        period[0] = '.';
        char buffer[256];
        
        addr.Serialize(seperateBytes);

        sprintf(value1, "%d", seperateBytes[0]);
        sprintf(value2, "%d", seperateBytes[1]);
        sprintf(value3, "%d", seperateBytes[2]);
        sprintf(value4, "%d", seperateBytes[3]);

        strncpy(buffer, value1, sizeof(buffer));
        strncat(buffer, period, sizeof(buffer));
        strncat(buffer, value2, sizeof(buffer));
        strncat(buffer, period, sizeof(buffer));
        strncat(buffer, value3, sizeof(buffer));
        strncat(buffer, period, sizeof(buffer));
        strncat(buffer, value4, sizeof(buffer));
        
        unsigned char digest[SHA_DIGEST_LENGTH];
        std::string input = (std::string)buffer;
         
        SHA_CTX ctx;
        SHA1_Init(&ctx);
        SHA1_Update(&ctx, (unsigned char*)input.c_str(), input.size());
        SHA1_Final(digest, &ctx);
         
        char mdString[SHA_DIGEST_LENGTH*2+1];
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
        
        printf("%s\n", mdString);

        return (std::string)mdString;
}

//Set local node to be landmark node by changing boolean values and succ, predecessor
void
GUChord::SetSelfToLandmark(){

        successor = nodeID;
        succIP = m_mainAddress;
        //std::cout<<"\n\nLandmark ID: "<<nodeID<<std::endl;
        //std::cout<<"\nSuccessor ID: "<<successor<<std::endl;

} 

//Send a Join Message to attempt to join a Chord Network
void
GUChord::SendJoinRequest(Ipv4Address destAddress, Ipv4Address srcAddress, Ipv4Address landmarkAddress, std::string nId)
{

if (destAddress != Ipv4Address::GetAny ())
    {

      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending CHORD_JOIN to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::CHORD_JOIN, transactionId);
      message.SetChordJoin (nId, successor, succIP, srcAddress, landmarkAddress);
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
GUChord::SendJoinResponse(Ipv4Address destAddress, Ipv4Address succ, std::string newSuccessor)
{

        if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending CHORD_JOIN_RSP to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::CHORD_JOIN_RSP, transactionId);
      
      message.SetChordJoinRsp (newSuccessor, succ);
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      std::cout<<"JOIN RESPONSE FAILED"<<std::endl;
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
GUChord::SendStableReq(Ipv4Address destAddress){

  if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending STABLE_REQ to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::STABLE_REQ, transactionId);
      
      message.SetStableReq ();
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      std::cout<<"STABLE REQ FAILED" <<std::endl;
    }
}

void
GUChord::SendStableRsp(Ipv4Address destAddress, std::string predecessorId, Ipv4Address predecessorIp){

        if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending STABLE_RSP to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::STABLE_RSP, transactionId);
      
      message.SetStableRsp (predecessorId, predecessorIp);
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      std::cout<<"STABLE RSP FAILED" <<std::endl;
    }



}

void
GUChord::SendLeaveRequest(Ipv4Address destAddress, Ipv4Address succ, Ipv4Address pred){

if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending CHORD_LEAVE to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::CHORD_LEAVE, transactionId);
      
      message.SetChordLeave (succ, pred);
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      std::cout<<"LEAVE REQUEST FAILED" <<std::endl;
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
      case GUChordMessage::STABLE_REQ:
        ProcessStableReq(message, sourceAddress, sourcePort);
        break;
      case GUChordMessage::STABLE_RSP:
        ProcessStableRsp(message, sourceAddress, sourcePort);
        break;
      case GUChordMessage::CHORD_LEAVE:
        ProcessChordLeave(message, sourceAddress, sourcePort);
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
void    
GUChord::ProcessChordJoin (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort)
{

        std::string messageNodeID = message.GetChordJoin().requesterID;
        Ipv4Address originAddress = message.GetChordJoin().originatorAddress;
        Ipv4Address landmarkIP = message.GetChordJoin().landmarkAddress;
        std::string landmarkID = getNodeID(landmarkIP);

        std::cout<<"Recieved join request message with messageNodeID: "<< messageNodeID << "mainAddress: " << m_mainAddress << " originAddress: "<< originAddress << " node ID: "<< nodeID << " Successor: " << successor << " Pred: " << predecessor << std::endl;
        
        
        if( messageNodeID < successor || successor < nodeID){
                
                if( successor < nodeID && messageNodeID < nodeID ){
                        SendJoinRequest(succIP, originAddress, landmarkIP, messageNodeID);
                }/*//Code has not been tested
                 else if( succIP == landmarkIP ){
                        if( messageNodeID > successor ){
                                SendJoinResponse(originAddress, succIP, successor);
                                succIP = originAddress;
                                successor = messageNodeID;                        
                        
                        }else if( messageNodeID < successor ){
                                SendJoinResponse(succIP, originAddress, messageNodeID);
                                SendJoinResponse(originAddress, landmarkSuccIP, landmarkSuccID);
                        }
                }*/else{
                        SendJoinResponse(originAddress, succIP, successor);
                        succIP = originAddress;
                        successor = messageNodeID;
                }      
                
        }else if( messageNodeID > successor ){
                
                //check if landmark, if it is then pass succ of landmark in message
                SendJoinRequest(succIP, originAddress, landmarkIP, messageNodeID);

        }else if( successor == nodeID ){

                SendJoinResponse(originAddress, m_mainAddress, nodeID);
                
                succIP = predIP = originAddress;
                successor = predecessor = messageNodeID;

        }       
}
void
GUChord::ProcessChordJoinRsp (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort)
{

        succIP = message.GetChordJoinRsp().successorVal;
        successor = message.GetChordJoinRsp().newSucc;

        std::cout<<"Changing successor of Node ID: "<< nodeID << " to: "<<succIP<<", Node ID: " << successor << std::endl;

}

void 
GUChord::PrintRingState(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

        std::string origin = message.GetRingState().originatorNodeID;

        if(  origin != nodeID ){
                CHORD_LOG ("Network Node: " << ReverseLookup(GetMainInterface()) << " Node ID: " << nodeID << " Successor: " << successor << " Predecessor: " << predecessor );

                std::cout<<"Chord ID: " << nodeID << "  Succ ID: " << successor << std::endl;
                
                SendRingStateMessage(succIP, origin);

        }

}

void
GUChord::ProcessStableReq(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

        
        if( sourceAddress != m_mainAddress ){    
                
                if( predecessor == "" )
                        SendStableRsp(sourceAddress, nodeID, m_mainAddress);
                else
                        SendStableRsp(sourceAddress, predecessor, predIP);

        }

}

void
GUChord::ProcessStableRsp(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

        std::string prdID = message.GetStableRsp().predID;
        Ipv4Address prdIP = message.GetStableRsp().predAddress;

        if( prdID == successor || prdIP == succIP ){
                std::cout<<"Successor's predecessor not set"<<std::endl;
                //successor's predecessor not set, so send a set predecessor message
        }else if( prdID > nodeID && prdID < successor ){
                successor = prdID;
                succIP = prdIP;
        }
        //Send a notify message to predecessor

}

void
GUChord::ProcessChordLeave (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

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


