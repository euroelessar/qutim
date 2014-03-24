var pendingScripts = [];
var loading = false;

function addScript(script) {
    pendingScripts.push(script);
}

function takeScripts() {
    var result = pendingScripts;
    pendingScripts = [];
    return result;
}
