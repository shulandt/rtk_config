#include "nmea.h"
#include <string.h>
#include <stdio.h>

//---------------------------------------------------------------------------
NMEA::NMEA()
{
 Init();
}
//---------------------------------------------------------------------------
void NMEA::Init()
{
 RPosition  = 0;
 FoundBegin = false;
}
//---------------------------------------------------------------------------
int NMEA::Decode (char Byte)
{
 if(Byte == '$')
 {
  FoundBegin = true;
  RPosition  = 0;
 }
 if(FoundBegin)
 {
  RBuff[RPosition] = Byte;
  RPosition++;
  if(RPosition == NMEA_BUFFSIZE)
  {
   FoundBegin = false;
   return ERR;
  }
  if(Byte == 0x0A)
  {
   FoundBegin = false;
   if(!CheckSum())
    return ERR;
   return Extract();
  }
 }

 return 0;
}

//---------------------------------------------------------------------------
int NMEA::Extract()
{
 int i   = 0;
 int Len = 0;

 Rmsg.Fields = 0;
 while((RBuff[i] != '*') && (i < NMEA_BUFFSIZE))
 {
  if(RBuff[i] == ',')
  {
   RBuff[i] = 0;
   Rmsg.Fields++;
  }
  i++;
 }
 RBuff[i] = 0;
 Rmsg.Fields++;
 if(Rmsg.Fields >= 40)
  return ERR;
 for(i = 0; i < Rmsg.Fields; i++)
 {
  strcpy(Rmsg.Item[i], RBuff + 1 + Len);
  Len += strlen(Rmsg.Item[i]) + 1;
 }

 return MessageType();
}

//---------------------------------------------------------------------------
bool NMEA::CheckSum()
{
 char Sum = 0;
 char StrSum[3];

 //if (RBuff[RPosition - 5] != '*')
 // return true;
 for (int i = 1; i < (RPosition - 5); i++)
  Sum ^= RBuff[i];
 sprintf(StrSum, "%02X", Sum);
 if (strncmp(StrSum, RBuff + RPosition - 4, 2))
  return false;
 return true;
}

//---------------------------------------------------------------------------
int NMEA::MessageType()
{
 if(!strncmp(Rmsg.Item[0], "PNVGRZA", 7))
 {
  memcpy(&P_RZAmsg, &Rmsg, sizeof(NMEAMessage));
  return _P_RZA_;
 }
 if(!strncmp(Rmsg.Item[0], "PNVGRZB", 7))
 {
  memcpy(&P_RZBmsg, &Rmsg, sizeof(NMEAMessage));
  return _P_RZB_;
 }
 if(!strncmp(Rmsg.Item[0], "PNVGVER", 7))
 {
  memcpy(&P_VERmsg, &Rmsg, sizeof(NMEAMessage));
  return _P_VER_;
 }
 if(!strncmp(Rmsg.Item[0], "PNVGNME", 7))
 {
  memcpy(&P_NMEmsg, &Rmsg, sizeof(NMEAMessage));
  return _P_NME_;
 }
 if(!strncmp(Rmsg.Item[0], "PNVGRTK", 7))
 {
  memcpy(&P_RTKmsg, &Rmsg, sizeof(NMEAMessage));
  return _P_RTK_;
 }
 if(!strncmp(Rmsg.Item[0], "PNVGCFG", 7))
 {
  memcpy(&P_CFGmsg, &Rmsg, sizeof(NMEAMessage));
  return _P_CFG_;
 }

 return _UNDEF_;
}

//---------------------------------------------------------------------------

