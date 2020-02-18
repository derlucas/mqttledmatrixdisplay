#include <Homie.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include <Max72xxPanel.h>


int pinCS = 15;
int numberOfHorizontalDisplays = 8;
int numberOfVerticalDisplays = 1;


Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

HomieNode scrollNode("scroll", "commands","control");
HomieNode textNode("text", "commands","control");
HomieNode barNode("bar", "bargraph0to100","control");

int spacer = 1;
int width = 5 + spacer;

int maxSteps;
int currentStep;
String scrollText;
long scrollMillis;
int scrollWait = 20;
long barMillis;
bool scrolling = false;






void showBar(int value) {
    matrix.fillScreen(LOW);
    matrix.setCursor(0, 0);

    int i = matrix.width() * value / 100;

    for(uint8_t x = 0; x < matrix.width(); x++) {
        for(uint8_t y = 0; y < matrix.height(); y++) {
            matrix.drawPixel(x, y, i>=x);
        }
    }

    matrix.write();
}

void showText(String text, int x) {
    matrix.fillScreen(LOW);
    matrix.setCursor(x, 0);
    matrix.print(text);
    matrix.write();
}

void scrollStep() {

    if(maxSteps == 0) return;

    matrix.fillScreen(LOW);

    int letter = currentStep / width;
    int x = (matrix.width() - 1) - currentStep % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
        if ( letter < scrollText.length() ) {
            matrix.drawChar(x, y, scrollText[letter], HIGH, LOW, 1);
        }

        letter--;
        x -= width;
    }

    currentStep++;
    matrix.write(); // Send bitmap to display

    if(currentStep >= maxSteps) {
        currentStep = 0;
        scrollText = "";
        maxSteps = 0;
        scrolling = false;
    }

}

void scrollTextAsync(String text, int wait) {

    if(text.length() > 200) {
        text = text.substring(0, 200);
    }

    text.trim();
    scrolling = true;
    maxSteps = width * text.length() + matrix.width() - spacer;
    currentStep = 0;
    scrollText = text;
    scrollWait = wait;
}

void scrollTextSync(String text, int wait) {
    for ( int i = 0 ; i < width * text.length() + matrix.width() - spacer; i++ ) {

        matrix.fillScreen(LOW);

        int letter = i / width;
        int x = (matrix.width() - 1) - i % width;
        int y = (matrix.height() - 8) / 2; // center the text vertically

        while ( x + width - spacer >= 0 && letter >= 0 ) {
            if ( letter < text.length() ) {
                matrix.drawChar(x, y, text[letter], HIGH, LOW, 1);
            }

            letter--;
            x -= width;
        }

        matrix.write(); // Send bitmap to display

        delay(wait);
    }
}

bool scrollHandler(const HomieRange& range, const String& value) {
    int sep = value.indexOf("|");
    int wait = 30;

    if(sep > 0) {
        wait = value.substring(0, sep).toInt();
        Homie.getLogger() << "scroll " << value << " wait " << wait << endl;
        scrollTextAsync(value.substring(sep+1), wait);
    } else {
        scrollTextAsync(value, wait);
        Homie.getLogger() << "scroll " << value << endl;
    }

    return true;
}

bool textHandler(const HomieRange& range, const String& value) {
    if(scrolling) return false;
    showText(value, 0);
    return true;
}

bool barHandler(const HomieRange& range, const String& value) {
    if(scrolling) return false;
    int intval = value.toInt();

    if(intval > 100) intval = 100;
    if(intval < 0) intval = 0;

    showBar(intval);
    barMillis = millis();

    return true;
}


