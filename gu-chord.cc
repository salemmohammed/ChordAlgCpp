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
                 TimeValue (MilliSeconds (2000)),
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
  m_sendStableTimer.Schedule (m_sendStableTimeout);
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
GUChord::startSendingStableReq(){

        SendStableReq(succIP);

        m_sendStableTimer.Schedule (m_sendStableTimeout);

}
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
                Ipv4Address lndmrkIP = ResolveNodeIpAddress(str);
                std::string lndmrkID = "notset";                
                SendJoinRequest(lndmrkIP, m_mainAddress, nodeID, lndmrkIP, lndmrkID);
      }
  }else if (command == "LEAVE"){

      //send leave requests to successor and predecessor
      SendLeaveRequest(succIP, succIP, predIP, successor, predecessor);    
      SendLeaveRequest(predIP, succIP, predIP, successor, predecessor);

  }else if (command == "RINGSTATE"){

        std::cout<<"\nPred ID: "<<predecessor<<"\nChord ID: " << nodeID << "\nSucc ID: " << successor << std::endl<<std::endl;

        CHORD_LOG ("Network Node: " << ReverseLookup(GetMainInterface()) << " Node ID: " << nodeID << " Successor: " << successor << " Predecessor: " << predecessor );

        SendRingStateMessage(succIP, nodeID);
  }else if (command == "STABILIZE"){
        
                SendStableReq(succIP);
        
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
        std::cout<<"Landmark: ID: "<< nodeID<< std::endl;
        successor = nodeID;
        succIP = m_mainAddress;
} 

//Send a Join Message to attempt to join a Chord Network
void
GUChord::SendJoinRequest( Ipv4Address destAddress, Ipv4Address srcAddress, std::string srcId, Ipv4Address landmarkAddress, std::string landmarkId )
{

if (destAddress != Ipv4Address::GetAny ())
    {

      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending CHORD_JOIN to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::CHORD_JOIN, transactionId);
      message.SetChordJoin ( srcId, landmarkId, srcAddress, landmarkAddress);
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

//std::cout<<"Sending stabilize request"<<std::endl;
  if (destAddress != Ipv4Address::GetAny () )
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending STABLE_REQ to Node: " << ReverseLookup(succIP) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
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
        //std::cout<<"Sending STABLE_RSP"<<std::endl;
      
      
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
GUChord::SendSetPred(Ipv4Address destAddress, std::string ndId, Ipv4Address ndAddr){

        if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending SET_PRED to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::SET_PRED, transactionId);
      
      message.SetSetPred (ndId, ndAddr);
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      std::cout<<"SET PREDECESSOR FAILED" <<std::endl;
    }

}

void
GUChord::SendNotify(Ipv4Address destAddress, std::string ndId, Ipv4Address ndAddr){

        if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending NOTIFY to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::NOTIFY, transactionId);
      
      message.SetNotify (ndId, ndAddr);
      packet->AddHeader (message);
      m_socket->SendTo (packet, 0 , InetSocketAddress (destAddress, m_appPort));
    }
  else
    {
      // Report failure   
      std::cout<<"NOTIFY FAILED" <<std::endl;
    }


}

void
GUChord::SendLeaveRequest(Ipv4Address destAddress, Ipv4Address succ, Ipv4Address pred, std::string sucIp, std::string predIp){

if (destAddress != Ipv4Address::GetAny ())
    {
      uint32_t transactionId = GetNextTransactionId ();
      CHORD_LOG ("Sending CHORD_LEAVE to Node: " << ReverseLookup(destAddress) << " IP: " << destAddress << " transactionId: " << transactionId);
      
      
      Ptr<Packet> packet = Create<Packet> ();
      GUChordMessage message = GUChordMessage (GUChordMessage::CHORD_LEAVE, transactionId);
      
      message.SetChordLeave (sucIp, predIp, succ, pred);
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
      case GUChordMessage::SET_PRED:
        ProcessSetPred(message, sourceAddress, sourcePort);
        break;
      case GUChordMessage::NOTIFY:
        ProcessNotify(message, sourceAddress, sourcePort);
        break;
      case GUChordMessage::CHORD_LEAVE:
        ProcessChordLeave(message, sourceAddress, sourcePort);
        break;
      default:
        ERROR_LOG ("Unknown Message Type!");
        break;
    }
}

