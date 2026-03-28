var external = null;

String.prototype.format = function() {  
    if(arguments.length == 0) return this;  
    var obj = arguments[0];  
    var s = this;  
    for(var key in obj) {  
        s = s.replace(new RegExp("\\{\\{" + key + "\\}\\}", "g"), obj[key]);  
    }  
    return s;  
}

new QWebChannel(qt.webChannelTransport, function(channel) {
    external = channel.objects.external;
});

function appendHtml(msg){
    var data = (typeof msg === "string") ? JSON.parse(msg) : msg;
    if (!data.HEAD) {
        data.HEAD = "qrc:/Resources/MainWindow/girl.png";
    }
    $("#placeholder").append(external.msgRHtmlTmpl.format(data));
    window.scrollTo(0, document.body.scrollHeight);
};
