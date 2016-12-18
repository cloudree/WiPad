/*
  MSX Joystick from USBHIDJoystick PC
  modified by cloudree

  2016.12.11 : remove XBOX,PS3 (wireless)
  2016.10.16 : arduino 1.6.10 + USB host shield library 2.0 : circuit@home.org
  2016.07.30 : mouse support
  MSX mouse specs are ...
    https://www.msx.org/wiki/Mouse/Trackball
    http://www.faq.msxnet.org/connector.html
    https://www.msx.org/forum/msx-talk/hardware/use-10eu-connect-modern-mouse-msx
*/

/*
  Example sketch for the PS3 Bluetooth library - developed by Kristian Lauszus
  For more information visit my blog: http://blog.tkjelectronics.dk/ or
  send me an e-mail:  kristianl@tkjelectronics.com
*/
/*
  Example sketch for the Xbox Wireless Reciver library - developed by Kristian Lauszus
  It supports up to four controllers wirelessly
  For more information see the blog post: http://blog.tkjelectronics.dk/2012/12/xbox-360-receiver-added-to-the-usb-host-library/ or
  send me an e-mail:  kristianl@tkjelectronics.com
*/
/////////////////////////////
//  PS/2-mouse to MSX v1.1 //
//  Made By: NYYRIKKI 2014 //
/////////////////////////////

// project setting
#define ATMEL_328     // Uno, etc...
// #define _DEBUG

#include <hid.h>
#include <hiduniversal.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
# include <spi4teensy3.h>
# include <SPI.h>
#endif

// USB Host Shield pin out
// D8 = ??
// D9 = INT
// D10 = SS
// D11 = SPI
// D12 = INT
// D13 = SS

const int PIN_UP      = 2;    // Up
const int PIN_DOWN    = 3;    // Down
const int PIN_LEFT    = 4;    // Left
const int PIN_RIGHT   = 5;    // Right
const int PIN_START   = 6;    // Start
const int PIN_SELECT  = 7;    // Select
const int PIN_OUT     = 8;    // Port Out

const int PIN_A       = A5;    // A
const int PIN_B       = A4;    // B
const int PIN_C       = A3;    // C / L
const int PIN_X       = A2;    // X
const int PIN_Y       = A1;    // Y
const int PIN_Z       = A0;    // Z / R

volatile int nJoyMouse = 0;      // 0 = undefined, 1 = JoyStick, 2 = Mouse

char x, y;
char TimeOut = 40;
long time;
volatile char rx, ry, rz;
volatile bool pressLeft, pressRight;

volatile bool isUp, isDown, isLeft, isRight;
volatile bool isA, isB, isC, isX, isY, isZ;
volatile bool isStart, isSelect;

// ----------------------------------
#ifdef _DEBUG
void DBG( String msg )
{
  Serial.print( msg );
}

void DBG( byte data )
{
  Serial.print( data, HEX );
  Serial.println( " " );
}

void DBGLN( String msg )
{
  Serial.println( msg );
}

void DBGLN( byte data )
{
  Serial.print( data, HEX );
  Serial.println( " " );
}

void Dump( uint8_t len, uint8_t *buf)
{
  Serial.print("len = ");
  Serial.println( len );
  for (int i=0; i<len; i++ ) {
    Serial.print( buf[i], HEX);
    Serial.print( ", ");
  }
  Serial.println( "\n");
}
#else
# define DBG(a)
# define DBGLN(a)
# define Dump(a,b)
#endif

// ----------------------------------

void ProcessMouse( uint8_t len, uint8_t *buf)
{
  if( nJoyMouse == 1 )
    return;
  switch( len ) {
  case 3:
    if( buf[0] == 1 || buf[0] == 2 || buf[0] == 3 || buf[2] > 0x10 ) {
      nJoyMouse = 2;
    }
    if( nJoyMouse == 2 ) {
      rx = (byte) buf[1];
      if ( rx ) {
        DBG( "rx=" );
        DBGLN( rx );
      }
      ry = (byte) buf[2];
      if ( ry ) {
        DBG( "ry=" );
        DBGLN( ry );
      }
      pressLeft = buf[0] & 1;     if ( pressLeft) DBGLN( "Left" );
      pressRight = buf[0] & 2;    if ( pressRight) DBGLN( "Right" );
    }    
    break;
  case 4:
    if( buf[0] == 1 || buf[0] == 2 || buf[0] == 3 || buf[2] > 0x10 || buf[3] == 0xff ) {
      nJoyMouse = 2;
    }
    if( nJoyMouse == 2 ) {
      rx = (byte) buf[1];
      if ( rx ) {
        DBG( "rx=" );
        DBGLN( rx );
      }
      ry = (byte) buf[2];
      if ( ry ) {
        DBG( "ry=" );
        DBGLN( ry );
      }
      rz = (byte) buf[3];
      if ( rz ) {
        DBG( "rz=" );
        DBGLN( rz );
      }
      pressLeft = buf[0] & 1;     if ( pressLeft) DBGLN( "Left" );
      pressRight = buf[0] & 2;    if ( pressRight) DBGLN( "Right" );
    }
    break;
  }
}