void    
GUChord::ProcessChordJoin (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort)
{

        std::string messageNodeID = message.GetChordJoin().requesterID;
        std::string landmID = message.GetChordJoin().landmarkID;
        Ipv4Address originAddress = message.GetChordJoin().originatorAddress;
        Ipv4Address landmarkIP = message.GetChordJoin().landmarkAddress;
        

        std::cout<<"Recieved join request message with messageNodeID: "<< messageNodeID << "mainAddress: " << m_mainAddress << " originAddress: "<< originAddress << " node ID: "<< nodeID << " Successor: " << successor << " Pred: " << predecessor << std::endl;
        

        if( landmID == "notset" && m_mainAddress != succIP ){
                std::cout<<"LMID NOT SET"<<std::endl;
                //SendJoinRequest(succIP, originAddress, messageNodeID, m_mainAddress, nodeID);

                if( nodeID < successor && messageNodeID > successor ){
                        //std::cout<<"non-wraparound case. pass node."<<std::endl;
                        SendJoinRequest(succIP, originAddress, messageNodeID, m_mainAddress, nodeID);
                        
                }else if( nodeID < successor && messageNodeID < successor ){
                        
                        //std::cout<<"non-wraparound case. place node."<<std::endl;
                
                        SendJoinResponse(originAddress, succIP, successor);
                        
                        succIP = originAddress;
                        successor = messageNodeID;
                }else if( nodeID > successor && messageNodeID > successor ){
                        //std::cout<<"wraparound case. pass node."<<std::endl;
        
                        SendJoinRequest(succIP, originAddress, messageNodeID, m_mainAddress, nodeID);
                }else{
                        //std::cout<<"wraparound case. place node."<<
                        SendJoinResponse(originAddress, succIP, successor);
                        
                        succIP = originAddress;
                        successor = messageNodeID;
                          
                }
                                                       
        }else if( successor == nodeID ){

                SendJoinResponse(originAddress, m_mainAddress, nodeID);
                
                succIP = originAddress;
                successor = messageNodeID;

        } else if( successor == landmID ){
                
                if( messageNodeID < successor && nodeID < successor ){
                        
                        SendJoinResponse(originAddress, succIP, successor);
                        
                        succIP = originAddress;
                        successor = messageNodeID;
 
                } else if( messageNodeID > successor && nodeID < successor){

                        SendJoinRequest(succIP, originAddress, messageNodeID, landmarkIP, landmID);
                }else if( messageNodeID > nodeID && nodeID > successor){

                        SendJoinResponse(originAddress, succIP, successor);
                        
                        succIP = originAddress;
                        successor = messageNodeID;
                }else{
                        SendJoinRequest(succIP, originAddress, messageNodeID, landmarkIP, landmID);
                }

        } else if( successor != landmID ){
                
                if( nodeID < successor && messageNodeID > successor ){
                        //std::cout<<"non-wraparound case. pass node."<<std::endl;
                        SendJoinRequest(succIP, originAddress, messageNodeID, landmarkIP, landmID);
                        
                }else if( nodeID < successor && messageNodeID < successor ){
                        
                        //std::cout<<"non-wraparound case. place node."<<std::endl;
                
                        SendJoinResponse(originAddress, succIP, successor);
                        
                        succIP = originAddress;
                        successor = messageNodeID;
                }else if( nodeID > successor && messageNodeID > successor ){
                        //std::cout<<"wraparound case. pass node."<<std::endl;
        
                        SendJoinRequest(succIP, originAddress, messageNodeID, landmarkIP, landmID);
                }else{
                        //std::cout<<"wraparound case. place node."<<
                        SendJoinResponse(originAddress, succIP, successor);
                        
                        succIP = originAddress;
                        successor = messageNodeID;
                          
                }     

        }
           
}
void
GUChord::ProcessChordJoinRsp (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort)
{

        succIP = message.GetChordJoinRsp().successorVal;
        successor = message.GetChordJoinRsp().newSucc;

        std::cout<<"Changing successor of Node ID: "<< nodeID << " to: "<< succIP <<", Node ID: " << successor << std::endl;

}

