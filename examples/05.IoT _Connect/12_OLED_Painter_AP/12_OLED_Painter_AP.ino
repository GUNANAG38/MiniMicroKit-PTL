#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

const char* ap_pass = "12345678"; // รหัสผ่าน WiFi ของบอร์ด

WebServer server(80);
WebSocketsServer webSocket(81); // พอร์ต 81 สำหรับรับข้อมูลวาดภาพแบบเรียลไทม์

// หน้าเว็บ HTML ในตัวบอร์ด (เปิดผ่าน http://192.168.4.1)
const char MAIN_page[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="th">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no,viewport-fit=cover">
<title>OLED Touch Painter - Direct AP</title>
<style>
:root {
    --bg-color: #f8fafc;
    --card-bg: #ffffff;
    --text-main: #0f172a;
    --text-muted: #64748b;
    --accent: #2563eb;
    --border: #e2e8f0;
    --shadow: 0 4px 6px -1px rgb(0 0 0 / 0.1);
}
[data-theme='dark'] {
    --bg-color: #0f172a;
    --card-bg: #1e293b;
    --text-main: #f8fafc;
    --text-muted: #94a3b8;
    --accent: #3b82f6;
    --border: #334155;
    --shadow: 0 4px 6px -1px rgb(0 0 0 / 0.3);
}
* { box-sizing: border-box; touch-action: none; }
html, body {
    margin: 0; padding: 0; width: 100vw; height: 100vh; height: 100dvh;
    overflow: hidden; background-color: var(--bg-color); color: var(--text-main);
    font-family: sans-serif; display: flex; justify-content: center; align-items: center;
}
.app-container {
    width: 100%; max-width: 480px; height: 100%; max-height: 100vh;
    padding: 12px; display: flex; flex-direction: column; gap: 8px; justify-content: space-between;
}
.card {
    background: var(--card-bg); border: 1px solid var(--border); border-radius: 12px;
    padding: 12px; box-shadow: var(--shadow); display: flex; flex-direction: column; gap: 6px;
}
.canvas-box {
    width: 100%; aspect-ratio: 128 / 64; background: #000000;
    border: 3px solid var(--border); border-radius: 8px; position: relative; overflow: hidden; cursor: crosshair;
}
canvas { width: 100%; height: 100%; display: block; }
.controls { display: flex; gap: 6px; }
button {
    padding: 12px; border-radius: 8px; border: none; font-weight: 600; font-size: 0.9rem; cursor: pointer;
    transition: transform 0.1s, background 0.2s;
}
button:active { transform: scale(0.97); }
.btn-primary { background: var(--accent); color: white; flex: 2; }
.btn-danger { background: #ef4444; color: white; flex: 1; }
.btn-secondary { background: var(--border); color: var(--text-main); font-size: 0.75rem; padding: 4px 10px; }
.result-footer { display: flex; justify-content: space-between; align-items: center; }
</style>
</head>
<body data-theme="light">
<div class="app-container">
    <section class="card" style="align-items: center; text-align: center;">
        <span style="font-weight: 700; font-size: 1.1rem;">OLED Touch Painter</span>
        <span style="font-size: 0.75rem; color: var(--text-muted);">GelectronLAB • Mini MicroKit PT-L V1.0</span>
    </section>
    <section class="card" style="flex: 1; justify-content: center; align-items: center;">
        <div style="width: 100%; display: flex; justify-content: space-between; align-items: center; font-size: 0.8rem; font-weight: 600; color: var(--text-muted);">
            <span>Virtual OLED (128x64)</span>
            <button class="btn-secondary" onclick="toggleTheme()">Switch Theme</button>
        </div>
        <div class="canvas-box"><canvas id="drawCanvas"></canvas></div>
        <div class="controls" style="width: 100%; margin-top: 4px;">
            <button class="btn-danger" onclick="clearCanvas()">ลบหน้าจอ</button>
            <button class="btn-primary" onclick="sendDrawing()">กดส่งไปที่บอร์ด</button>
        </div>
    </section>
    <section class="card">
        <div class="result-footer">
            <span style="font-size: 0.75rem; color: var(--text-muted);">Status : <b id="ws-status" style="color: #ef4444;">Connecting...</b></span>
            <span style="font-size: 0.75rem; color: var(--accent);" id="send-status">พร้อมใช้งาน</span>
        </div>
    </section>
</div>
<script>
window.addEventListener('DOMContentLoaded', () => {
    const savedTheme = localStorage.getItem("theme") || "light";
    document.body.dataset.theme = savedTheme;
    initCanvas();
    initWebSocket();
});
function toggleTheme() {
    const body = document.body;
    const currentTheme = body.dataset.theme === 'dark' ? 'light' : 'dark';
    body.dataset.theme = currentTheme;
    localStorage.setItem("theme", currentTheme);
}
const canvas = document.getElementById('drawCanvas');
const ctx = canvas.getContext('2d');
let painting = false;
let ws;

function initCanvas() {
    canvas.width = 128; canvas.height = 64;
    ctx.strokeStyle = '#ffffff'; ctx.lineWidth = 2; ctx.lineCap = 'round';
    ctx.fillStyle = '#000000'; ctx.fillRect(0, 0, canvas.width, canvas.height);
}
function getPos(e) {
    const rect = canvas.getBoundingClientRect();
    const clientX = e.touches ? e.touches[0].clientX : e.clientX;
    const clientY = e.touches ? e.touches[0].clientY : e.clientY;
    return {
        x: (clientX - rect.left) * (canvas.width / rect.width),
        y: (clientY - rect.top) * (canvas.height / rect.height)
    };
}
canvas.addEventListener('mousedown', (e) => { painting = true; draw(e); });
canvas.addEventListener('mouseup', () => { painting = false; ctx.beginPath(); });
canvas.addEventListener('mousemove', draw);
canvas.addEventListener('touchstart', (e) => { painting = true; draw(e); e.preventDefault(); });
canvas.addEventListener('touchend', () => { painting = false; ctx.beginPath(); });
canvas.addEventListener('touchmove', (e) => { draw(e); e.preventDefault(); });

function draw(e) {
    if (!painting) return;
    const pos = getPos(e);
    ctx.lineTo(pos.x, pos.y);
    ctx.stroke();
    ctx.beginPath();
    ctx.moveTo(pos.x, pos.y);
}
function clearCanvas() {
    ctx.fillStyle = '#000000'; ctx.fillRect(0, 0, canvas.width, canvas.height);
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send("CLEAR");
    }
    document.getElementById("send-status").textContent = "ลบหน้าจอแล้ว";
}
function initWebSocket() {
    ws = new WebSocket('ws://' + window.location.hostname + ':81/');
    ws.onopen = function() {
        document.getElementById("ws-status").textContent = "Connected";
        document.getElementById("ws-status").style.color = "#16a34a";
    };
    ws.onclose = function() {
        document.getElementById("ws-status").textContent = "Disconnected";
        document.getElementById("ws-status").style.color = "#ef4444";
        setTimeout(initWebSocket, 3000);
    };
}
function sendDrawing() {
    if (!ws || ws.readyState !== WebSocket.OPEN) {
        alert("ยังไม่ได้เชื่อมต่อกับบอร์ด!");
        return;
    }
    const imgData = ctx.getImageData(0, 0, 128, 64);
    const data = imgData.data;
    let pixelString = "";
    for (let i = 0; i < data.length; i += 4) {
        let avg = (data[i] + data[i+1] + data[i+2]) / 3;
        pixelString += (avg > 128) ? "1" : "0";
    }
    ws.send(pixelString);
    document.getElementById("send-status").textContent = "ส่งภาพไปที่บอร์ดแล้ว!";
    document.getElementById("send-status").style.color = "#16a34a";
}
</script>
</body>
</html>
)rawliteral";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        String message = "";
        for (size_t i = 0; i < length; i++) {
            message += (char)payload[i];
        }
        message.trim();

        if (message == "CLEAR") {
            kit.clearDisplay();
            kit.displayUpdate();
            kit.setLED(false, false, false);
            kit.playSystemSound(SOUND_NOTIFY);
        } 
        else if (message.length() >= 8192) {
            kit.clearDisplay();
            int idx = 0;
            for (int y = 0; y < 64; y++) {
                for (int x = 0; x < 128; x++) {
                    if (idx < message.length()) {
                        if (message.charAt(idx) == '1') {
                            kit.display.drawPixel(x, y, SSD1306_WHITE);
                        }
                    }
                    idx++;
                }
            }
            kit.displayUpdate();
            kit.setLED(false, false, true); // ไฟเขียว
            kit.playSystemSound(SOUND_SUCCESS);
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    
    kit.begin();
    kit.showDialog("AP Mode", "Starting...", 800);

    // สร้างชื่อ WiFi ไม่ให้ซ้ำกันโดยอ้างอิงจาก Chip ID ของบอร์ดแต่ละตัว
    uint64_t chipid = ESP.getEfuseMac();
    String ssidName = "Painter-" + String((uint32_t)(chipid & 0xFFFF), HEX);

    // เปิด Wi-Fi Access Point
    WiFi.softAP(ssidName.c_str(), ap_pass);
    IPAddress myIP = WiFi.softAPIP();

    // ตั้งค่า Web Server
    server.on("/", []() {
        server.send_P(200, "text/html", MAIN_page);
    });
    server.begin();

    // ตั้งค่า WebSocket Server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    // แสดงชื่อ Wi-Fi และ IP บนจอ OLED ของบอร์ดนั้นๆ
    kit.clearDisplay();
    kit.printText("WiFi: " + ssidName, 12, 5, 1);
    kit.printText("Pass: 12345678", 20, 25, 1);
    kit.printText("IP: 192.168.4.1", 18, 45, 1);
    kit.displayUpdate();
}

void loop() {
    kit.update();
    server.handleClient();
    webSocket.loop();
}