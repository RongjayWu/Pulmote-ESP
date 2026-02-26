#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IRsend.h>

const int RECV_PIN = 15;
IRrecv irrecv(RECV_PIN);
decode_results results;

const int IR_SEND_PIN = 4;
IRsend irsend(IR_SEND_PIN);

// 記錄最後收到的訊號
decode_results lastSignal;
bool hasSignal = false;
unsigned long lastSendTime = 0; // 記錄上一次發送時間

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
  irsend.begin();
  Serial.println("IR Receiver & Repeater Ready");
}

// 判斷是否為有效訊號（非重複、非閒置）
bool isValidSignal(decode_results &r) {
  // 過濾 NEC Repeat 訊號（bits = 0 或 value = 0xFFFFFFFF）
  if (r.decode_type == NEC && (r.value == 0xFFFFFFFF || r.bits == 0)) return false;

  // 可以加入其他協定的過濾條件
  // if (r.decode_type == SONY && r.bits == 0) return false;

  return true;
}

void loop() {
  // 接收新訊號
  if (irrecv.decode(&results)) {
    if (isValidSignal(results)) {
      Serial.print("Received: ");
      Serial.println(resultToHumanReadableBasic(&results));

      // 記錄最後收到的有效訊號
      lastSignal = results;
      hasSignal = true;
    } else {
      Serial.println("Ignored: Repeat/Idle signal");
    }

    irrecv.resume(); // 準備接收下一個訊號
  }

  // 重複發送
  if (hasSignal && (millis() - lastSendTime >= 2000)) {
    Serial.print("Sending: ");
    Serial.println(resultToHumanReadableBasic(&lastSignal));

    switch (lastSignal.decode_type) {
      case NEC:
        irsend.sendNEC(lastSignal.value, lastSignal.bits);
        break;
      case SONY:
        irsend.sendSony(lastSignal.value, lastSignal.bits);
        break;
      case RC5:
        irsend.sendRC5(lastSignal.value, lastSignal.bits);
        break;
      case RC6:
        irsend.sendRC6(lastSignal.value, lastSignal.bits);
        break;
      default:
        Serial.println("Unsupported IR protocol");
        break;
    }

    lastSendTime = millis();
  }
}