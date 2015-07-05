
'use strict';

var client = (function() {
    var client = {};
    var clientId = undefined;
    var webChannel = undefined;
    var connection = undefined;
    var pendingCalls = [];

    var methods = [
        'debugLog',
        'appendNick',
        'contextMenu',
        'appendText',
        'setTopic',
    ];

    function createDefaultCallback(name) {
        return function() {
            var originalArgs = Array.prototype.slice.call(arguments);
            pendingCalls.push(function() {
                client[name].apply(undefined, originalArgs);
            });
        }
    }

    for (var i = 0; i < methods.length; ++i) {
        var method = methods[i];
        client[method] = createDefaultCallback(method);
    }

    client.zoomImage = function(element) {
        var fullClass = 'fullSizeImage';
        var scaledClass = 'scaledToFitImage';

        if (element.classList.contains(fullClass)) {
            element.classList.add(scaledClass);
            element.classList.remove(fullClass);
        } else if (element.classList.contains(scaledClass)) {
            element.classList.add(fullClass);
            element.classList.remove(scaledClass);
        } else {
            return false;
        }

        alignChat(true);
    };

    client.setDefaultFont = function(family, size) {
        var body = document.body;
        body.style.fontFamily = family;
        body.style.fontSize = size + 'px';
    };

    client.markMessage = function(id, success) {
        var element = document.querySelector('#message' + id);
        if (element) {
            if (success) {
                element.classList.remove('notDelivered');
                element.classList.add('delivered');
            } else {
                element.classList.add('failedToDeliver');
            }
        }
    };

    client.initQuickChat = function(id, wsUri) {
        clientId = id;

        window.onload = function() {
            var socket = new WebSocket(wsUri);
            console.log('creating websocket');

            socket.onclose = function() {
                console.error("web channel closed");
            };
            socket.onerror = function(error) {
                console.error("web channel error: " + error);
            };
            socket.onopen = function() {
                webChannel = new QWebChannel(socket, function(channel) {
                    connection = channel.objects.client;

                    function createRealCallback(name) {
                        return connection[name].bind(undefined, clientId);
                    };

                    for (var i = 0; i < methods.length; ++i) {
                        var method = methods[i];
                        client[method] = createRealCallback(method);
                    }

                    var pending = pendingCalls;
                    pendingCalls = [];

                    for (i = 0; i < pending.length; ++i)
                        pending[i]();
                });
            }
        }
    };

    return client;
})();
