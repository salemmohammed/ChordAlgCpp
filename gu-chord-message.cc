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

#include "ns3/gu-chord-message.h"
#include "ns3/log.h"
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("GUChordMessage");
NS_OBJECT_ENSURE_REGISTERED (GUChordMessage);

GUChordMessage::GUChordMessage ()
{
}

GUChordMessage::~GUChordMessage ()
{
}

GUChordMessage::GUChordMessage (GUChordMessage::MessageType messageType, uint32_t transactionId)
{
  m_messageType = messageType;
  m_transactionId = transactionId;
}

TypeId 
GUChordMessage::GetTypeId (void)
{
  static TypeId tid = TypeId ("GUChordMessage")
    .SetParent<Header> ()
    .AddConstructor<GUChordMessage> ()
  ;
  return tid;
}

TypeId
GUChordMessage::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


uint32_t
GUChordMessage::GetSerializedSize (void) const
{
  // size of messageType, transaction id
  uint32_t size = sizeof (uint8_t) + sizeof (uint32_t);
  switch (m_messageType)
    {
      case PING_REQ:
        size += m_message.pingReq.GetSerializedSize ();
        break;
      case PING_RSP:
        size += m_message.pingRsp.GetSerializedSize ();
        break;
      case CHORD_JOIN:
        size += m_message.joinMessage.GetSerializedSize ();
        break;
      case CHORD_JOIN_RSP:
        size += m_message.joinResponse.GetSerializedSize ();
        break;
      case RING_STATE:
        size += m_message.rs.GetSerializedSize ();
        break;
      case STABLE_REQ:
        size += m_message.stableMessage.GetSerializedSize ();
        break;
      case STABLE_RSP:
        size += m_message.stableResponse.GetSerializedSize ();
        break;
      case SET_PRED:
        size += m_message.setPredMessage.GetSerializedSize ();
        break;
      case NOTIFY:
        size += m_message.notifyMessage.GetSerializedSize ();
        break;
      case CHORD_LEAVE:
        size += m_message.leaveMessage.GetSerializedSize ();
        break;
      case FINGERME_REQ:
        size += m_message.fingerReq.GetSerializedSize ();
        break;
      case FINGERME_RSP:
        size += m_message.fingerRsp.GetSerializedSize ();
        break;
      default:
        NS_ASSERT (false);
    }
  return size;
}

void
GUChordMessage::Print (std::ostream &os) const
{
  os << "\n****GUChordMessage Dump****\n" ;
  os << "messageType: " << m_messageType << "\n";
  os << "transactionId: " << m_transactionId << "\n";
  os << "PAYLOAD:: \n";
  
  switch (m_messageType)
    {
      case PING_REQ:
        m_message.pingReq.Print (os);
        break;
      case PING_RSP:
        m_message.pingRsp.Print (os);
        break;
      case CHORD_JOIN:
        m_message.joinMessage.Print (os);
        break;
      case CHORD_JOIN_RSP:
        m_message.joinResponse.Print (os);
        break;
      case RING_STATE:
        m_message.rs.Print (os);
        break;
      case STABLE_REQ:
        m_message.stableMessage.Print (os);
        break;
      case STABLE_RSP:
        m_message.stableResponse.Print (os);
        break;
      case SET_PRED:
        m_message.setPredMessage.Print (os);
        break;
      case NOTIFY:
        m_message.notifyMessage.Print (os);
        break;
      case CHORD_LEAVE:
        m_message.leaveMessage.Print (os);
        break;
      case FINGERME_REQ:
        m_message.fingerReq.Print (os);
        break;
      case FINGERME_RSP:
        m_message.fingerRsp.Print (os);
        break;
      default:
        break;  
    }
  os << "\n****END OF MESSAGE****\n";
}

