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
  size = IPV4_ADDRESS_SIZE + sizeof(uint16_t) + requesterID.length();
  return size;
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
  start.WriteHtonU32 (originatorAddress.Get ());
        
}
uint32_t
GUChordMessage::ChordJoin::Deserialize (Buffer::Iterator &start)
{

  uint16_t length = start.ReadU16 ();
  char* str = (char*) malloc (length);
  start.Read ((uint8_t*)str, length);
  requesterID = std::string (str, length);
  free (str);

  originatorAddress = Ipv4Address (start.ReadNtohU32 ());
  return ChordJoin::GetSerializedSize ();


}
void
GUChordMessage::SetChordJoin ( std::string rqID, Ipv4Address originAddr )
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
        m_message.joinMessage.originatorAddress = originAddr;
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
  size = (2*IPV4_ADDRESS_SIZE) + sizeof(uint16_t);
  return size;
}
void
GUChordMessage::ChordJoinRsp::Print (std::ostream &os) const
{
  os << "ChordJoinRsp::succ: "<< successorVal <<" pred: " << predecessorVal <<"\n";
}
void
GUChordMessage::ChordJoinRsp::Serialize (Buffer::Iterator &start) const
{
        start.WriteHtonU32 (successorVal.Get ());
        start.WriteHtonU32 (predecessorVal.Get ());
}
uint32_t
GUChordMessage::ChordJoinRsp::Deserialize (Buffer::Iterator &start)
{

  successorVal = Ipv4Address (start.ReadNtohU32 ());
  predecessorVal = Ipv4Address (start.ReadNtohU32 ());
  return ChordJoinRsp::GetSerializedSize ();
}
void
GUChordMessage::SetChordJoinRsp ( Ipv4Address succ, Ipv4Address pred )
{
   if (m_messageType == 0)
      {
        m_messageType = CHORD_JOIN_RSP;
      }
   else
      {
        NS_ASSERT (m_messageType == CHORD_JOIN_RSP);
      }
        m_message.joinResponse.successorVal = succ;
        m_message.joinResponse.predecessorVal = pred;
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



/********************************************************************/

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

