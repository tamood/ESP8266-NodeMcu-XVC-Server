#include <ESP8266WiFi.h>

enum PINS {
  TMS = 12,
  TDI,
  TDO,
  TCK
};

const char * ssid = "ssid";
const char * password = "password";

WiFiServer server(2542);

unsigned char tms_buf[256];
unsigned char tdi_buf[256];

void setup() {

  pinMode(TDI, OUTPUT);
  pinMode(TDO, INPUT);
  pinMode(TCK, OUTPUT);
  pinMode(TMS, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting XVC Server on port 2542");

  server.begin();

  digitalWrite(LED_BUILTIN, LOW);
}

void proceed_in_slow_mode(int offset, int len)
{
  int tms = tms_buf[offset];
  int tdi = tdi_buf[offset];
  int tdo = 0;

  for (int i = 0; i < len; i++)
  {
    int tms_bit = tms & (1 << i);
    int tdi_bit = tdi & (1 << i);

    digitalWrite(TMS, tms_bit);
    digitalWrite(TDI, tdi_bit);

    tdo |= digitalRead(TDO) << i;

    digitalWrite(TCK, 1);
    digitalWrite(TCK, 0);
  }

  tdi_buf[offset] = tdo;
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while (1)
  {
    if (client.available() > 10)
    {
      digitalWrite(LED_BUILTIN, HIGH);

      unsigned char cmd_name[6];
      client.read(cmd_name, 6);

      if (memcmp(cmd_name, "shift:", 6))
      {
        client.stop();
        return;
      }

      int cmd_len;
      client.read((uint8_t *) &cmd_len, 4);

      int lenB = (cmd_len + 7) / 8;
      while (client.available() < (lenB * 2));

      client.read(tms_buf, lenB);
      client.read(tdi_buf, lenB);

      int i = 0;
      for (; (i < lenB - 1); i++)
        proceed_in_slow_mode(i, 8);

      proceed_in_slow_mode(i, cmd_len - i * 8);

      if (client)
        if (client.write(tdi_buf, lenB) != lenB)
        {
          client.stop();
          return;
        }

      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