void
GUChordMessage::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (m_messageType);
  i.WriteHtonU32 (m_transactionId);

  switch (m_messageType)
    {
      case PING_REQ:
        m_message.pingReq.Serialize (i);
        break;
      case PING_RSP:
        m_message.pingRsp.Serialize (i);
        break;
      case CHORD_JOIN:
        m_message.joinMessage.Serialize (i);
        break;
      case CHORD_JOIN_RSP:
        m_message.joinResponse.Serialize (i);
        break;
      case RING_STATE:
        m_message.rs.Serialize (i);
        break;
      case STABLE_REQ:
        m_message.stableMessage.Serialize (i);
        break;
      case STABLE_RSP:
        m_message.stableResponse.Serialize (i);
        break;
      case SET_PRED:
        m_message.setPredMessage.Serialize (i);
        break;
      case NOTIFY:
        m_message.notifyMessage.Serialize (i);
        break;
      case CHORD_LEAVE:
        m_message.leaveMessage.Serialize (i);
        break;
      case FINGERME_REQ:
        m_message.fingerReq.Serialize (i);
        break;
      case FINGERME_RSP:
        m_message.fingerRsp.Serialize (i);
        break;
      default:
        NS_ASSERT (false);   
    }
}

uint32_t 
GUChordMessage::Deserialize (Buffer::Iterator start)
{
  uint32_t size;
  Buffer::Iterator i = start;
  m_messageType = (MessageType) i.ReadU8 ();
  m_transactionId = i.ReadNtohU32 ();

  size = sizeof (uint8_t) + sizeof (uint32_t);

  switch (m_messageType)
    {
      case PING_REQ:
        size += m_message.pingReq.Deserialize (i);
        break;
      case PING_RSP:
        size += m_message.pingRsp.Deserialize (i);
        break;
      case CHORD_JOIN:
        size += m_message.joinMessage.Deserialize (i);
        break;
      case CHORD_JOIN_RSP:
        size += m_message.joinResponse.Deserialize (i);
        break;
      case RING_STATE:
        size += m_message.rs.Deserialize (i);
        break;
      case STABLE_REQ:
        size += m_message.stableMessage.Deserialize (i);
        break;
      case STABLE_RSP:
        size += m_message.stableResponse.Deserialize (i);
        break;
      case SET_PRED:
        size += m_message.setPredMessage.Deserialize (i);
        break;
      case NOTIFY:
        size += m_message.notifyMessage.Deserialize (i);
        break;
      case CHORD_LEAVE:
        size += m_message.leaveMessage.Deserialize (i);
        break;
      case FINGERME_REQ:
        size += m_message.fingerReq.Deserialize (i);
        break;
      case FINGERME_RSP:
        size += m_message.fingerRsp.Deserialize (i);
        break;
      default:
        NS_ASSERT (false);
    }
  return size;
}

/* PING_REQ */

uint32_t 
GUChordMessage::PingReq::GetSerializedSize (void) const
{
  uint32_t size;
  size = sizeof(uint16_t) + pingMessage.length();
  return size;
}

void
GUChordMessage::PingReq::Print (std::ostream &os) const
{
  os << "PingReq:: Message: " << pingMessage << "\n";
}

void
GUChordMessage::PingReq::Serialize (Buffer::Iterator &start) const
{
  start.WriteU16 (pingMessage.length ());
  start.Write ((uint8_t *) (const_cast<char*> (pingMessage.c_str())), pingMessage.length());
}

uint32_t
GUChordMessage::PingReq::Deserialize (Buffer::Iterator &start)
{  
  uint16_t length = start.ReadU16 ();
  char* str = (char*) malloc (length);
  start.Read ((uint8_t*)str, length);
  pingMessage = std::string (str, length);
  free (str);
  return PingReq::GetSerializedSize ();
}

void
GUChordMessage::SetPingReq (std::string pingMessage)
{
  if (m_messageType == 0)
    {
      m_messageType = PING_REQ;
    }
  else
    {
      NS_ASSERT (m_messageType == PING_REQ);
    }
  m_message.pingReq.pingMessage = pingMessage;
}

GUChordMessage::PingReq
GUChordMessage::GetPingReq ()
{
  return m_message.pingReq;
}

/* PING_RSP */

uint32_t 
GUChordMessage::PingRsp::GetSerializedSize (void) const
{
  uint32_t size;
  size = sizeof(uint16_t) + pingMessage.length();
  return size;
}

void
GUChordMessage::PingRsp::Print (std::ostream &os) const
{
  os << "PingReq:: Message: " << pingMessage << "\n";
}

void
GUChordMessage::PingRsp::Serialize (Buffer::Iterator &start) const
{
  start.WriteU16 (pingMessage.length ());
  start.Write ((uint8_t *) (const_cast<char*> (pingMessage.c_str())), pingMessage.length());
}

