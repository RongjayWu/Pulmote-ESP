#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IRsend.h>

#define RECV_PIN 15
#define IR_SEND_PIN 4
#define RESEND_INTERVAL 2000
#define MIN_SIGNAL_INTERVAL 300 // 避免雜訊快速觸發

IRrecv irrecv(RECV_PIN);
IRsend irsend(IR_SEND_PIN);
decode_results results;

decode_results lastSignal;
bool hasSignal = false;

unsigned long lastReceiveTime = 0;
unsigned long lastSendTime = 0;

void setup()
{
  Serial.begin(115200);
  irrecv.enableIRIn();
  irsend.begin();
  Serial.println("IR Receiver & Smart Repeater Ready");
}

// 過濾無效訊號
bool isValidSignal(decode_results &r)
{

  // 過濾 NEC 重複碼
  if (r.decode_type == NEC &&
      (r.value == 0xFFFFFFFF || r.bits == 0))
  {
    return false;
  }

  // 過濾 UNKNOWN 或太短訊號
  if (r.decode_type == UNKNOWN || r.bits < 8)
  {
    return false;
  }

  return true;
}

void sendIR(decode_results &signal)
{

  Serial.print("Sending: ");
  Serial.println(resultToHumanReadableBasic(&signal));

  irrecv.disableIRIn(); // 發送時暫停接收

  switch (signal.decode_type)
  {
  case NEC:
    irsend.sendNEC(signal.value, signal.bits);
    break;

  case SONY:
    irsend.sendSony(signal.value, signal.bits);
    break;

  case RC5:
    irsend.sendRC5(signal.value, signal.bits);
    break;

  case RC6:
    irsend.sendRC6(signal.value, signal.bits);
    break;

  default:
    // 支援 RAW 發送
    irsend.sendRaw((const uint16_t *)signal.rawbuf, signal.rawlen, 38);
    break;
  }

  irrecv.enableIRIn(); // 重新啟動接收
}

void loop()
{

  // 接收 IR
  if (irrecv.decode(&results))
  {

    unsigned long now = millis();

    if (isValidSignal(results) &&
        (now - lastReceiveTime > MIN_SIGNAL_INTERVAL))
    {

      Serial.print("Received: ");
      Serial.println(resultToHumanReadableBasic(&results));

      lastSignal = results;
      hasSignal = true;
      lastReceiveTime = now;
    }

    irrecv.resume();
  }

  // 定時重發
  if (hasSignal && millis() - lastSendTime >= RESEND_INTERVAL)
  {
    sendIR(lastSignal);
    lastSendTime = millis();
  }
}