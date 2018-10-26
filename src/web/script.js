var ws;

window.onload = function() {
    ws = new WebSocket("wss://localhost:8008");
    ws.onopen = function (e) {
        console.log("opened");
    };

    ws.onclose = function (e) {
        console.log("closed");
    }
};