uint32_t
GUChordMessage::PingRsp::Deserialize (Buffer::Iterator &start)
{  
  uint16_t length = start.ReadU16 ();
  char* str = (char*) malloc (length);
  start.Read ((uint8_t*)str, length);
  pingMessage = std::string (str, length);
  free (str);
  return PingRsp::GetSerializedSize ();
}

void
GUChordMessage::SetPingRsp (std::string pingMessage)
{
  if (m_messageType == 0)
    {
      m_messageType = PING_RSP;
    }
  else
    {
      NS_ASSERT (m_messageType == PING_RSP);
    }
  m_message.pingRsp.pingMessage = pingMessage;
}

GUChordMessage::PingRsp
GUChordMessage::GetPingRsp ()
{
  return m_message.pingRsp;
}


//
//
//

/******                 CHORD JOIN METHODS                      ********/


uint32_t
GUChordMessage::ChordJoin::GetSerializedSize (void) const
{
  uint32_t size;
  size = (2*IPV4_ADDRESS_SIZE) + sizeof(uint16_t) + requesterID.length() + landmarkID.length();
  return 10*size;
}
void
GUChordMessage::ChordJoin::Print (std::ostream &os) const
{
  os << "ChordJoin::requesterID: " << requesterID << "\n";
}
void
GUChordMessage::ChordJoin::Serialize (Buffer::Iterator &start) const
{
  start.WriteU16 (requesterID.length ());
  start.Write ((uint8_t *) (const_cast<char*> (requesterID.c_str())), requesterID.length());

  start.WriteU16 (landmarkID.length ());
  start.Write ((uint8_t *) (const_cast<char*> (landmarkID.c_str())), landmarkID.length());

  start.WriteHtonU32 (originatorAddress.Get ());
  start.WriteHtonU32 (landmarkAddress.Get ());
  
        
}
uint32_t
GUChordMessage::ChordJoin::Deserialize (Buffer::Iterator &start)
{

  uint16_t length = start.ReadU16 ();
  char* str = (char*) malloc (length);
  start.Read ((uint8_t*)str, length);
  requesterID = std::string (str, length);
  free (str);

  uint16_t length2 = start.ReadU16 ();
  char* str2 = (char*) malloc (length2);
  start.Read ((uint8_t*)str2, length2);
  landmarkID = std::string (str2, length2);
  free (str2);

  originatorAddress = Ipv4Address (start.ReadNtohU32 ());
  landmarkAddress = Ipv4Address (start.ReadNtohU32 ());

  return ChordJoin::GetSerializedSize ();


}
void
GUChordMessage::SetChordJoin ( std::string rqID, std::string lmID, Ipv4Address originAddr, Ipv4Address landmarkAddr )
{
   if (m_messageType == 0)
      {
        m_messageType = CHORD_JOIN;
      }
   else
      {
        NS_ASSERT (m_messageType == CHORD_JOIN);
      }
        m_message.joinMessage.requesterID = rqID;
        m_message.joinMessage.landmarkID = lmID;
        m_message.joinMessage.originatorAddress = originAddr;
        m_message.joinMessage.landmarkAddress = landmarkAddr;
}

GUChordMessage::ChordJoin
GUChordMessage::GetChordJoin ()
{
  return m_message.joinMessage;
}



/************************       CHORD JOIN RESPONSE METHODS                  ******************************/


uint32_t
GUChordMessage::ChordJoinRsp::GetSerializedSize (void) const
{
  uint32_t size;
  size = (IPV4_ADDRESS_SIZE) + sizeof(uint16_t) + newSucc.length();
  return size;
}
void
GUChordMessage::ChordJoinRsp::Print (std::ostream &os) const
{
  os << "ChordJoinRsp::succ: "<< successorVal <<"\n";
}
void
GUChordMessage::ChordJoinRsp::Serialize (Buffer::Iterator &start) const
{
        start.WriteU16 (newSucc.length ());
        start.Write ((uint8_t *) (const_cast<char*> (newSucc.c_str())), newSucc.length());
        start.WriteHtonU32 (successorVal.Get ());
}
uint32_t
GUChordMessage::ChordJoinRsp::Deserialize (Buffer::Iterator &start)
{

  uint16_t length = start.ReadU16 ();
  char* str = (char*) malloc (length);
  start.Read ((uint8_t*)str, length);
  newSucc = std::string (str, length);
  free (str);

  successorVal = Ipv4Address (start.ReadNtohU32 ());
  return ChordJoinRsp::GetSerializedSize ();
}
void
GUChordMessage::SetChordJoinRsp ( std::string succVal, Ipv4Address succ)
{
   if (m_messageType == 0)
      {
        m_messageType = CHORD_JOIN_RSP;
      }
   else
      {
        NS_ASSERT (m_messageType == CHORD_JOIN_RSP);
      }
        m_message.joinResponse.newSucc = succVal;
        m_message.joinResponse.successorVal = succ;
}

