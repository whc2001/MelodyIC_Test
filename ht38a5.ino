#define SCL 2
#define SDA 3

char buf[10];
unsigned char idx = 0;

void SendCmd(unsigned char cmd)
{
  unsigned char i;
  bool parity = false;
  digitalWrite(SCL, LOW);
  delay(18);
  for(i = 0; i < 8; ++i)
  {
    digitalWrite(SCL, LOW);
    delay(2);
    if(cmd & 0x01)
    {
      digitalWrite(SDA, HIGH);
      parity = !parity;
    }
    else
    {
      digitalWrite(SDA, LOW);
    }
    digitalWrite(SCL, HIGH);
    delay(2);
    cmd >>= 1;
  }
  digitalWrite(SCL, LOW);
  delay(2);
  digitalWrite(SDA, parity ? HIGH : LOW);
  digitalWrite(SCL, HIGH);
  delay(2);
}

void setup() 
{
  Serial.begin(115200);
  pinMode(SCL, OUTPUT);
  pinMode(SDA, OUTPUT);
  digitalWrite(SCL, HIGH);
  digitalWrite(SDA, HIGH);
  delay(500);
  Serial.println("Init OK");
  Serial.println("HOLTEK HT38A5 TEST");
  Serial.println("Commands:");
  Serial.println("    Pn (1 <= n <= 12): Play song n");
  Serial.println("    Vn (1 <= n <= 16): Set volume to n");
  Serial.println("    S: Stop playing");
  Serial.println();
  SendCmd(0x50);  // Stop
}

void loop() 
{
  if(Serial.available())
  {
    char recv = Serial.read();
    buf[idx] = recv;
    idx = (idx + 1) % 10;
    if(recv == 0x0A)
    {
      idx = 0;
      if(buf[0] == 'P' || buf[0] == 'p')
      {
        unsigned char val = atoi(buf + 1);
        if(val < 1 || val > 12)
          Serial.println("Song number must be from 1 to 12!");
        else
        {
          SendCmd(val - 1);
          Serial.print("Playing song #");
          Serial.println(val);
        }
      }
      else if(buf[0] == 'V' || buf[0] == 'v')
      {
        unsigned char val = atoi(buf + 1);
        if(val < 1 || val > 16)
          Serial.println("Volume must be from 1 (min) to 16 (max)!");
        else
        {
          SendCmd(0x40 + (16 - val));
          Serial.print("Volume set to ");
          Serial.println(val);
        }
      }
      else if(buf[0] == 'S' || buf[0] == 's')
      {
        SendCmd(0x50);
        Serial.println("Stop");        
      }
    }
  }
}