void ProcessJoyStick( uint8_t len, uint8_t *buf)
{
  if( nJoyMouse == 2 )
    return;
  if( len > 4 ) 
    nJoyMouse = 1;
    
  switch( len ) 
  {
  case 4 :
    if( buf[0] == 0xff || buf[0] == 0x80 || buf[1] == 0x80 ) {
      nJoyMouse = 1;
    }
    if( nJoyMouse == 1 ) {
      // painkiller rotary joystick
      DBG("PK-Rotary ");
      isUp =    ( buf[1] == 0x00 );     if ( isUp ) DBGLN("UP");
      isRight = ( buf[0] == 0xff );     if ( isRight ) DBGLN("RT");
      isDown =  ( buf[1] == 0xff );     if ( isDown ) DBGLN("DN");
      isLeft =  ( buf[0] == 0x00 );     if ( isLeft ) DBGLN("LT");
      isA = ( buf[2] & 0x01 );   if ( isA ) DBGLN("A");
      isB = ( buf[2] & 0x02 );   if ( isB ) DBGLN("B");
      //isC = ( buf[2] & 0x04 );   if ( isC ) DBGLN("C");
      isX = ( buf[2] & 0x04 );   if ( isX ) DBGLN("X");
      isY = ( buf[2] & 0x08 );   if ( isY ) DBGLN("Y");
      //isZ = ( buf[3] & 0x08 );   if ( isZ ) DBGLN("Z");
      isStart = ( buf[3] & 0x04 );    if ( isStart ) DBGLN("Start");
      isSelect = ( buf[3] & 0x08 );   if ( isSelect ) DBGLN("Select");
    }
    break;
    
  case 7 :
    if ( (buf[2] & 0xf0) == 0xf0 && buf[5] == 0x7f && buf[6] == 0x7f ) {
      // PS2 to USB Conveter (Dual Player) : not analog
      if ( buf[0] != 1 ) return;
      // player 1
      DBG("PS2Dual ");
      isUp =    ( buf[4] == 0x00 );     if ( isUp ) DBGLN("UP");
      isRight = ( buf[3] == 0xff );     if ( isRight ) DBGLN("RT");
      isDown =  ( buf[4] == 0xff );     if ( isDown ) DBGLN("DN");
      isLeft =  ( buf[3] == 0x00 );     if ( isLeft ) DBGLN("LT");
      isA = ( buf[1] & 0x04 );   if ( isA ) DBGLN("A");
      isB = ( buf[1] & 0x02 );   if ( isB ) DBGLN("B");
      isC = ( buf[1] & 0x80 || buf[1] & 0x10 );   if ( isC ) DBGLN("C");
      isX = ( buf[1] & 0x08 );   if ( isX ) DBGLN("X");
      isY = ( buf[1] & 0x01 );   if ( isY ) DBGLN("Y");
      isZ = ( buf[1] & 0x40 || buf[1] & 0x20 );   if ( isZ ) DBGLN("Z");
      isStart = ( buf[2] & 0x01 );    if ( isStart ) DBGLN("Start");
      isSelect = ( buf[2] & 0x02 );   if ( isSelect ) DBGLN("Select");
    }
    else if ( len == 7 && buf[2] == 0x80 && buf[3] == 0x80 && buf[6] == 0 ) {
      DBG("8BITDO ");
      isUp =    ( buf[1] == 0x00 );     if ( isUp ) DBGLN("UP");
      isRight = ( buf[0] == 0xff );     if ( isRight ) DBGLN("RT");
      isDown =  ( buf[1] == 0xff );     if ( isDown ) DBGLN("DN");
      isLeft =  ( buf[0] == 0x00 );     if ( isLeft ) DBGLN("LT");
      isA = ( buf[5] & 0x04 );   if ( isC ) DBGLN("A");
      isB = ( buf[4] & 0x20 );   if ( isB ) DBGLN("B");
      isC = ( buf[4] & 0x10 );   if ( isA ) DBGLN("C");
      isX = ( buf[4] & 0x80 );   if ( isX ) DBGLN("X");
      isY = ( buf[5] & 0x01 );   if ( isY ) DBGLN("Y");
      isZ = ( buf[5] & 0x08 );   if ( isZ ) DBGLN("Z");
      isStart = ( buf[5] & 0x80 );    if ( isStart ) DBGLN("Start");
      isSelect = ( buf[5] & 0x40 );   if ( isSelect ) DBGLN("Select");
    }
    break;
    
  case 8 :
    if ( buf[0] == 0x01 && buf[1] == 0x7F ) {
      // PS2 to USB Converter (Black Simple, One player)
      DBG("PS2Single ");
      isUp =    ( buf[4] == 0x00 );     if ( isUp ) DBGLN("UP");
      isRight = ( buf[3] == 0xff );     if ( isRight ) DBGLN("RT");
      isDown =  ( buf[4] == 0xff );     if ( isDown ) DBGLN("DN");
      isLeft =  ( buf[3] == 0x00 );     if ( isLeft ) DBGLN("LT");
      isA = ( buf[5] & 0x40 );   if ( isA ) DBGLN("A");
      isB = ( buf[5] & 0x20 );   if ( isB ) DBGLN("B");
      isC = ( buf[6] & 0x08 || buf[6] & 0x01 );   if ( isC ) DBGLN("C");
      isX = ( buf[5] & 0x80 );   if ( isX ) DBGLN("X");
      isY = ( buf[5] & 0x10 );   if ( isY ) DBGLN("Y");
      isZ = ( buf[6] & 0x04 || buf[6] & 0x02 );   if ( isZ ) DBGLN("Z");
      isStart = ( buf[6] & 0x20 );    if ( isStart ) DBGLN("Start");
      isSelect = ( buf[6] & 0x10 );   if ( isSelect ) DBGLN("Select");
    }
    else if ( buf[2] == 0x80 && buf[3] == 0x80 && buf[4] == 0x80 ) {
      // NES copy USB pad
      DBG("NES-USB ");
      isUp =    ( buf[1] == 0 );      if ( isUp ) DBGLN("UP");
      isDown =  ( buf[1] == 0xff );   if ( isDown ) DBGLN("DN");
      isLeft =  ( buf[0] == 0 );      if ( isLeft ) DBGLN("LT");
      isRight = ( buf[0] == 0xff );   if ( isRight ) DBGLN("RT");
      isA = ( buf[5] & 0x20 );    if ( isA ) DBGLN("A");
      isB = ( buf[5] & 0x40 );    if ( isB ) DBGLN("B");
      isC = ( buf[6] & 0x01 );    if ( isC ) DBGLN("C");
      isX = ( buf[5] & 0x10 );    if ( isX ) DBGLN("X");
      isY = ( buf[5] & 0x80 );    if ( isY ) DBGLN("Y");
      isZ = ( buf[6] & 0x02 );    if ( isZ ) DBGLN("Z");
      isStart = ( buf[6] & 0x20 );    if ( isStart ) DBGLN("Start");
      isSelect = ( buf[6] & 0x10 );   if ( isSelect ) DBGLN("Select");
    }
    else if ( buf[0] == 0x1 && buf[1] == 0x80 && buf[2] == 0x80 ) {
      // NES copy USB pad 2
      DBG("NES-USB2 ");
      isUp =    ( buf[4] == 0 );      if ( isUp ) DBGLN("UP");
      isDown =  ( buf[4] == 0xff );   if ( isDown ) DBGLN("DN");
      isLeft =  ( buf[3] == 0 );      if ( isLeft ) DBGLN("LT");
      isRight = ( buf[3] == 0xff );   if ( isRight ) DBGLN("RT");
      isA = ( buf[5] & 0x20 );    if ( isA ) DBGLN("A");
      isB = ( buf[5] & 0x40 );    if ( isB ) DBGLN("B");
      isC = ( buf[6] & 0x01 );    if ( isC ) DBGLN("C");
      isX = ( buf[5] & 0x10 );    if ( isX ) DBGLN("X");
      isY = ( buf[5] & 0x80 );    if ( isY ) DBGLN("Y");
      isZ = ( buf[6] & 0x02 );    if ( isZ ) DBGLN("Z");
      isStart = ( buf[6] & 0x20 );    if ( isStart ) DBGLN("Start");
      isSelect = ( buf[6] & 0x10 );   if ( isSelect ) DBGLN("Select");
    }
    break;
    
  case 20 :
    if ( buf[0] == 0 && buf[1] == 0x14 ) {
      // Wing Wireless
      DBG("Wing ");
      isUp =    ( (int8_t)buf[9] > 0 ) || ( buf[2] & 0x01 );    if ( isUp ) DBGLN("UP");
      isDown =  ( (int8_t)buf[9] < 0 ) || ( buf[2] & 0x02 );    if ( isDown ) DBGLN("DN");
      isLeft =  ( (int8_t)buf[7] < 0 ) || ( buf[2] & 0x04 );    if ( isLeft ) DBGLN("LT");
      isRight = ( (int8_t)buf[7] > 0 ) || ( buf[2] & 0x08 );    if ( isRight ) DBGLN("RT");
      isA = ( buf[3] & 0x10 );    if ( isA ) DBGLN("A");
      isB = ( buf[3] & 0x20 );    if ( isB ) DBGLN("B");
      isC = ( buf[3] & 0x01 );    if ( isC ) DBGLN("C");
      isX = ( buf[3] & 0x40 );    if ( isX ) DBGLN("X");
      isY = ( buf[3] & 0x80 );    if ( isY ) DBGLN("Y");
      isZ = ( buf[3] & 0x02 );    if ( isZ ) DBGLN("Z");
      isStart = ( buf[2] & 0x10 );    if ( isStart ) DBGLN("Start");
      isSelect = ( buf[2] & 0x20 );   if ( isSelect ) DBGLN("Select");
    }
    break;
    
  case 27 :
    if ( buf[3] == 0x80 && buf[4] == 0x80 && buf[5] == 0x80 && buf[6] == 0x80 ) {
      // PS3 : Joytron Pae-Wang, Hori Fighting Stick Mini
      DBG("PS3 ");
      isUp =    ( buf[2] == 0x07 || buf[2] == 0x00 || buf[2] == 0x01 );    if ( isUp ) DBGLN("UP");
      isRight = ( buf[2] == 0x01 || buf[2] == 0x02 || buf[2] == 0x03 );    if ( isRight ) DBGLN("RT");
      isDown =  ( buf[2] == 0x03 || buf[2] == 0x04 || buf[2] == 0x05 );    if ( isDown ) DBGLN("DN");
      isLeft =  ( buf[2] == 0x05 || buf[2] == 0x06 || buf[2] == 0x07 );    if ( isLeft ) DBGLN("LT");
      //  10 01 08 20, 01 08 20 10
      //  40 02 04 80, 02 04 80 40
      isA = ( buf[0] & 0x02 || buf[0] & 0x40 );    if ( isA ) DBGLN("A");
      isB = ( buf[0] & 0x04 );    if ( isB ) DBGLN("B");
      isC = ( buf[0] & 0x80 );    if ( isC ) DBGLN("C");
      isX = ( buf[0] & 0x01 || buf[0] & 0x10 );    if ( isX ) DBGLN("X");
      isY = ( buf[0] & 0x08 );    if ( isY ) DBGLN("Y");
      isZ = ( buf[0] & 0x20 );    if ( isZ ) DBGLN("Z");
      isStart = ( buf[1] & 0x02 );    if ( isStart ) DBGLN("Start");
      isSelect = ( buf[1] & 0x01 );   if ( isSelect ) DBGLN("Select");
    }
    break;

  case 64 :
    if ( buf[1] == 0x80 && buf[2] == 0x80 && buf[3] == 0x80 && buf[4] == 0x80 ) {
      // PS4 : Hori Fighting Stick Mini
      DBG("PS4 ");
      int st = buf[5] & 0xf;
      isUp =    ( st == 7 || st <= 1 );     if ( isUp ) DBGLN("UP");
      isRight = ( 1 <= st && st <= 3 );     if ( isRight ) DBGLN("RT");
      isDown =  ( 3 <= st && st <= 5 );     if ( isDown ) DBGLN("DN");
      isLeft =  ( 5 <= st && st <= 7 );     if ( isLeft ) DBGLN("LT");
      isA = ( buf[5] & 0x20 || buf[6] & 0x04 );   if ( isA ) DBGLN("A");
      isB = ( buf[5] & 0x40 );                    if ( isB ) DBGLN("B");
      isC = ( buf[6] & 0x08 );                    if ( isC ) DBGLN("C");
      isX = ( buf[5] & 0x10 || buf[6] & 0x01 );   if ( isX ) DBGLN("X");
      isY = ( buf[5] & 0x80 );                    if ( isY ) DBGLN("Y");
      isZ = ( buf[6] & 0x02 );                    if ( isZ ) DBGLN("Z");
      isStart = ( buf[6] & 0x10 );    if ( isStart ) DBGLN("Start");
      isSelect = ( buf[6] & 0x20 );   if ( isSelect ) DBGLN("Select");
    }
    break;
  }
    
  if ( (buf[3] & 0xF0) == 0xF0 ) {
    // Dahoon DHU-3300
    DBG("DHU-3300 ");
    isUp =    ( buf[1] == 0 );        if ( isUp ) DBGLN("UP");
    isDown =  ( buf[1] == 0xff );     if ( isDown ) DBGLN("DN");
    isLeft =  ( buf[0] == 0 );        if ( isLeft ) DBGLN("LT");
    isRight = ( buf[0] == 0xff );     if ( isRight ) DBGLN("RT");
    isA = ( buf[2] & 0x01 ) || ( buf[2] & 0x08 );    if ( isA ) DBGLN("A");
    isB = ( buf[2] & 0x02 );    if ( isB ) DBGLN("B");
    isC = ( buf[2] & 0x04 );    if ( isC ) DBGLN("C");
    isX = ( buf[2] & 0x10 ) || ( buf[2] & 0x80 );    if ( isX ) DBGLN("X");
    isY = ( buf[2] & 0x20 );    if ( isY ) DBGLN("Y");
    isZ = ( buf[2] & 0x40 );    if ( isZ ) DBGLN("Z");
    isStart = ( buf[3] & 0x01 );    if ( isStart ) DBGLN("Start");
    isSelect = ( buf[3] & 0x02 );   if ( isSelect ) DBGLN("Select");
  }
}