GUChordMessage::ChordJoinRsp
GUChordMessage::GetChordJoinRsp ()
{
  return m_message.joinResponse;
}



/************************** RING STATE MESSAGE **********************************/


uint32_t
GUChordMessage::RingState::GetSerializedSize (void) const
{
  uint32_t size;
  size = sizeof(uint16_t) + originatorNodeID.length();
  return size;
}
void
GUChordMessage::RingState::Print (std::ostream &os) const
{
  os << "Ring State message \n";
}
void
GUChordMessage::RingState::Serialize (Buffer::Iterator &start) const
{
        start.WriteU16 (originatorNodeID.length ());
        start.Write ((uint8_t *) (const_cast<char*> (originatorNodeID.c_str())), originatorNodeID.length());
  
}
uint32_t
GUChordMessage::RingState::Deserialize (Buffer::Iterator &start)
{

  uint16_t length = start.ReadU16 ();
  char* str = (char*) malloc (length);
  start.Read ((uint8_t*)str, length);
  originatorNodeID = std::string (str, length);
  free (str);
  
  return RingState::GetSerializedSize ();
}
void
GUChordMessage::SetRingState ( std::string origin )
{
   if (m_messageType == 0)
      {
        m_messageType = RING_STATE;
      }
   else
      {
        NS_ASSERT (m_messageType == RING_STATE);
      }
        m_message.rs.originatorNodeID = origin;
}

GUChordMessage::RingState
GUChordMessage::GetRingState ()
{
  return m_message.rs;
}

//
//
//

/******                 STABILIZE REQUEST METHODS                      ********/


uint32_t
GUChordMessage::StableReq::GetSerializedSize (void) const
{
  uint32_t size;
  size = sizeof(uint16_t);
  return size;
}
void
GUChordMessage::StableReq::Print (std::ostream &os) const
{
  os << "StabilizeReq \n";
}
void
GUChordMessage::StableReq::Serialize (Buffer::Iterator &start) const
{
        
}
uint32_t
GUChordMessage::StableReq::Deserialize (Buffer::Iterator &start)
{

  return StableReq::GetSerializedSize ();

}
void
GUChordMessage::SetStableReq ()
{
   if (m_messageType == 0)
      {
        m_messageType = STABLE_REQ;
      }
   else
      {
        NS_ASSERT (m_messageType == STABLE_REQ);
      }
}

GUChordMessage::StableReq
GUChordMessage::GetStableReq ()
{
  return m_message.stableMessage;
}

/************************       STABILIZE RESPONSE METHODS                  ******************************/

uint32_t
GUChordMessage::StableRsp::GetSerializedSize (void) const
{
  uint32_t size;
  size = (IPV4_ADDRESS_SIZE) + sizeof(uint16_t) + predID.length();
  return size;
}
void
GUChordMessage::StableRsp::Print (std::ostream &os) const
{
  os << "StableRsp::pred: "<< predAddress << "\n";
}
void
GUChordMessage::StableRsp::Serialize (Buffer::Iterator &start) const
{

        start.WriteU16 (predID.length ());
        start.Write ((uint8_t *) (const_cast<char*> (predID.c_str())), predID.length());

        start.WriteHtonU32 (predAddress.Get ());
}
uint32_t
GUChordMessage::StableRsp::Deserialize (Buffer::Iterator &start)
{

        uint16_t length = start.ReadU16 ();
        char* str = (char*) malloc (length);
        start.Read ((uint8_t*)str, length);
        predID = std::string (str, length);
        free (str);

        predAddress = Ipv4Address (start.ReadNtohU32 ());

        return StableRsp::GetSerializedSize ();
}
void
GUChordMessage::SetStableRsp (std::string predId, Ipv4Address predIp)
{
   if (m_messageType == 0)
      {
        m_messageType = STABLE_RSP;
      }
   else
      {
        NS_ASSERT (m_messageType == STABLE_RSP);
      }
        m_message.stableResponse.predID = predId;
        m_message.stableResponse.predAddress = predIp;
}