void 
GUChord::PrintRingState(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

        std::string origin = message.GetRingState().originatorNodeID;

        if(  origin != nodeID ){
                CHORD_LOG ("Network Node: " << ReverseLookup(GetMainInterface()) << " Node ID: " << nodeID << " Successor: " << successor << " Predecessor: " << predecessor );

                std::cout<<"Pred ID: "<<predecessor<<"\nChord ID: " << nodeID << "\nSucc ID: " << successor << std::endl<<std::endl;
                
                SendRingStateMessage(succIP, origin);

        }

}

void
GUChord::ProcessStableReq(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

        
        if( sourceAddress != m_mainAddress ){    
                
                if( predecessor == "" ){
                        //std::cout<<"not set"<<std::endl;
                        SendStableRsp(sourceAddress, nodeID, m_mainAddress);
                }else{
                        //std::cout<<"set"<<std::endl;
                        SendStableRsp(sourceAddress, predecessor, predIP);
                }

        }

}

void
GUChord::ProcessStableRsp(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

        std::string prdID = message.GetStableRsp().predID;
        Ipv4Address prdIP = message.GetStableRsp().predAddress;

        if( prdID == successor ){
                //successor's predecessor not set, so send a set predecessor message to successor
                //if( succIP != m_mainAddress ){
                        //std::cout<<"StableMessageRecieved"<<std::endl;
                        SendSetPred( succIP, nodeID, m_mainAddress);
                //}
        }
        /*if( (prdID > nodeID && prdID < successor) || (nodeID > successor && prdID < nodeID && prdID < successor) ){
                
                successor = prdID;
                succIP = prdIP;
                //std::cout<<"Changing successor of NODE ID: "<<nodeID<<"\nto: "<<successor;
        }*/
        //Send a notify message to predecessor

        //std::cout<<"Sending notify to: "<<prdIP <<std::endl;
        //SendNotify(prdIP, nodeID, m_mainAddress);

}

void
GUChord::ProcessSetPred(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){
        //std::cout<<"SetPredRecieved"<<std::endl;

        std::string setPredID = message.GetSetPred().newPredID;
        Ipv4Address setPredIP = message.GetSetPred().newPredIP;
        
        if( predecessor == "" || (predecessor < nodeID && setPredID > predecessor) || (predecessor > nodeID && ( setPredID > predecessor || setPredID < nodeID )) ){
                        
                predecessor = setPredID;
                predIP = setPredIP;
        }
        //std::cout<<"Node ID: "<<nodeID<<"\nNew predecessor: "<< predecessor << "  Pred IP: "<<predIP <<std::endl;

}

void
GUChord::ProcessNotify(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

        std::string messageNodeID = message.GetNotify().potentialPredID;
        Ipv4Address messageNodeIP = message.GetNotify().potentialPredIP;
        //std::string successor
        
        if( predecessor == "" || messageNodeID > predecessor || (nodeID > successor && messageNodeID < predecessor )){
                //std::cout<<"Predecessor for node: "<<nodeID<<" changed to "<<messageNodeID<<std::endl;
                predecessor = messageNodeID;                
                predIP = messageNodeIP;
        }
}

void
GUChord::ProcessChordLeave (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort){

        Ipv4Address predecessorIP = message.GetChordLeave().predecessorAddress;
        Ipv4Address successorIP = message.GetChordLeave().successorAddress;
        std::string predecessorID = message.GetChordLeave().predecessorID;
        std::string successorID = message.GetChordLeave().successorID;

        

        if( m_mainAddress == successorIP ){
                std::cout<<"Successor notified of leave.  New Pred ID: "<<predecessorID<<std::endl;
                predecessor = predecessorID;
                predIP = predecessorIP;
        }else if( m_mainAddress == predecessorIP ){
                std::cout<<"Predecessor notified of leave. New Succ ID: "<<successorID<<std::endl;
                successor = successorID;
                succIP = successorIP;
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