class MyReportParser : public HIDReportParser {
  public:
    MyReportParser() { }
    virtual void Parse(HID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf)
    {
      Dump( len, buf );
      ProcessMouse( len, buf );
      ProcessJoyStick( len, buf );
    }
} ReportParser;

// ----------------------------------
USB Usb;
USBHub Hub1(&Usb);
HIDUniversal Hid(&Usb);

void setup()
{
#ifdef _DEBUG
  Serial.begin(9600);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif

  String str = "WiPad Start : ";
  str.concat (__DATE__);
  Serial.println( str );
#endif

  if ( Usb.Init() == -1 ) {
    DBGLN( "No USB Host Shield Found" );
    while (1); //halt
  }

  delay(200);
  if ( !Hid.SetReportParser(0, &ReportParser) )
    DBGLN( "SetReportParser Error" );

  // msx pins are not HIGH / LOW input,
  // they are NOT_CONNECTED(3state, internaly pull-upped) / LOW

  pinMode( PIN_UP,     INPUT);
  pinMode( PIN_DOWN,   INPUT);
  pinMode( PIN_LEFT,   INPUT);
  pinMode( PIN_RIGHT,  INPUT);
  pinMode( PIN_START,  INPUT);
  pinMode( PIN_SELECT, INPUT);
  pinMode( PIN_A,      INPUT);
  pinMode( PIN_B,      INPUT);
  pinMode( PIN_C,      INPUT);
  pinMode( PIN_X,      INPUT);
  pinMode( PIN_Y,      INPUT);
  pinMode( PIN_Z,      INPUT);

  // initialize
  digitalWrite( PIN_UP,    LOW);
  digitalWrite( PIN_DOWN,  LOW);
  digitalWrite( PIN_LEFT,  LOW);
  digitalWrite( PIN_RIGHT, LOW);
  digitalWrite( PIN_START, LOW);
  digitalWrite( PIN_A,     LOW);
  digitalWrite( PIN_B,     LOW);
  digitalWrite( PIN_C,     LOW);
  digitalWrite( PIN_X,     LOW);
  digitalWrite( PIN_Y,     LOW);
  digitalWrite( PIN_Z,     LOW);

  nJoyMouse = 0;
  x = y = rx = ry = rz = 0;
  TimeOut = 40;
  pressLeft = pressRight = false;
  isUp = isDown = isLeft = isRight = isA = isB = isC = isX = isY = isZ = false;
  time = millis();
}

