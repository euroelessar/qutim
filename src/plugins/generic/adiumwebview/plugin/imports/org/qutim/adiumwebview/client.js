var client = (function () {
    function sendMessage(name, data) {
        navigator.qt.postMessage(JSON.stringify({ name: name, data: data }));
    }

    function debugLog() {
        var data = [];
        for (var i = 0; i < debugLog.arguments.length; ++i)
            data.push(debugLog.arguments[i]);

        sendMessage('debugLog', data);
    }
    
    return {
        debugLog: debugLog
    };
})();

client.debugLog("How are you?", document.title);
