
/*************************************************************
  Download latest ERa library here:
    https://github.com/eoh-jsc/era-lib/releases/latest
    https://www.arduino.cc/reference/en/libraries/era
    https://registry.platformio.org/libraries/eoh-ltd/ERa/installation

    ERa website:                https://e-ra.io
    ERa blog:                   https://iotasia.org
    ERa forum:                  https://forum.eoh.io
    Follow us:                  https://www.fb.com/EoHPlatform
 *************************************************************/
// #include <ERa.hpp>
// #include <Widgets/ERaWidgets.hpp> 


// Enable debug console
#define ERA_DEBUG
#define VPIN_PROTECT_FROM   10
#define VPIN_PROTECT_TO     11
#define VPIN_PA   1
#define VPIN_PB   2
#define VPIN_PC   3
#define VPIN_S1   4
#define VPIN_S2   5
#define VPIN_S3   6
#define VPIN_COSA 7
#define VPIN_COSB 8
#define VPIN_COSC 9


float protectCurrent = 0.0;


/* Define MQTT host */
#define DEFAULT_MQTT_HOST "mqtt1.eoh.io"

// You should get Auth Token in the ERa App or ERa Dashboard
#define ERA_AUTH_TOKEN "32ce2a87-7ea4-44da-8a95-e5ecd508f57b"

/* Define setting button */
// #define BUTTON_PIN              0

#if defined(BUTTON_PIN)
    // Active low (false), Active high (true)
    #define BUTTON_INVERT       false
    #define BUTTON_HOLD_TIMEOUT 5000UL

    // This directive is used to specify whether the configuration should be erased.
    // If it's set to true, the configuration will be erased.
    #define ERA_ERASE_CONFIG    false
#endif

#include <Arduino.h>
#include <ERa.hpp>
#include <ERaSimpleEsp32.hpp>
const char ssid[] = "Trung Tin";
const char pass[] = "2225202160107@student.tdmu.edu.vn";

WiFiClient mbTcpClient;

#if defined(ERA_AUTOMATION)
    #include <Automation/ERaSmart.hpp>

    #if defined(ESP32) || defined(ESP8266)
        #include <Time/ERaEspTime.hpp>
        ERaEspTime syncTime;
    #else
        #define USE_BASE_TIME

        #include <Time/ERaBaseTime.hpp>
        ERaBaseTime syncTime;
    #endif

    ERaSmart smart(ERa, syncTime);
#endif

#if defined(BUTTON_PIN)
    #include <ERa/ERaButton.hpp>

    ERaButton button;

    #if ERA_VERSION_NUMBER >= ERA_VERSION_VAL(1, 6, 0)
        static void eventButton(uint16_t pin, ButtonEventT event) {
            if (event != ButtonEventT::BUTTON_ON_HOLD) {
                return;
            }
            ERa.switchToConfig(ERA_ERASE_CONFIG);
            (void)pin;
        }
    #elif ERA_VERSION_NUMBER >= ERA_VERSION_VAL(1, 2, 0)
        static void eventButton(uint8_t pin, ButtonEventT event) {
            if (event != ButtonEventT::BUTTON_ON_HOLD) {
                return;
            }
            ERa.switchToConfig(ERA_ERASE_CONFIG);
            (void)pin;
        }
    #else
        static void eventButton(ButtonEventT event) {
            if (event != ButtonEventT::BUTTON_ON_HOLD) {
                return;
            }
            ERa.switchToConfig(ERA_ERASE_CONFIG);
        }
    #endif

    #if defined(ESP32)
        #include <pthread.h>

        pthread_t pthreadButton;

        static void* handlerButton(void* args) {
            for (;;) {
                button.run();
                ERaDelay(10);
            }
            pthread_exit(NULL);
        }

        void initButton() {
            pinMode(BUTTON_PIN, INPUT);
            button.setButton(BUTTON_PIN, digitalRead, eventButton,
                            BUTTON_INVERT).onHold(BUTTON_HOLD_TIMEOUT);
            pthread_create(&pthreadButton, NULL, handlerButton, NULL);
        }
    #elif defined(ESP8266)
        #include <Ticker.h>

        Ticker ticker;

        static void handlerButton() {
            button.run();
        }

        void initButton() {
            pinMode(BUTTON_PIN, INPUT);
            button.setButton(BUTTON_PIN, digitalRead, eventButton,
                            BUTTON_INVERT).onHold(BUTTON_HOLD_TIMEOUT);
            ticker.attach_ms(100, handlerButton);
        }
    #elif defined(ARDUINO_AMEBA)
        #include <GTimer.h>

        const uint32_t timerIdButton {0};

        static void handlerButton(uint32_t data) {
            button.run();
            (void)data;
        }

        void initButton() {
            pinMode(BUTTON_PIN, INPUT);
            button.setButton(BUTTON_PIN, digitalReadArduino, eventButton,
                            BUTTON_INVERT).onHold(BUTTON_HOLD_TIMEOUT);
            GTimer.begin(timerIdButton, (100 * 1000), handlerButton);
        }
    #endif