#ifdef ATMEL_328
// Optimized for Atmel328
// NOTE: Fixed pins!
void sendMSX(char c)
{
    while (digitalRead(PIN_OUT)==LOW) {if (millis()>time) return;};
    DDRD = ((DDRD & 195)|((~ (c>>2)) & 60));
    while (digitalRead(PIN_OUT)==HIGH) {if (millis()>time) return;};
    DDRD = ((DDRD & 195)|((~ (c<<2)) & 60));
}

void JoyHigh()
{
     DDRD=(DDRD & 195);
}

#else
// Standard version
void sendMSX(char c)
{
    while (digitalRead(PIN_OUT)==LOW) {
      if (millis()>time) return;
    };
    if(c&128) pinMode(PIN_RIGHT,INPUT);   else pinMode(PIN_RIGHT,OUTPUT);
    if(c&64)  pinMode(PIN_LEFT,INPUT);    else pinMode(PIN_LEFT,OUTPUT);
    if(c&32)  pinMode(PIN_DOWN,INPUT);    else pinMode(PIN_DOWN,OUTPUT);
    if(c&16)  pinMode(PIN_UP,INPUT);      else pinMode(PIN_UP,OUTPUT);
    while (digitalRead(PIN_OUT)==HIGH) {
      if (millis()>time) return;
    };
    if(c&8)   pinMode(PIN_RIGHT,INPUT);   else pinMode(PIN_RIGHT,OUTPUT);
    if(c&4)   pinMode(PIN_LEFT,INPUT);    else pinMode(PIN_LEFT,OUTPUT);
    if(c&2)   pinMode(PIN_DOWN,INPUT);    else pinMode(PIN_DOWN,OUTPUT);
    if(c&1)   pinMode(PIN_UP,INPUT);      else pinMode(PIN_UP,OUTPUT);
}