GUChordMessage::StableRsp
GUChordMessage::GetStableRsp ()
{
  return m_message.stableResponse;
}


/************************************      SET PRED METHODS         ***************************/

uint32_t
GUChordMessage::SetPred::GetSerializedSize (void) const
{
  uint32_t size;
  size = (IPV4_ADDRESS_SIZE) + sizeof(uint16_t) + newPredID.length();
  return size;
}
void
GUChordMessage::SetPred::Print (std::ostream &os) const
{
  os << "SetPred\n";
}
void
GUChordMessage::SetPred::Serialize (Buffer::Iterator &start) const
{

        start.WriteU16 (newPredID.length ());
        start.Write ((uint8_t *) (const_cast<char*> (newPredID.c_str())), newPredID.length());

        start.WriteHtonU32 (newPredIP.Get ());
}
uint32_t
GUChordMessage::SetPred::Deserialize (Buffer::Iterator &start)
{

        uint16_t length = start.ReadU16 ();
        char* str = (char*) malloc (length);
        start.Read ((uint8_t*)str, length);
        newPredID = std::string (str, length);
        free (str);

        newPredIP = Ipv4Address (start.ReadNtohU32 ());

        return SetPred::GetSerializedSize ();
}
void
GUChordMessage::SetSetPred (std::string newPredId, Ipv4Address newPredIp)
{
   if (m_messageType == 0)
      {
        m_messageType = SET_PRED;
      }
   else
      {
        NS_ASSERT (m_messageType == SET_PRED);
      }
        m_message.setPredMessage.newPredID = newPredId;
        m_message.setPredMessage.newPredIP = newPredIp;
}

GUChordMessage::SetPred
GUChordMessage::GetSetPred ()
{
  return m_message.setPredMessage;
}


/************************************      NOTIFY METHODS             **************************/

uint32_t
GUChordMessage::Notify::GetSerializedSize (void) const
{
  uint32_t size;
  size = (IPV4_ADDRESS_SIZE) + sizeof(uint16_t) + potentialPredID.length();
  return size;
}
void
GUChordMessage::Notify::Print (std::ostream &os) const
{
  os << "Notify\n";
}
void
GUChordMessage::Notify::Serialize (Buffer::Iterator &start) const
{

        start.WriteU16 (potentialPredID.length ());
        start.Write ((uint8_t *) (const_cast<char*> (potentialPredID.c_str())), potentialPredID.length());

        start.WriteHtonU32 (potentialPredIP.Get ());
}
uint32_t
GUChordMessage::Notify::Deserialize (Buffer::Iterator &start)
{

        uint16_t length = start.ReadU16 ();
        char* str = (char*) malloc (length);
        start.Read ((uint8_t*)str, length);
        potentialPredID = std::string (str, length);
        free (str);

        potentialPredIP = Ipv4Address (start.ReadNtohU32 ());

        return Notify::GetSerializedSize ();
}
void
GUChordMessage::SetNotify (std::string potPredId, Ipv4Address potPredIp)
{
   if (m_messageType == 0)
      {
        m_messageType = NOTIFY;
      }
   else
      {
        NS_ASSERT (m_messageType == NOTIFY);
      }
        m_message.notifyMessage.potentialPredID = potPredId;
        m_message.notifyMessage.potentialPredIP = potPredIp;
}

GUChordMessage::Notify
GUChordMessage::GetNotify ()
{
  return m_message.notifyMessage;
}



/************************************      CHORD LEAVE METHODS      ****************************/

