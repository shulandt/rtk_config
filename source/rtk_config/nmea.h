#ifndef NMEA_H
#define NMEA_H

#define ERR            -1
#define NMEA_BUFFSIZE  130

//-----------------------------------------------------------------------------
enum {_UNDEF_ = 1, _P_RZA_, _P_RZB_, _P_VER_, _P_NME_, _P_RTK_, _P_CFG_};

//-----------------------------------------------------------------------------
typedef struct
{
 char          Item[40][20];
 unsigned char Fields;
}NMEAMessage;

//-----------------------------------------------------------------------------
class NMEA
{
 public:
  NMEAMessage   P_RZAmsg, P_RZBmsg, P_VERmsg, P_NMEmsg, P_RTKmsg, P_CFGmsg;
  NMEA();
  void          Init();
  int           Decode(char Byte);
  unsigned char GetFields() {return (Rmsg.Fields - 1);}
  char*         GetItem(unsigned char N){return Rmsg.Item[N + 1];}
 private:
  char          RBuff[NMEA_BUFFSIZE];
  unsigned char RPosition;
  bool          FoundBegin;
  NMEAMessage   Rmsg;
  int           Extract();
  int           MessageType();
  bool          CheckSum();
};

//-----------------------------------------------------------------------------

#endif // NMEA_H