void JoyHigh()
{
    pinMode(PIN_UP,INPUT);
    pinMode(PIN_DOWN,INPUT);
    pinMode(PIN_LEFT,INPUT);
    pinMode(PIN_RIGHT,INPUT);
}
#endif

void loop()
{
  Usb.Task();

  switch( nJoyMouse ) {
  case 1: // JoyStick
    pinMode( PIN_UP, isUp ? OUTPUT : INPUT);
    pinMode( PIN_DOWN, isDown ? OUTPUT : INPUT);
    pinMode( PIN_LEFT, isLeft ? OUTPUT : INPUT);
    pinMode( PIN_RIGHT, isRight ? OUTPUT : INPUT);
    pinMode( PIN_START, isStart ? OUTPUT : INPUT);
    pinMode( PIN_SELECT, isSelect ? OUTPUT : INPUT);
  
    pinMode( PIN_A, isA ? OUTPUT : INPUT);
    pinMode( PIN_B, isB ? OUTPUT : INPUT);
    pinMode( PIN_C, isC ? OUTPUT : INPUT);
    pinMode( PIN_X, isX ? OUTPUT : INPUT);
    pinMode( PIN_Y, isY ? OUTPUT : INPUT);
    pinMode( PIN_Z, isZ ? OUTPUT : INPUT);
    break;
    
  case 2: // Mouse
    // move 
    x = x + rx;
    y = y + ry;
    rx = ry = rz = 0;

    // buttons
    if( pressLeft )  pinMode( PIN_A,OUTPUT ); else pinMode( PIN_A, INPUT );
    if( pressRight ) pinMode( PIN_B,OUTPUT ); else pinMode( PIN_B, INPUT );

    // send to msx
    time = millis() + 40;
    sendMSX( -x );
    time = millis() + 3;
    sendMSX( -y );
    if( millis() < time ) {
      x = 0; y = 0;
      time = millis() + 2;
    } 
    while( digitalRead( PIN_OUT ) == LOW ) {
      if( millis() > time )
        break;
    }
    JoyHigh();
    
    break;
  }
}

