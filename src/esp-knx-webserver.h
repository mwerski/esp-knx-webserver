#pragma once

#include <Arduino.h>

#if defined(ESP32)
#include <WebServer.h>
#include <ArduinoOTA.h>
#elif defined(ESP8266)
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#elif defined(LIBRETINY)
#include <WebServer.h>
#else
#error "Wrong hardware. Not ESP8266 or ESP32 or LIBRETINY"
#endif

typedef enum __knxModeOptions
{
    KNX_MODE_OFF = 0,
    KNX_MODE_NORMAL = 1,
    KNX_MODE_PROG = 2,
} knxModeOptions_t;

typedef void callbackSetKnxMode(knxModeOptions_t mode);
typedef knxModeOptions_t callbackGetKnxMode();
typedef void callbackStartTftUpdate();
typedef void callbackStartTftDebug();

class KnxWebserver
{
public:
    void startWeb(const char *username, const char *password);
    void startOta();
    void endOta();
    void setHostname(String newName);
    void setBuildDetails(String details);
    void setKnxDetail(String physAddr, bool configOk);
    void loop();

    void registerSetKnxModeCallback(callbackSetKnxMode *fctn);
    void registerGetKnxModeCallback(callbackGetKnxMode *fctn);
    void registerTftUpdateCallback(callbackStartTftUpdate *fctn);
    void registerTftDebugCallback(callbackStartTftDebug *fctn);

private:
#if defined(ESP32) || defined(LIBRETINY)
    WebServer *server;
#elif defined(ESP8266)
    ESP8266WebServer *server;
#endif
    String hostname = "ESP-KNX-Device";
    String knxPhysAddr = "0.0.0";
    String buildDetails = "";
    bool knxConfigOk = false;
    const char *username;
    const char *password;
    bool authRequired = false;
    bool otaActive = false;
    bool otaIntialized = false;
    uint8_t updateProgress = 0;
    unsigned long otaStartTime = 0;

    void handleRoot();
    void handleProgMode();
    void handleNormalMode();
    void handleKnxOff();
    void handleOtaOn();
    void handleOtaOff();
    void handleRestart();
    void handleTftUpdate();
    void handleTftDebug();
    void handleWebUpdateProgress();
    void handleWebUpdateDone();
    void handleNotFound();
    void otaSetup();
    int getRSSIasQuality(int RSSI);

    callbackSetKnxMode *setKnxModeFctn;
    callbackGetKnxMode *getKnxModeFctn;
    callbackStartTftUpdate *startTftUpdateFctn;
    callbackStartTftDebug *startTftDebugFctn;
};

const char* const UPDATE_HTML PROGMEM = R"literal(
<!DOCTYPE html>
<link rel='icon' href='/favicon.ico' sizes='any'>
<body style='width:480px'>
    <h2>ESP Firmware Updater</h2>
    <form method='POST' enctype='multipart/form-data' id='upload-form'>
    <input type='file' id='file' name='upload' accept=".bin,.uf2">
    <input type='submit' value='upload'>
    </form>
    <br>
    <a href="/">back</a>
    <div id='prg' style='width:0;color:white;text-align:center'>0%</div>
</body>
<script>
    var prg = document.getElementById('prg');
    var form = document.getElementById('upload-form');
    form.addEventListener('submit', el=>{
    prg.style.backgroundColor = 'blue';
    el.preventDefault();
    var data = new FormData(form);
    var req = new XMLHttpRequest();
    var fsize = document.getElementById('file').files[0].size;
    req.open('POST', '/upload?size=' + fsize);
    req.upload.addEventListener('progress', p=>{
        let w = Math.round(p.loaded/p.total*100) + '%';
        if(p.lengthComputable){
            prg.innerHTML = w;
            prg.style.width = w;
        }
        if(w == '100%') prg.style.backgroundColor = 'black';
    });
    req.send(data);
    });
</script>
)literal";