uint32_t
GUChordMessage::ChordLeave::GetSerializedSize (void) const
{
  uint32_t size;
  size = (2*IPV4_ADDRESS_SIZE) + sizeof(uint16_t) + successorID.length() + predecessorID.length();
  return 10*size;
}
void
GUChordMessage::ChordLeave::Print (std::ostream &os) const
{
  os << "ChordJoin\n";
}
void
GUChordMessage::ChordLeave::Serialize (Buffer::Iterator &start) const
{

  start.WriteU16 (successorID.length ());
  start.Write ((uint8_t *) (const_cast<char*> (successorID.c_str())), successorID.length());
  
  start.WriteU16 (predecessorID.length ());
  start.Write ((uint8_t *) (const_cast<char*> (predecessorID.c_str())), predecessorID.length());

  start.WriteHtonU32 (successorAddress.Get ());
  start.WriteHtonU32 (predecessorAddress.Get ());
        
}
uint32_t
GUChordMessage::ChordLeave::Deserialize (Buffer::Iterator &start)
{

        uint16_t length = start.ReadU16 ();
        char* str = (char*) malloc (length);
        start.Read ((uint8_t*)str, length);
        successorID = std::string (str, length);
        free (str);

        uint16_t length2 = start.ReadU16 ();
        char* str2 = (char*) malloc (length2);
        start.Read ((uint8_t*)str2, length2);
        predecessorID = std::string (str, length);
        free (str);

        successorAddress = Ipv4Address (start.ReadNtohU32 ());
        predecessorAddress = Ipv4Address (start.ReadNtohU32 ());
          
        return ChordLeave::GetSerializedSize ();
}
void
GUChordMessage::SetChordLeave ( Ipv4Address successor, Ipv4Address predecessor, std::string sId, std::string pId )
{
   if (m_messageType == 0)
      {
        m_messageType = CHORD_LEAVE;
      }
   else
      {
        NS_ASSERT (m_messageType == CHORD_LEAVE);
      }
        m_message.leaveMessage.successorID = sId;
        m_message.leaveMessage.predecessorID = pId;
        m_message.leaveMessage.successorAddress = successor;
        m_message.leaveMessage.predecessorAddress = predecessor;
}

GUChordMessage::ChordLeave
GUChordMessage::GetChordLeave ()
{
  return m_message.leaveMessage;
}

/********************************      FINGER REQ       **************************************/

uint32_t
GUChordMessage::FingerReq::GetSerializedSize (void) const
{

  uint32_t size;
  size = sizeof(uint16_t) + (IPV4_ADDRESS_SIZE);
  size += sizeof(uint32_t);

  for (std::vector<std::string>::const_iterator it = testIdentifiers.begin(); it != testIdentifiers.end(); it++) {
    size += sizeof(uint16_t);
    size += (*it).length();
  }

  for (std::vector<std::string>::const_iterator it = fingerEntries.begin(); it != fingerEntries.end(); it++) {
    size += sizeof(uint16_t);
    size += (*it).length();
  }
  
  for (std::vector<Ipv4Address>::const_iterator it = fingerIps.begin(); it != fingerIps.end(); it++) {
    size += sizeof(uint16_t) + (IPV4_ADDRESS_SIZE);
  }

  return 5*size;
}
void
GUChordMessage::FingerReq::Print (std::ostream &os) const
{
  os << "FingerReq\n";
}
void
GUChordMessage::FingerReq::Serialize (Buffer::Iterator &start) const
{
        start.WriteHtonU32 (originatorNode.Get ());  
        
        start.WriteHtonU32(testIdentifiers.size());
  
        for (std::vector<std::string>::const_iterator it = testIdentifiers.begin(); it != testIdentifiers.end(); it++) {
        start.WriteU16 ((*it).length());
        start.Write ((uint8_t *) (const_cast<char*> ((*it).c_str())), (*it).length());
        }
        
        start.WriteHtonU32(fingerEntries.size());
  
        for (std::vector<std::string>::const_iterator it = fingerEntries.begin(); it != fingerEntries.end(); it++) {
        start.WriteU16 ((*it).length());
        start.Write ((uint8_t *) (const_cast<char*> ((*it).c_str())), (*it).length());
        }
        
        start.WriteHtonU32(fingerIps.size());
  
        for (std::vector<Ipv4Address>::const_iterator it = fingerIps.begin(); it != fingerIps.end(); it++) {
        start.WriteHtonU32 ((*it).Get ());
        }
}
uint32_t
GUChordMessage::FingerReq::Deserialize (Buffer::Iterator &start)
{
        originatorNode = Ipv4Address (start.ReadNtohU32 ());
        
        uint32_t dlen = start.ReadNtohU32();
          for (uint32_t i = 0; i < dlen; i++) {
            uint16_t length = start.ReadU16 ();
            char* str = (char*) malloc (length);
            start.Read ((uint8_t*)str, length);
            testIdentifiers.push_back(std::string (str, length));
            free (str);
          }

        uint32_t dlen2 = start.ReadNtohU32();
          for (uint32_t i = 0; i < dlen2; i++) {
            uint16_t length = start.ReadU16 ();
            char* str = (char*) malloc (length);
            start.Read ((uint8_t*)str, length);
            fingerEntries.push_back(std::string (str, length));
            free (str);
          }
        
        uint32_t dlen3 = start.ReadNtohU32();
          for (uint32_t i = 0; i < dlen3; i++) {
                fingerIps.push_back(Ipv4Address (start.ReadNtohU32 ()));            
          }

        return FingerReq::GetSerializedSize ();
}
void
GUChordMessage::SetFingerReq (std::vector<std::string> testIds, std::vector<std::string> fingerEntries, std::vector<Ipv4Address> fingerIP, Ipv4Address originator)
{
   if (m_messageType == 0)
      {
        m_messageType = FINGERME_REQ;
      }
   else
      {
        NS_ASSERT (m_messageType == FINGERME_REQ);
      }        
        m_message.fingerReq.originatorNode = originator;
        m_message.fingerReq.testIdentifiers = testIds;
        m_message.fingerReq.fingerEntries = fingerEntries;
        m_message.fingerReq.fingerIps = fingerIP;
}

