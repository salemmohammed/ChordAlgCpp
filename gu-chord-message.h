/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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

#ifndef GU_CHORD_MESSAGE_H
#define GU_CHORD_MESSAGE_H

#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/packet.h"
#include "ns3/object.h"
#include <vector>

using namespace ns3;

#define IPV4_ADDRESS_SIZE 4

class GUChordMessage : public Header
{
  public:
    GUChordMessage ();
    virtual ~GUChordMessage ();


    enum MessageType
      {
        PING_REQ = 1,
        PING_RSP = 2,
        CHORD_JOIN = 3,
        CHORD_JOIN_RSP = 4,
        RING_STATE = 5,
        STABLE_REQ = 6,
        STABLE_RSP = 7,
        SET_PRED = 8,
        NOTIFY = 9,
        CHORD_LEAVE = 10,
        FINGERME_REQ = 11,
        FINGERME_RSP = 12,    
      };

    GUChordMessage (GUChordMessage::MessageType messageType, uint32_t transactionId);

    /**
    *  \brief Sets message type
    *  \param messageType message type
    */
    void SetMessageType (MessageType messageType);

    /**
     *  \returns message type
     */
    MessageType GetMessageType () const;

    /**
     *  \brief Sets Transaction Id
     *  \param transactionId Transaction Id of the request
     */
    void SetTransactionId (uint32_t transactionId);

    /**
     *  \returns Transaction Id
     */
    uint32_t GetTransactionId () const;

  private:
    /**
     *  \cond
     */
    MessageType m_messageType;
    uint32_t m_transactionId;
    /**
     *  \endcond
     */
  public:
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;
    void Print (std::ostream &os) const;
    uint32_t GetSerializedSize (void) const;
    void Serialize (Buffer::Iterator start) const;
    uint32_t Deserialize (Buffer::Iterator start);

    
    struct PingReq
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
        // Payload
        std::string pingMessage;
      };

    struct PingRsp
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
        // Payload
        std::string pingMessage;
      };
    struct ChordJoin
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
        // Payload
        std::string requesterID;
        std::string landmarkID;
        Ipv4Address originatorAddress;
        Ipv4Address landmarkAddress;
        
      };
    struct ChordJoinRsp
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
        //Payload
        std::string newSucc;
        Ipv4Address successorVal;
      };
    struct RingState
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
        std::string originatorNodeID;
      };
    struct StableReq
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
      };
    struct StableRsp
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
        //Payload
        std::string predID;
        Ipv4Address predAddress;
      };
    struct SetPred
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
        //Payload
        std::string newPredID;
        Ipv4Address newPredIP;
      };
    struct Notify
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
        //Payload
        std::string potentialPredID;
        Ipv4Address potentialPredIP;
      };    
    struct ChordLeave
      {
        void Print (std::ostream &os) const;
        uint32_t GetSerializedSize (void) const;
        void Serialize (Buffer::Iterator &start) const;
        uint32_t Deserialize (Buffer::Iterator &start);
        //Payload
        std::string successorID;
        std::string predecessorID;        
        Ipv4Address successorAddress;
        Ipv4Address predecessorAddress;
        
      };
    struct FingerReq
        {
          void Print (std::ostream &os) const;
          uint32_t GetSerializedSize (void) const;
          void Serialize (Buffer::Iterator &start) const;
          uint32_t Deserialize (Buffer::Iterator &start);
          //Payload
          Ipv4Address originatorNode;
          std::vector<std::string> testIdentifiers;
          std::vector<std::string> fingerEntries;
          std::vector<Ipv4Address> fingerIps;
          
        };
    struct FingerRsp
        {
          void Print (std::ostream &os) const;
          uint32_t GetSerializedSize (void) const;
          void Serialize (Buffer::Iterator &start) const;
          uint32_t Deserialize (Buffer::Iterator &start);
          //Payload
          std::vector<std::string> fingerID;
          std::vector<Ipv4Address> fingerAddress;
        };



  private:
    struct
      {
        PingReq pingReq;
        PingRsp pingRsp;
        ChordJoin joinMessage;
        ChordJoinRsp joinResponse;
        RingState rs;
        StableReq stableMessage;
        StableRsp stableResponse;
        SetPred setPredMessage;
        Notify notifyMessage;
        ChordLeave leaveMessage;
        FingerReq fingerReq;
        FingerRsp fingerRsp;
      } m_message;
    
  public:
    /**
     *  \returns PingReq Struct
     */
    PingReq GetPingReq ();

    /**
     *  \brief Sets PingReq message params
     *  \param message Payload String
     */

    void SetPingReq (std::string message);

    /**
     * \returns PingRsp Struct
     */
    PingRsp GetPingRsp ();
    /**
     *  \brief Sets PingRsp message params
     *  \param message Payload String
     */
    void SetPingRsp (std::string message);

    //Get & Set for ChordJoin
    
    ChordJoin GetChordJoin ();
   
    void SetChordJoin (std::string rqID, std::string lmID, Ipv4Address originAddr, Ipv4Address landmarkAddr);

    ChordJoinRsp GetChordJoinRsp ();
    
    void SetChordJoinRsp (std::string succVal, Ipv4Address succ);
        
    RingState GetRingState ();
        
    void SetRingState (std::string origin);
   
    StableReq GetStableReq ();

    void SetStableReq ();

    StableRsp GetStableRsp ();

    void SetStableRsp (std::string predId, Ipv4Address predIp);

    SetPred GetSetPred ();
    
    void SetSetPred (std::string newPredId, Ipv4Address newPredIp);

    Notify GetNotify ();
        
    void SetNotify (std::string potPredId, Ipv4Address potPredIp);

    ChordLeave GetChordLeave ();
        
    void SetChordLeave (std::string sId, std::string pId, Ipv4Address successor, Ipv4Address predecessor);

    FingerReq GetFingerReq ();
        
    void SetFingerReq (std::vector<std::string> testIds, std::vector<std::string> fingerEntries, std::vector<Ipv4Address> fingerIP, Ipv4Address originator);

    FingerRsp GetFingerRsp ();
        
    void SetFingerRsp (std::vector<std::string> fingerNum, std::vector<Ipv4Address> fingerAddr);
    



}; // class GUChordMessage

static inline std::ostream& operator<< (std::ostream& os, const GUChordMessage& message)
{
  message.Print (os);
  return os;
}

#endif