const char FAVICON[] PROGMEM = {
    '\x89', '\x50', '\x4E', '\x47', '\x0D', '\x0A', '\x1A', '\x0A', '\x00', '\x00', '\x00', '\x0D', '\x49', '\x48', '\x44', '\x52',
    '\x00', '\x00', '\x00', '\x20', '\x00', '\x00', '\x00', '\x20', '\x08', '\x03', '\x00', '\x00', '\x00', '\x44', '\xA4', '\x8A',
    '\xC6', '\x00', '\x00', '\x01', '\x74', '\x50', '\x4C', '\x54', '\x45', '\x00', '\x00', '\x00', '\x8E', '\x21', '\x26', '\x00',
    '\x00', '\x00', '\x8E', '\x21', '\x26', '\x01', '\x00', '\x00', '\x8E', '\x21', '\x26', '\x05', '\x00', '\x00', '\x8E', '\x21',
    '\x26', '\x00', '\x00', '\x00', '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00', '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00',
    '\x51', '\x12', '\x15', '\x00', '\x00', '\x00', '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x8E',
    '\x21', '\x26', '\x00', '\x00', '\x00', '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00', '\x8E', '\x21', '\x26', '\x00', '\x00',
    '\x00', '\x8E', '\x21', '\x26', '\x8E', '\x21', '\x26', '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00', '\x8E', '\x21', '\x26',
    '\x00', '\x00', '\x00', '\x45', '\x0C', '\x0E', '\x7C', '\x1D', '\x20', '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00', '\x8E',
    '\x21', '\x26', '\x00', '\x00', '\x00', '\x0F', '\x47', '\x3F', '\x00', '\x1E', '\x0E', '\x8E', '\x21', '\x26', '\x0E', '\x9F',
    '\x4C', '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x8E', '\x21', '\x26', '\x8E', '\x21', '\x26',
    '\x1C', '\x71', '\x83', '\x0E', '\x01', '\x01', '\x7C', '\x1D', '\x20', '\x45', '\x0C', '\x0E', '\x8E', '\x21', '\x26', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x0C', '\xB1', '\x4B', '\x8E', '\x21', '\x26', '\x00', '\x00',
    '\x00', '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00', '\x07', '\x00', '\x00', '\x23', '\x58', '\x6F', '\x8E', '\x21', '\x26',
    '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00', '\x4B', '\x59', '\x43', '\x5B', '\x47', '\x4D', '\x8E', '\x21', '\x26', '\x00',
    '\x00', '\x00', '\x0C', '\x90', '\x75', '\x0B', '\x84', '\x8A', '\x00', '\x81', '\x87', '\x03', '\x8A', '\x4B', '\x7C', '\x1D',
    '\x20', '\x03', '\x75', '\x69', '\x20', '\x7B', '\x70', '\x19', '\x8A', '\x65', '\x21', '\x8B', '\x50', '\x00', '\x7F', '\x5A',
    '\x45', '\x0C', '\x0E', '\x1E', '\x58', '\x92', '\x10', '\x55', '\x8E', '\x00', '\x55', '\x80', '\x2A', '\x5C', '\x86', '\x7C',
    '\x1D', '\x20', '\x45', '\x0C', '\x0E', '\x26', '\x66', '\x80', '\x00', '\x5B', '\x73', '\x04', '\x4A', '\x7D', '\x2E', '\x53',
    '\x87', '\x29', '\x6D', '\x73', '\x00', '\x5D', '\x61', '\x8E', '\x21', '\x26', '\x7C', '\x1D', '\x20', '\x45', '\x0C', '\x0E',
    '\x00', '\x00', '\x00', '\x00', '\x5B', '\x51', '\x3C', '\x64', '\x5E', '\x00', '\x3F', '\x6C', '\x00', '\x68', '\x84', '\x3D',
    '\x4B', '\x78', '\x00', '\x74', '\x79', '\x32', '\x5E', '\x76', '\x05', '\x52', '\x68', '\x0C', '\xB0', '\x4A', '\x0C', '\xB1',
    '\x4B', '\x3A', '\x6D', '\x53', '\x06', '\x5B', '\x40', '\x00', '\x32', '\x4A', '\x00', '\x59', '\x3F', '\x61', '\x3B', '\x51',
    '\x8E', '\x21', '\x26', '\x00', '\x00', '\x00', '\x00', '\x74', '\xAE', '\x7C', '\x1D', '\x20', '\x45', '\x0C', '\x0E', '\x00',
    '\x9B', '\x7C', '\x00', '\x80', '\xA2', '\x05', '\x8E', '\x80', '\x00', '\xA0', '\x71', '\x00', '\xAB', '\x5D', '\x0E', '\x01',
    '\x01', '\x00', '\x7C', '\x9E', '\x00', '\x92', '\x75', '\x05', '\xAE', '\x53', '\x08', '\xAB', '\x52', '\x9B', '\x18', '\x3E',
    '\x4B', '\x00', '\x00', '\x00', '\x6D', '\x74', '\x52', '\x4E', '\x53', '\x00', '\x92', '\x92', '\x53', '\x52', '\x0C', '\x0C',
    '\xCC', '\xCC', '\x8F', '\x8F', '\xEA', '\x56', '\x03', '\xF3', '\xF2', '\xEB', '\xBB', '\xB5', '\xB5', '\xAE', '\xAE', '\xA9',
    '\xA9', '\x73', '\x59', '\x36', '\x36', '\x15', '\x15', '\xED', '\xEA', '\xDD', '\xDC', '\x4F', '\x49', '\x2E', '\x09', '\x07',
    '\xFB', '\xF6', '\xF6', '\xE7', '\xBC', '\xBA', '\x93', '\x92', '\x8F', '\x8F', '\x81', '\x81', '\x74', '\x73', '\x72', '\x70',
    '\x70', '\x6B', '\x6B', '\x5B', '\x59', '\x47', '\x3C', '\x3C', '\x2E', '\x26', '\x22', '\x22', '\xFE', '\xFC', '\xFC', '\xF1',
    '\xF1', '\xF0', '\xEF', '\xEE', '\xED', '\xEB', '\xE8', '\xE7', '\xE7', '\xDE', '\xDB', '\xDA', '\xDA', '\xD5', '\xD3', '\xD0',
    '\xCF', '\xCD', '\xC7', '\xC1', '\xC1', '\xC1', '\xC1', '\xA9', '\xA5', '\x9A', '\x99', '\x98', '\x97', '\x91', '\x8E', '\x80',
    '\x67', '\x60', '\x5B', '\x38', '\x38', '\x30', '\xBE', '\xA3', '\x9B', '\x74', '\x00', '\x00', '\x01', '\x89', '\x49', '\x44',
    '\x41', '\x54', '\x38', '\xCB', '\x85', '\xD3', '\xD7', '\x56', '\x02', '\x31', '\x10', '\x06', '\xE0', '\x11', '\x16', '\x29',
    '\x0A', '\xD8', '\xA5', '\x0B', '\x4A', '\xB1', '\x61', '\xC7', '\x02', '\x22', '\xD8', '\x7B', '\xEF', '\xBD', '\x77', '\x13',
    '\x17', '\x17', '\xB0', '\xBD', '\xBC', '\x29', '\xEB', '\x91', '\xC4', '\x0B', '\xBE', '\x9B', '\xB9', '\xD8', '\xFF', '\xEC',
    '\xC9', '\x24', '\x33', '\xC0', '\x99', '\xDA', '\x9D', '\x2E', '\xA0', '\x8E', '\xB7', '\xED', '\xB4', '\x0C', '\x39', '\x3A',
    '\x2A', '\xA1', '\x94', '\xC9', '\x8C', '\x90', '\x11', '\x08', '\x35', '\x47', '\x3F', '\x54', '\x63', '\x6C', '\x21', '\x55',
    '\x60', '\x44', '\xDD', '\xB4', '\xB4', '\xB0', '\x40', '\x2D', '\x56', '\x40', '\x36', '\x89', '\xBA', '\x4A', '\x03', '\x53',
    '\x20', '\xF3', '\xA1', '\x26', '\x1E', '\x00', '\xA2', '\x19', '\xFB', '\x41', '\x36', '\x8C', '\xCC', '\x46', '\x62', '\x3E',
    '\x97', '\x54', '\x14', '\xC5', '\x82', '\x47', '\xE0', '\x57', '\xD4', '\xCE', '\x4A', '\x03', '\xE2', '\xD4', '\xDC', '\x3B',
    '\x66', '\x1A', '\x81', '\xB2', '\x47', '\xC0', '\x69', '\xE6', '\xDF', '\xAB', '\xAC', '\xD6', '\x0A', '\xA2', '\xB7', '\xAF',
    '\xC7', '\x60', '\x30', '\xD8', '\x6C', '\x9D', '\x3C', '\x61', '\x71', '\x00', '\x69', '\xCF', '\xE3', '\x71', '\xA1', '\xA0',
    '\x0F', '\x04', '\xFE', '\x10', '\x1E', '\xF4', '\x7A', '\x49', '\xBB', '\x30', '\x8A', '\x98', '\x7E', '\x90', '\x0C', '\x60',
    '\x66', '\x0C', '\xC0', '\xDD', '\x54', '\x57', '\x67', '\x46', '\x56', '\x39', '\x60', '\xC3', '\x96', '\xFA', '\xFA', '\xE6',
    '\x6A', '\xE0', '\xD2', '\xA8', '\x82', '\x96', '\xEC', '\xD5', '\xFE', '\xDA', '\xFA', '\xDE', '\x45', '\x18', '\x08', '\x03',
    '\x9E', '\x28', '\x89', '\xB7', '\xD2', '\xC0', '\xCB', '\xE6', '\xAB', '\x6E', '\xE3', '\x89', '\x04', '\x84', '\xCB', '\x1C',
    '\x27', '\x81', '\xB8', '\xA6', '\x69', '\x3B', '\x37', '\xA9', '\xD4', '\xED', '\xEE', '\x87', '\xA6', '\x5D', '\x92', '\x40',
    '\x9B', '\xF4', '\x87', '\xF8', '\xCC', '\x01', '\xE8', '\x0E', '\x67', '\xAF', '\xFF', '\x02', '\x47', '\x55', '\x84', '\x53',
    '\x3F', '\xE4', '\xF9', '\x62', '\x3E', '\xBF', '\x70', '\xA6', '\x1F', '\xD2', '\x51', '\x43', '\x9C', '\x42', '\x10', '\x51',
    '\x2C', '\x90', '\x9D', '\x7E', '\x63', '\x3E', '\xC3', '\x2C', '\xC0', '\x84', '\x00', '\x4C', '\x84', '\x9B', '\x75', '\x91',
    '\x29', '\x2C', '\x3D', '\x00', '\x3C', '\xAE', '\x14', '\x9E', '\x59', '\x17', '\xC9', '\x4A', '\xA2', '\xE4', '\x90', '\x5C',
    '\x26', '\x0C', '\xDC', '\xFF', '\x43', '\x32', '\xCB', '\xC5', '\xE2', '\x1C', '\xAD', '\x72', '\x9B', '\x69', '\x3D', '\x10',
    '\xFD', '\x8E', '\xC5', '\xBE', '\x22', '\x20', '\x5C', '\x94', '\x78', '\xD5', '\x89', '\xC4', '\xFD', '\x9D', '\x78', '\xD5',
    '\x65', '\x1F', '\xAB', '\xEC', '\x73', '\x97', '\x1D', '\x98', '\xB2', '\x23', '\x27', '\x0D', '\xAD', '\x5B', '\x0D', '\x28',
    '\xE2', '\xD0', '\xCA', '\x63', '\xAF', '\xAE', '\x8A', '\x63', '\x2F', '\x2F', '\x8E', '\x49', '\x0D', '\x80', '\xB0', '\x38',
    '\xE2', '\xEA', '\xF1', '\x80', '\xB0', '\x7A', '\xC2', '\xF2', '\xB2', '\x40', '\x0E', '\xC4', '\xE5', '\x95', '\xD6', '\xDF',
    '\xBE', '\x75', '\x02', '\xE2', '\xFA', '\xFF', '\x00', '\xFA', '\xFE', '\x6F', '\xD1', '\x22', '\x63', '\x63', '\x1C', '\x00',
    '\x00', '\x00', '\x00', '\x49', '\x45', '\x4E', '\x44', '\xAE', '\x42', '\x60', '\x82'};