#endif

/* This function will run every time ERa is connected */
ERA_CONNECTED() {
    ERA_LOG(ERA_PSTR("ERa"), ERA_PSTR("ERa connected!"));
}

/* This function will run every time ERa is disconnected */
ERA_DISCONNECTED() {
    ERA_LOG(ERA_PSTR("ERa"), ERA_PSTR("ERa disconnected!"));
}
// Tạo biến ERaString để lưu chuỗi nhận được khi gửi từ ERa
ERaString estr;
// Tạo terminal box trong đó:
// V1 là from datastream
// V2 là to datastream
// ERaWidgetTerminalBox terminal(estr, V10, V11);
// // // Hàm này sẽ được gọi khi nhận được chuỗi từ ERa gửi xuống
// void terminalCallback() {
// //     // Ví dụ khi từ ERa gửi chuỗi "Hi"
// //     // Chip sẽ phản hồi lại "Hello! Thank you for using ERa."

// //     // Kiểm tra nếu nhận được chuỗi "Hi"
//     if (estr == "10") {
//         // Đưa chuỗi "Hello! " vào buffer gửi
//         terminal.print("ĐÃ THIẾT LẬP DÒNG BẢO VỆ! ");
//     }
// //     // Đưa chuỗi "Thank you for using ERa." vào buffer gửi
//     terminal.print("Thank you for using ERa.");
// //     // Gửi chuỗi trong buffer lên ERa
//     terminal.flush();
// }
// Khởi tạo Terminal box widget với hàm callback: terminalCallback

// Đặt hàm trên trước dòng
// ERa.begin(ssid, pass);
/* This function print uptime every second */
void timerEvent() {
    ERA_LOG(ERA_PSTR("Timer"), ERA_PSTR("Uptime: %d"), ERaMillis() / 1000L);
  float P_A = random(100, 200); // dữ liệu ảo
  float P_B = random(200, 300);
  float P_C = random(300, 400);
  float V_A = random(1000, 2000); 
  float V_B = random(2000, 3000); 
  float V_C = random(3000, 4000);  
float COSA = 0.8 + random(-50, 51) / 1000.0;  // 0.75 - 0.85
float COSB = 0.85 + random(-30, 31) / 1000.0; // 0.82 - 0.88
float COSC = 0.9 + random(-20, 21) / 1000.0;  // 0.88 - 0.92
  ERa.virtualWrite(VPIN_PA, P_A);
  ERa.virtualWrite(VPIN_PB, P_B);
  ERa.virtualWrite(VPIN_PC, P_C);
  ERa.virtualWrite(VPIN_S1, V_A);
  ERa.virtualWrite(VPIN_S2, V_B);
  ERa.virtualWrite(VPIN_S3, V_C);
  ERa.virtualWrite(VPIN_COSA, COSA);
  ERa.virtualWrite(VPIN_COSB, COSB);
  ERa.virtualWrite(VPIN_COSC, COSC);
  ERA_LOG(ERA_PSTR("Timer"), ERA_PSTR("P_A=%.2f, P_B=%.2f, P_C=%.2f,V_A=%.2f, V_B=%.2f, V_C=%.2f,COSA=%.2f, COSB=%.2f, COSC=%.2f"), P_A, P_B, P_C,V_A, V_B, V_C,COSA,COSB,COSC);
//   ERA_LOG(ERA_PSTR("Timer"), ERA_PSTR("V_A=%.2f, V_B=%.2f, V_C=%.2f"), V_A, V_B, V_C);

}


ERA_WRITE(V10) {
    if (!param.isString()) return;
    ERaString estr = param.getString();
    if (estr == "10") digitalWrite(0, HIGH);
    else if (estr == "0") digitalWrite(0, LOW);
}
#if defined(USE_BASE_TIME)
    unsigned long getTimeCallback() {
        // Please implement your own function
        // to get the current time in seconds.
        return 0;
    } 
#endif

void setup() {
    /* Setup debug console */
#if defined(ERA_DEBUG)
    Serial.begin(115200);
#endif

#if defined(BUTTON_PIN)
    /* Initializing button. */
    initButton();
    /* Enable read/write WiFi credentials */
    ERa.setPersistent(true);
#endif

#if defined(USE_BASE_TIME)
    syncTime.setGetTimeCallback(getTimeCallback);
#endif

    /* Setup Client for Modbus TCP/IP */
    ERa.setModbusClient(mbTcpClient);

    /* Set scan WiFi. If activated, the board will scan
       and connect to the best quality WiFi. */
    ERa.setScanWiFi(true);
    // terminal.begin(terminalCallback);         
    /* Initializing the ERa library. */
    ERa.begin(ssid, pass);

    /* Setup timer called function every second */
    ERa.addInterval(1000L, timerEvent);


}


void loop() {
    ERa.run();

}