void onHomieEvent(const HomieEvent& event) {
    switch(event.type) {
        case HomieEventType::STANDALONE_MODE:
            scrollTextAsync("StndAln.Mode", 10);
            break;
        case HomieEventType::CONFIGURATION_MODE:
            scrollTextAsync("Conf.Mode", 10);
            break;
        case HomieEventType::NORMAL_MODE:
            scrollTextAsync("Normal Mode", 10);
            break;
        case HomieEventType::OTA_STARTED:
            scrollTextAsync("OTA Start", 10);
            break;
        case HomieEventType::OTA_PROGRESS:
            // Do whatever you want when OTA is in progress

            // You can use event.sizeDone and event.sizeTotal
            break;
        case HomieEventType::OTA_FAILED:
            scrollTextAsync("OTA Failed", 10);
            break;
        case HomieEventType::OTA_SUCCESSFUL:
            scrollTextAsync("OTA Success", 10);
            break;
        case HomieEventType::ABOUT_TO_RESET:
            scrollTextAsync("Reset", 10);
            break;
        case HomieEventType::WIFI_CONNECTED:
            scrollTextAsync("WiFi Conn", 10);
            // You can use event.ip, event.gateway, event.mask
            break;
        case HomieEventType::WIFI_DISCONNECTED:
            // Do whatever you want when Wi-Fi is disconnected in normal mode
            scrollTextAsync("WiFi Disc", 10);

            // You can use event.wifiReason
            /*
              Wi-Fi Reason (souce: https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientEvents/WiFiClientEvents.ino)
              0  SYSTEM_EVENT_WIFI_READY               < ESP32 WiFi ready
              1  SYSTEM_EVENT_SCAN_DONE                < ESP32 finish scanning AP
              2  SYSTEM_EVENT_STA_START                < ESP32 station start
              3  SYSTEM_EVENT_STA_STOP                 < ESP32 station stop
              4  SYSTEM_EVENT_STA_CONNECTED            < ESP32 station connected to AP
              5  SYSTEM_EVENT_STA_DISCONNECTED         < ESP32 station disconnected from AP
              6  SYSTEM_EVENT_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
              7  SYSTEM_EVENT_STA_GOT_IP               < ESP32 station got IP from connected AP
              8  SYSTEM_EVENT_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
              9  SYSTEM_EVENT_STA_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
              10 SYSTEM_EVENT_STA_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
              11 SYSTEM_EVENT_STA_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
              12 SYSTEM_EVENT_STA_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
              13 SYSTEM_EVENT_AP_START                 < ESP32 soft-AP start
              14 SYSTEM_EVENT_AP_STOP                  < ESP32 soft-AP stop
              15 SYSTEM_EVENT_AP_STACONNECTED          < a station connected to ESP32 soft-AP
              16 SYSTEM_EVENT_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
              17 SYSTEM_EVENT_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
              18 SYSTEM_EVENT_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
              19 SYSTEM_EVENT_GOT_IP6                  < ESP32 station or ap or ethernet interface v6IP addr is preferred
              20 SYSTEM_EVENT_ETH_START                < ESP32 ethernet start
              21 SYSTEM_EVENT_ETH_STOP                 < ESP32 ethernet stop
              22 SYSTEM_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
              23 SYSTEM_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
              24 SYSTEM_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
              25 SYSTEM_EVENT_MAX
            */
            break;
        case HomieEventType::MQTT_READY:
            scrollTextAsync("MQTT Conn", 10);
            break;
        case HomieEventType::MQTT_DISCONNECTED:
            // Do whatever you want when MQTT is disconnected in normal mode
            scrollTextAsync("MQTT Disc", 10);

            // You can use event.mqttReason
            /*
              MQTT Reason (source: https://github.com/marvinroger/async-mqtt-client/blob/master/src/AsyncMqttClient/DisconnectReasons.hpp)
              0 TCP_DISCONNECTED
              1 MQTT_UNACCEPTABLE_PROTOCOL_VERSION
              2 MQTT_IDENTIFIER_REJECTED
              3 MQTT_SERVER_UNAVAILABLE
              4 MQTT_MALFORMED_CREDENTIALS
              5 MQTT_NOT_AUTHORIZED
              6 ESP8266_NOT_ENOUGH_SPACE
              7 TLS_BAD_FINGERPRINT
            */
            break;
        case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
            // Do whatever you want when an MQTT packet with QoS > 0 is acknowledged by the broker

            // You can use event.packetId
            break;
        case HomieEventType::READY_TO_SLEEP:
            // After you've called `prepareToSleep()`, the event is triggered when MQTT is disconnected
            break;
        case HomieEventType::SENDING_STATISTICS:
            // Do whatever you want when statistics are sent in normal mode
            break;
    }
}

void setup() {
    Serial.begin(115200);

    Homie.onEvent(onHomieEvent); // before Homie.setup()
    Homie.setResetTrigger(D1, LOW, 2000);

    matrix.setIntensity(1);
    for(int i=0;i<numberOfHorizontalDisplays;i++) {
        matrix.setRotation(i, 1);
    }

    Homie_setFirmware("matrix-64x8", "1.0.0");

    scrollNode.advertise("on").settable(scrollHandler);
    textNode.advertise("on").settable(textHandler);
    barNode.advertise("on").settable(barHandler);


    scrollTextSync("huhu", 15);
    Homie.setup();

}

void loop() {

    Homie.loop();

    long currentMillis = millis();
    if(currentMillis - scrollMillis > scrollWait) {
        if(scrollText.length() > 0) {
            scrollStep();
        }
        scrollMillis = currentMillis;
    }

    if(barMillis > 0 && currentMillis - barMillis > 2000) {
        showText("", 0);
        barMillis = 0;
    }
}