GUChordMessage::FingerReq
GUChordMessage::GetFingerReq ()
{
  return m_message.fingerReq;
}


/**********************************      FINGER RSP     ************************************/


uint32_t
GUChordMessage::FingerRsp::GetSerializedSize (void) const
{
  uint32_t size;
  size = sizeof(uint16_t);
  size += sizeof(uint32_t);

  for (std::vector<std::string>::const_iterator it = fingerID.begin(); it != fingerID.end(); it++) {
    size += sizeof(uint16_t);
    size += (*it).length();
  }

  for (std::vector<Ipv4Address>::const_iterator it = fingerAddress.begin(); it != fingerAddress.end(); it++) {
    size += sizeof(uint16_t) + (IPV4_ADDRESS_SIZE);
  }

  return 5*size;
}
void
GUChordMessage::FingerRsp::Print (std::ostream &os) const
{
  os << "FingerRsq\n";
}
void
GUChordMessage::FingerRsp::Serialize (Buffer::Iterator &start) const
{

        start.WriteHtonU32(fingerID.size());
  
        for (std::vector<std::string>::const_iterator it = fingerID.begin(); it != fingerID.end(); it++) {
        start.WriteU16 ((*it).length());
        start.Write ((uint8_t *) (const_cast<char*> ((*it).c_str())), (*it).length());
        }

        start.WriteHtonU32(fingerAddress.size());
  
        for (std::vector<Ipv4Address>::const_iterator it = fingerAddress.begin(); it != fingerAddress.end(); it++) {
        start.WriteHtonU32 ((*it).Get ());
        }
}
uint32_t
GUChordMessage::FingerRsp::Deserialize (Buffer::Iterator &start)
{

        uint32_t dlen2 = start.ReadNtohU32();
          for (uint32_t i = 0; i < dlen2; i++) {
            uint16_t length = start.ReadU16 ();
            char* str = (char*) malloc (length);
            start.Read ((uint8_t*)str, length);
            fingerID.push_back(std::string (str, length));
            free (str);
          }

        uint32_t dlen3 = start.ReadNtohU32();
          for (uint32_t i = 0; i < dlen3; i++) {
                fingerAddress.push_back(Ipv4Address (start.ReadNtohU32 ()));            
          }

        return FingerRsp::GetSerializedSize ();
}
void
GUChordMessage::SetFingerRsp (std::vector<std::string> fingerNum, std::vector<Ipv4Address> fingerAddr)
{
   if (m_messageType == 0)
      {
        m_messageType = FINGERME_RSP;
      }
   else
      {
        NS_ASSERT (m_messageType == FINGERME_RSP);
      }
        m_message.fingerRsp.fingerID = fingerNum;
        m_message.fingerRsp.fingerAddress = fingerAddr;
}

GUChordMessage::FingerRsp
GUChordMessage::GetFingerRsp ()
{
  return m_message.fingerRsp;
}

/***************************************************************/

void
GUChordMessage::SetMessageType (MessageType messageType)
{
  m_messageType = messageType;
}

GUChordMessage::MessageType
GUChordMessage::GetMessageType () const
{
  return m_messageType;
}

void
GUChordMessage::SetTransactionId (uint32_t transactionId)
{
  m_transactionId = transactionId;
}

uint32_t 
GUChordMessage::GetTransactionId (void) const
{
  return m_transactionId;
}

