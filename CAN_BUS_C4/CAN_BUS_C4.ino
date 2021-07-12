#include <SPI.h>
#include "mcp_can.h"
#include "mcp2515_can.h"

const int CS_PIN = 10;

// Serial port data rate
const long SERIAL_SPEED = 115200;

// CAN bus data rate
const byte CAN_SPEED = CAN_125KBPS;

// Reverse gear state
byte reverseEngaged = 0;
unsigned long timeSinceSourceChange = 0;
bool shouldStopReverse = false;
unsigned long timeSinceReverseDisengaged = 0;

bool mute = false;
bool mute_pressed = false;
bool next_pressed = false;

mcp2515_can CAN(CS_PIN);
unsigned char len = 0;
byte buffer[8];

void setup() {
    Serial.begin(SERIAL_SPEED);
    byte canSpeed = CAN_SPEED;

    while (CAN.begin(canSpeed, MCP_8MHz) != CAN_OK) {
      delay(100);
    }
    Serial.println("CAN OK !");
}

void loop() 
{
    //unsigned char len = 0;
    //byte buf[8];
    
    //  - timeSinceReverseDisengaged: this timer waits a few seconds after disengaging reverse
    //    before switching back to OpenAuto, in case we're parallel parking and switching back and
    //    forth between reverse and first gear. If we don't do this, the screen would switch
    //    back to OpenAuto immediately (which takes some time) and it takes some time to go back
    //    to Camera again, which is annoying when parking.
    
    if (shouldStopReverse && millis() - timeSinceReverseDisengaged > 5000) 
    {    
        Serial.println("REVERSE_DISENGAGED");
        
        timeSinceSourceChange = millis();
        reverseEngaged = false;
        shouldStopReverse = false;
        timeSinceReverseDisengaged = 0;
    }


    
    if (CAN.checkReceive() == CAN_MSGAVAIL) 
    {
        CAN.readMsgBuf(&len, buffer);
        int id = CAN.getCanId();

        //can_debug();

        // reverse gear state
        if (id == 246 && len == 8) 
        {
            //Serial.println(buffer[7]);
            if (buffer[7] == 160) 
            {
                // Reverse gear engaged
                if(!reverseEngaged)
                  Serial.println("REVERSE_ENGAGED");
                
                reverseEngaged = true;
                shouldStopReverse = false;
                timeSinceReverseDisengaged = 0;
            } 
            else 
            {
                // Reverse gear disengaged
                if (reverseEngaged && !shouldStopReverse) 
                {
                    shouldStopReverse = true;
                    timeSinceReverseDisengaged = millis();
                }
            }   
        }
        else
        if (id == 543) 
        {
          if (buffer[0] == 0) 
          {
              // no button pressed
              if(mute_pressed)
              {
                  mute_pressed = false;
              }
              if(next_pressed)
              {
                  next_pressed = false;
              }
          }
          else if (buffer[0] == 8) 
          {
              // vol+ button pressed
              Serial.println("VOL_UP");
          }
          else if (buffer[0] == 4) 
          {
              // vol- button pressed
              Serial.println("VOL_DOWN");
          }
          else if (buffer[0] == 12) 
          {
              // MUTE button pressed
              //Serial.println("TOGGLE_PLAY");
              if(!mute_pressed)
              {
                  mute_pressed = true;
                  if (mute)
                  {
                      mute = false;
                      Serial.println("PLAY");
                  }
                  else
                  {
                      mute = true;
                      Serial.println("PAUSE");
                  }
              } 
          }
          else if (buffer[0] == 128) 
          {
              // NEXT button pressed
              //Serial.println("NEXT");
              if(!next_pressed)
              {
                  next_pressed = true;
                  Serial.println("NEXT");
              }
          }
//            if (buffer[0] & 0b00000000) 
//            {
//                // no button pressed
//                if(mute_pressed)
//                {
//                    mute_pressed = false;
//                }
//                if(next_pressed)
//                {
//                    next_pressed = false;
//                }
//            }
//            else if (buffer[0] & 0b00001000) 
//            {
//                // vol+ button pressed
//                Serial.println("VOL_UP");
//            }
//            else if (buffer[0] & 0b00000100) 
//            {
//                // vol- button pressed
//                Serial.println("VOL_DOWN");
//            }
//            else if (buffer[0] & 0b00001100) 
//            {
//                // MUTE button pressed
//                Serial.println("TOGGLE_PLAY");
//                /*if(!mute_pressed)
//                {
//                    mute_pressed = true;
//                    if (mute)
//                    {
//                        mute = false;
//                        Serial.println("PLAY");
//                    }
//                    else
//                    {
//                        mute = true;
//                        Serial.println("PAUSE");
//                    }
//                }*/ 
//            }
//            else if (buffer[0] & 0b10000000) 
//            {
//                // NEXT button pressed
//                Serial.println("NEXT");
//                /*if(!next_pressed)
//                {
//                    next_pressed = true;
//                    Serial.println("NEXT");
//                }*/
//            }
        }


    }
}

void can_debug()
{
  Serial.print("FRAME:ID=");
        Serial.print(CAN.getCanId());
        Serial.print(":LEN=");
        Serial.print(len);
        
        char tmp[3];
        for(int i = 0; i<len; i++) {
            Serial.print(":");
            
            snprintf(tmp, 3, "%02X", buffer[i]);
            
            Serial.print(tmp);
        }

        Serial.println();
}
