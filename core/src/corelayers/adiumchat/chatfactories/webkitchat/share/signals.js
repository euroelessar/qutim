(function() {
     try {
         window['setStylesheet'] = setStylesheet;
         window['setCustomStylesheet'] = setCustomStylesheet;
         window['appendMessage'] = appendMessage;
         window['appendNextMessage'] = appendNextMessage;
         window['addSeparator'] = function() {
                     var separator = document.getElementById("separator");
                     if (separator)
                         separator.parentNode.removeChild(separator);
                     window['appendMessage']("<hr id='separator'><div id='insert'></div>");
                 };
         if (window['clientConnector'] !== undefined)
             return false;
         var connector = {
             "methods": [
                 'setStylesheet',
                 'setCustomStylesheet',
                 'appendMessage',
                 'appendNextMessage',
                 'addSeparator'
             ],
             "init": function() {
                         var methods = connector.methods;
                         for (var i = 0; i < methods.length; ++i) {
                             var signal = methods[i] + 'Request';
                             var method = methods[i];
                             var object = { id: method };
                             client.debug('Trying to connect ' + signal + ' to ' + method);
                             client[signal].connect(object, function () {
                                                                try {
                                                                    var method = this.id;
                                                                    if (window[method] !== undefined)
                                                                        window[method](arguments[0], arguments[1]);
                                                                } catch (e) {
                                                                    client.debug(JSON.stringify(e));
                                                                }
                                                            });
                         }
                     }
         };
         window.clientConnector = connector;
         connector.init();
         return true;
     } catch (e) {
         return 'Exception: ' + e.toString();
     }
 })();
