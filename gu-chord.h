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

#ifndef GU_CHORD_H
#define GU_CHORD_H

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
#include "ns3/finger.h"

using namespace ns3;

class GUChord : public GUApplication
{
  public:
    static TypeId GetTypeId (void);
    GUChord ();
    virtual ~GUChord ();

    void SendPing (Ipv4Address destAddress, std::string pingMessage);
    void RecvMessage (Ptr<Socket> socket);

    std::string GetNodeNumber();
    Ipv4Address GetMainInterface ();   //retrieve device address
    std::string getNodeID(Ipv4Address addr);              //Compute Hash Value
    std::string getFingerBound( Ipv4Address addr, uint32_t i );
    void SetSelfToLandmark();   


    void SendJoinRequest(Ipv4Address destAddress, Ipv4Address srcAddress, std::string srcId, Ipv4Address landmarkAddress, std::string landmarkId);    //Method to send out join message to landmark node
    void SendJoinResponse(Ipv4Address destAddress, Ipv4Address succ, std::string newSuccessor);   //Method to send back the correct pred and succ to join requester
    void SendRingStateMessage(Ipv4Address destAddress, std::string srcNodeID);
    void startSendingStableReq();
    void SendStableReq(Ipv4Address destAddress);
    void SendStableRsp(Ipv4Address destAddress, std::string predecessorId, Ipv4Address predecessorIp);
    void SendSetPred(Ipv4Address destAddress, std::string ndId, Ipv4Address ndAddr);
    void SendNotify(Ipv4Address destAddress, std::string ndId, Ipv4Address ndAddr);
    void SendLeaveRequest(Ipv4Address destAddress, Ipv4Address succ, Ipv4Address pred, std::string sucIp, std::string predIp);
    void SendFingerReq(Ipv4Address destAddress, std::vector<std::string> testIds, std::vector<std::string> fingerEntries, std::vector<Ipv4Address> fingerIP, Ipv4Address originator);
    void SendFingerRsp(Ipv4Address destAddress, std::vector<std::string> fingerNum, std::vector<Ipv4Address> fingerAddr);                      

    void ProcessPingReq (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);
    void ProcessPingRsp (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);
    void ProcessChordJoin (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);      //process message for joining network
    void ProcessChordJoinRsp (GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);          //Process message when node in network finds the correct succ. and pred. for a join request
    void PrintRingState(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);
    void ProcessStableReq(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);
    void ProcessStableRsp(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);
    void ProcessSetPred(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);
    void ProcessNotify(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);
    void ProcessChordLeave(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);
    void ProcessFingerReq(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);
    void ProcessFingerRsp(GUChordMessage message, Ipv4Address sourceAddress, uint16_t sourcePort);

    void AuditPings ();
    uint32_t GetNextTransactionId ();
    void StopChord ();

    // Callback with Application Layer (add more when required)
    void SetPingSuccessCallback (Callback <void, Ipv4Address, std::string> pingSuccessFn);
    void SetPingFailureCallback (Callback <void, Ipv4Address, std::string> pingFailureFn);
    void SetPingRecvCallback (Callback <void, Ipv4Address, std::string> pingRecvFn);

    // From GUApplication
    virtual void ProcessCommand (std::vector<std::string> tokens);

    
  protected:
    virtual void DoDispose ();
    
  private:
    virtual void StartApplication (void);
    virtual void StopApplication (void);

    uint32_t m_currentTransactionId;
    Ptr<Socket> m_socket;
    Time m_pingTimeout;
    Time m_sendStableTimeout;
    
    uint16_t m_appPort;
    // Timers
    Timer m_auditPingsTimer;
    Timer m_sendStableTimer;
    // Ping tracker
    std::map<uint32_t, Ptr<PingRequest> > m_pingTracker;
    // Callbacks
    Callback <void, Ipv4Address, std::string> m_pingSuccessFn;
    Callback <void, Ipv4Address, std::string> m_pingFailureFn;
    Callback <void, Ipv4Address, std::string> m_pingRecvFn;

    Ipv4Address m_mainAddress;
    Ipv4Address succIP;
    Ipv4Address predIP;
    std::string successor;      //next node
    std::string predecessor;    //previous node
    std::string nodeID;      //Computed ID
    std::vector<std::string> fingerTestVals;
    std::vector<Finger> fingerTable;    //Finger Table
};

#endif


