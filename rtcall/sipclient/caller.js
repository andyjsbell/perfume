/// This caller.js is derived from sip-js project
/// uskee.org
///

function getFlashMovie(name) {
    var isIE = navigator.appName.indexOf("Microsoft") != -1;
    return (isIE) ? window[name] : document[name];
}

function getQuerystring(key, default_) {
    if (default_==null)
        default_=""; 
    key = key.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");
    var regex = new RegExp("[\\?&]"+key+"=([^&#]*)");
    var qs = regex.exec(window.location.href);
    return (qs == null ? default_ : qs[1]);
}

function cleanHTML(value) {
    return ("" + value).replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

function Caller() {
    // properties in config
    this.displayname = 'First Last';
    this.username = 'myname';
    this.domain = 'localhost';
    this.authname =  'myname';
    this.password = '';
    this.transport = 'udp';
    
    // properties in register
    this.outbound = 'proxy';
    this.outbound_proxy_address ='127.0.0.1:5060';
    this.register_interval = 180;
    this.rport = true;
    this.sipoutbound = false;
    this.local_aor = '"' + this.displayname + '" <sip:' + this.username + '@' + this.domain + '>';
    this.sock_state = "idle";
    this.register_state = "not registered";
    this.register_button = 'Register';

    // properties in call
    this.call_state = "idle";
    // possible state values:
    //     idle - not in a call (may not have socket)
    // outbound call:
    //     waiting - want to initiate a call, and waiting for socket connection
    //     inviting - sending outbound INVITE, may be waiting for media
    //     ringback - received 18x response for outbound INVITE
    //     accepted - received 2xx response for outbound INVITE
    //     active - sent ACK for outbound INVITE
    // incoming call:
    //     incoming - received incoming INVITE
    //     accepting - accepted an incoming INVITE, waiting for media
    // failure, termination
    //     failed - outbound/inbound call failed due to some reason
    //     closed - call is closed by remote party

    this.target_scheme = "sip";
    this.target_value = "yourname@localhost";
    this.target_aor = this.target_scheme + ":" + this.target_value;
    this.has_audio = true;
    this.has_tones = false;
    this.has_video = true;
    this.has_text = false;
    this.has_location = false;
    this.location = null;
    
    // properties in network
    this.network_status = null;
    
    //properties in program log
    this.log_scroll = true;

    // private attributes
    this._handlers = {};
    this.network_type = "WebRTC"; // WebRTC(default) or Flash
    this.listen_ip = null;
    this._listen_port = null;
    this._stack = null;
    this._sock = null;
    this._next_message = null;
    
    // SIP headers
    this.user_agent = "sip-js/1.0";
    this.server = this.user_agent;
    
    // media context
    this._local_sdp = null;
    this._remote_sdp = null;
    this._rtp = []; // RealTimeSocket instances
    this._gw = null;
    
    // HTML5
    this.has_html5_websocket = false;
    this.has_html5_video = false;
    this.has_html5_webrtc = false;
    this.websocket_path = "/sip";
    this._webrtc_local_stream = null;
    this._webrtc_peer_connection = null;
    this.enable_sound_alert = false;
    this.webrtc_servers = "stun://stun.l.google.com:19302"; // comma separated list, each item is either "url" or "url|credential"
    this._sdp_timeout = 500; // 0.5s after calling createOffer or createAnswer, use the SDP
    
    // SIP requirements for websocket
    this._instance_id = "";
    this._gruu = "";
};

Caller.prototype.populate = function() {
    for (var attr in this) {
        var def = this[attr];
        if ((typeof def != "function") && attr.charAt(0) != "_") {
            var param = getQuerystring(attr, def);
            if (typeof this[attr] == "number")
                param = (typeof param == "number" ? param : parseInt(param));
            else if (typeof this[attr] == "boolean")
                param = (typeof param == "boolean" ? param : (param != "false" ? true : false));
            else
                param = unescape(param);
            if (def == param)
                this.dispatchEvent({"type": "propertyChange", "property": attr, "newValue": param});
            else
                this.setProperty(attr, param);
        }
    }
};

/// For <div id="webrtc-network">, <div id="local-video">, <div id="remote-video"> with HTML5 setttings
Caller.prototype.detectHTML5 = function() {
    this.setProperty("has_html5_websocket", typeof WebSocket != "undefined");
    this.setProperty("has_html5_video", !!document.createElement('video').canPlayType);
    this.setProperty("has_html5_webrtc", typeof navigator.webkitGetUserMedia != "undefined" && typeof webkitRTCPeerConnection != "undefined");
    log("detecting HTML support websocket=" + this.has_html5_websocket + " video=" + this.has_html5_video + " webrtc=" + this.has_html5_webrtc);
    if (!this.has_html5_websocket || !this.has_html5_video || !this.has_html5_webrtc) {
        $("webrtc-network").innerHTML += '<font color="red">Some HTML5 features are missing in your browser</font>';
    }
    
    if (this.has_html5_websocket) {
        // SIP over websocket works
        this.setProperty("network_status", "available");
        this.listen_ip = 'r' + Math.floor(Math.random() * 10000000000) + ".invalid";
        this._listen_port = 0;

        if (this.outbound_proxy_address == "127.0.0.1:5060") {
            var outbound_proxy_address = "127.0.0.1:5080";
            if (window.location.href) {
                var uri = sip.parse_uri(window.location.href);
                outbound_proxy_address = uri.host + ":5080";
            }
            this.setProperty("outbound_proxy", true);
            this.setProperty("outbound_proxy_address", outbound_proxy_address);
        }
    }
    
    if (this.has_html5_video) {
        // add <video> to local and remote video boxes
        var local = document.createElement("video");
        local.id = "html5-local-video";
        local.style.width = "240";
        local.style.height = "168";
		// local.style.backgroundColor = "#000000";
        local.autoplay = "autoplay";
        $('local-video').appendChild(local);
        
        var remote = document.createElement("video");
        remote.id = "html5-remote-video";
        remote.style.width = "240";
        remote.style.height = "168";
		// remote.style.backgroundColor = "#000000";
        remote.autoplay = "autoplay";
        $('remote-video').appendChild(remote);
        
        var audio = document.createElement("audio");
        audio.id = "html5-audio";
        audio.autoplay = "autoplay";
        $("webrtc-network").appendChild(audio);
    }
};

/// SIP event handler
Caller.prototype.addEventListener = function(type, handler) {
    if (this._handlers[type] === undefined)
        this._handlers[type] = [];
    this._handlers[type].push(handler);
};

/// SIP event dispatch
Caller.prototype.dispatchEvent = function(event) {
//    log("dispatchEvent(" + event.type + ", " + event.property + ", " + event.newValue + ")");
    if (this._handlers[event.type] !== undefined) {
        var handlers = this._handlers[event.type];
        for (var i=0; i<handlers.length; ++i) {
            var handler = handlers[i];
            handler(event);
        }
    }
};

/// SIP property settings
Caller.prototype.setProperty = function(name, value) {
    if (this[name] !== undefined) {
        var oldValue = this[name];
        if (oldValue != value) {
            this[name] = value; // set property with new value, e.g. this.displayname
            this.dispatchEvent({"type": "propertyChange", "property": name, "newValue": value, "oldValue": oldValue});
        }
        
        if (name == "username" || name == "domain" || name == "displayname") {
            this.setProperty("local_aor", '"' + this.displayname + '" <sip:' + this.username + '@' + this.domain + '>');
        }
        else if (name == "target_scheme") {
            log("target_scheme=" + value);
            var target_value = {"sip": "yourname@" + this.domain, "tel" : "+12125551234", "urn": "service:sos"}[value];
            this.setProperty("target_value", target_value);
            this.setProperty("target_aor", this.target_scheme + ":" + this.target_value);
        }
        else if (name == "target_value") {
            this.setProperty("target_aor", this.target_scheme + ":" + this.target_value);
        }
        else if (name == "network_type") {
            if (value == "Flash" && this.transport == "ws") {
                this.setProperty("transport", "udp");
            }
            else if (value == "WebRTC" && this.transport != "ws") {
                this.setProperty("transport", "ws");
            }
        }
    }
    else {
        this.dispatchEvent({"type": "propertyChange", "property": name, "newValue": value});
    }
};

/// SIP signal status changed
Caller.prototype.statusChanged = function(value) {
    this.setProperty("network_status", value);
    var enable = (value == "connected");
    
    if (enable) {
		//TODO: SIP is connected and do something for it here
    }
    else {
        // disable all edits and reset
        this.setProperty("sock_state", "idle");
        this.setProperty("register_state", "not registered");
        this.setProperty("call_state", "idle");
        this._listen_port = null;
        this._stack = null;
        this._sock = null;
        this._local_sdp = null;
        this._remote_sdp = null;
        this._rtp = []; // RealTimeSocket instances
        this._gw = null;
    }
};

/// SIP network status changed
Caller.prototype.networkChanged = function() {
    for (var i=0; i<network.interfaces.length; ++i) {
        var intf = network.interfaces[i];
        if (intf.active) {
            for (var j=0; j<intf.addresses.length; ++j) {
                var addr = intf.addresses[j];
                if (addr.ipVersion == "IPv4") {
                    this.setProperty("listen_ip", addr.address);
                    break;
                }
            }
        }
        if (this.listen_ip)
            break;
    }
};

/// SIP register
Caller.prototype.register = function() {
    log("register() " + this.local_aor);
    if (this.sock_state == "idle") {
        this.setProperty("sock_state", "creating");
        this.setProperty("register_state", "waiting");
        this.setProperty("register_button", "Unregister");
    
        this.createSocket();
    }
    else if (this.sock_state == "bound" || this.sock_state == "connected") {
        if (this._reg && this.register_state != "not registered") {
            this.setProperty("register_state", "unregistering");
        
            this.sendUnregister();
        }
        else if (this.register_state == "not registered") {
            this.setProperty("register_state", "registering");
        
            this.sendRegister();
        }
        else {
            log("ignoring register in state " + this.register_state + " " + this._reg);
        }
    }
};

/// SIP switch network type: WebRTC or Flash
Caller.prototype.switchNetworkType = function() {
    var current = this.network_type;
    var other = (current == 'Flash' ? 'WebRTC' : 'Flash');
	this.network_type = other;
    return true;
};

/// SIP signal send invite
Caller.prototype.call = function() {
    log("call() " + this.target_aor);
    if (this.sock_state == "idle") {
        this.setProperty("sock_state", "creating");
        this.setProperty("call_state", "waiting");
        this.createSocket();
    }
    else if (this.sock_state == "bound" || this.sock_state == "connected") {
        if (this.call_state == "idle") {
            this.setProperty("call_state", "inviting");
            this.sendInvite();
        }
        else if (this.call_state == "incoming") {
            this.sendInviteResponse(200, 'OK');
        }
        else {
            this.dispatchMessage("End the existing call first");
        }
    }
};

/// SIP signal send cancel/bye
Caller.prototype.end = function() {
    log("end()");
    if (this.call_state != "idle") {
        if (this.call_state == "inviting" || this.call_state == "ringback") {
            this.sendCancel();
        }
        else if (this.call_state == "incoming") {
            this.sendInviteResponse(603, 'Decline');
        }
        else if (this.call_state == "active" || this.call_state == "accepted" || this.call_state == "accepting") {
            this.sendBye();
        }
        else {
            if (this.call_state != "failed" && this.call_state != "closed") {
                log("ignoring end in " + this.call_state + " state");
            }
            this.hungup();
        }
        this.setProperty("call_state", "idle");
    }
};

Caller.prototype.createUUID4 = function() {
    return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
        var r = Math.random()*16|0, v = c == 'x' ? r : (r & 0x3 | 0x8);
        return v.toString(16);
    });
};

Caller.prototype.createInstanceId = function() {
    if (!this._instance_id && typeof localStorage != "undefined") {
        this._instance_id = localStorage.getItem("instance_id");
        if (!this._instance_id) {
            this._instance_id = "<urn:uuid:" + this.createUUID4() + ">";
            localStorage.setItem("instance_id", this._instance_id);
        }
    }
};

Caller.prototype.createSocket = function() {
    log("createSocket() transport=" + this.transport);
    if (this.transport == "udp") { // Flash network
        this._sock = new network.DatagramSocket();
        var parent = this;
        this._sock.addEventListener("propertyChange", function(event) { parent.onSockPropertyChange(event); });
        this._sock.addEventListener("data", function(event) { parent.onSockData(event); });
        this._sock.addEventListener("ioError", function(event) { parent.onSockError(event); });
        this._sock.bind(0, "0.0.0.0");
        this._sock.receive();
    }
    else if (this.transport == "ws") { // WebRTC network
        log("  connecting to " + this.outbound_proxy_address);
        try {
            this._sock = new WebSocket('ws://' + this.outbound_proxy_address + this.websocket_path, ["sip"]);
            var parent = this;
            this._sock.onopen = function() { parent.onWebSocketOpen(); };
            this._sock.onclose = function() { parent.onWebSocketClose(); };
            this._sock.onerror = function(error) { parent.onWebSocketError(error); };
            this._sock.onmessage = function(msg) { parent.onWebSocketMessage(msg); };
        } catch (error) {
            log("error in websocket: " + error, "error");
        }
    }
    else {
        log(this.transport + " transport is not yet implemented", "error");
        this.setProperty("sock_state", "idle");
        if (this.register_state == "waiting") {
            this.setProperty("register_state", "not registered");
            this.setProperty("register_button", "Register");
        }
        if (this.call_state == "waiting") {
            this.setProperty("call_state", "idle");
        }
    }
};

Caller.prototype.onSockPropertyChange = function(event) {
    if (event.property == "bound") {
        if (event.newValue) {
            //this._listen_port = sock.localPort;
            //log("listen_port=" + this._listen_port);
        }
        else {
            listen_port = null;
            this.resetSockState();
        }
    }
    else if (event.property == "localPort") {
        if (event.newValue) {
            this._listen_port = this._sock.localPort;
            log("listen_port=" + this._listen_port);
            this.setProperty("sock_state", "bound");
            this.createStack();
        }
    }
    else if (event.property == "connected") {
        // TCP or WS socket
        log("socket connected=" + event.newValue);
        if (event.newValue) {
            this.setProperty("sock_state", "connected");
            this.createStack();
        }
        else {
            this.resetSockState();
        }
    }
};

Caller.prototype.resetSockState = function() {
    this.setProperty("sock_state", "idle");
    if (this.register_state == "waiting" || this.register_state == "registered") {
        this.setProperty("register_state", "not registered");
        this.setProperty("register_button", "Register");
        this._reg = null;
    }
    if (this.call_state == "waiting") {
        this.setProperty("call_state", "idle");
    }
    
};

/// SIP signal statck creating
Caller.prototype.createStack = function() {
    var transport = new sip.TransportInfo(this.listen_ip, this._listen_port, this.transport, this.transport == "tls", this.transport != "udp", this.transport != "udp");
    this._stack = new sip.Stack(this, transport);
    
    if (this.register_state == "waiting") {
        this.setProperty("register_state", "registering");
        this.sendRegister();
    }
    
    if (this.call_state == "waiting") {
        this.setProperty("call_state", "inviting");
        this.sendInvite();
    }
    
    if (this._next_message != null) {
        this.sendMessage(this._next_message);
        this._next_message = null;
    }
};

/// SIP signal send request
Caller.prototype.sendRegister = function() {
    if (this._reg == null) {
        this._reg = new sip.UserAgent(this._stack);
        this._reg.remoteParty = new sip.Address(this.local_aor);
        this._reg.localParty = new sip.Address(this.local_aor);
        if (this.outbound == "proxy") {
            var outbound_proxy = this.getRouteHeader();
            if (this.transport != "udp")
                outbound_proxy.value.uri.param['transport'] = this.transport;
            this._reg.routeSet = [outbound_proxy];
            // For REGISTER should we change uri instead of routeSet?
            //this._reg.remoteTarget = new sip.URI("sip:" + this.username + "@" + this.outbound_proxy_address);
        }
    } 
    
    var m = this.createRegister();
    m.setItem('Expires', new sip.Header("" + this.register_interval, 'Expires'))
    this._reg.sendRequest(m);
};

/// SIP signal create register message
Caller.prototype.createRegister = function() {
    var m = this._reg.createRequest('REGISTER');
    var c = new sip.Header(this._stack.uri.toString(), 'Contact');
    c.value.uri.user = this.username;
    if (this.transport == "ws" || this.transport == "wss") {
        this.createInstanceId();
        c.setItem('reg-id', '1');
        c.setItem('+sip.instance', this._instance_id);
        m.setItem('Supported', new sip.Header('path, outbound, gruu', 'Supported'));
    }
    m.setItem('Contact', c);
    return m;
};

/// SIP signal create unregister message
Caller.prototype.sendUnregister = function() {
    var m = this.createRegister();
    m.setItem('Expires', new sip.Header("0", 'Expires'))
    this._reg.sendRequest(m);
};

/// SIP signal register callback
Caller.prototype.receivedRegisterResponse = function(ua, response) {
    if (response.isfinal()) {
        if (this.register_state == "registering") {
            if (response.is2xx()) {
                this.setProperty("register_state", "registered");
                this.setProperty("register_button", "Unregister");
            }
            else {
                this.setProperty("register_state", "not registered");
                this.setProperty("register_button", "Register");
                this._reg = null;
            }
        }
        else if (this.register_state == "unregistering") {
            this.setProperty("register_state", "not registered");
            this.setProperty("register_button", "Register");
            this._reg = null;
        }
    }
};

/// SIP signal create connection
Caller.prototype.sendInvite = function() {
    if (this._call == null) {
        this._call = new sip.UserAgent(this._stack);
        this._call.remoteParty = new sip.Address(this.target_aor);
        this._call.localParty = new sip.Address(this.local_aor);
        if (this.outbound == "proxy") {
            this._call.routeSet = [this.getRouteHeader(this._call.remoteParty.uri.user)];
        }
    } 
    
    this.dispatchMessage("Inviting " + this._call.remoteParty.toString() + " ...");
    
    if (this.network_type == "WebRTC") {
        this.createWebRtcConnection();
    }
    else {
        this.createMediaSockets();
    }
};

/// SIP set video property for HTML5 <video>:  local-video or remote-video
Caller.prototype.setVideoProperty = function(videoname, attr, value) {
    if (this.network_type == "WebRTC") {
        log("set " + videoname + "." + attr + " = " + value);
        var obj = $("html5-" + videoname);
        if (obj) {
            if (attr == "controls") {
                obj.controls = value;
            }
            else if (attr == "live" && this.has_html5_webrtc) {
                if (value) {
                    log("local-stream=" + (this._webrtc_local_stream === null));
                    if (this._webrtc_local_stream == null) {
                        var phone = this;
                        try {
                            navigator.webkitGetUserMedia({"video": true, "audio": true},
                                function(stream) { phone.onUserMediaSuccess(stream); },
                                function(error) { phone.onUserMediaError(error); });
                        }
                        catch (e) {
                            // try older style
                            navigator.webkitGetUserMedia("video,audio",
                                function(stream) { phone.onUserMediaSuccess(stream); },
                                function(error) { phone.onUserMediaError(error); });
                        }
                    }
                    else {
                        this.onUserMediaSuccess(this._webrtc_local_stream);
                    }
                }
                else {
                    obj.setAttribute('src', '');
                }
            }
            else {
                log("ignoring set property '" + attr + "' on '" + videoname + '"');
            }
        }
        else {
            log("cannot get video object of id 'html5-" + videoname + "' to set property '" + attr + "'");
        }
    }
    else {
        var obj = getFlashMovie(videoname);
        if (obj) {
            obj.setProperty(attr, value);
        }
        else {
            log("cannot get video object of name '" + videoname + "' to set property '" + attr + "'");
        }
    }
};

Caller.prototype.getVideoProperty = function(videoname, attr) {
    var result = undefined;
    if (this.network_type == "WebRTC") {
        var obj = $("html5-" + videoname);
        if (obj) {
            if (attr == "controls") {
                result = obj.controls;
            }
            else {
                log("ignoring get property '" + attr + "' on '" + videoname + '"');
            }
        }
        else {
            log("cannot get video object of id 'html5-" + videoname + "' to get property '" + attr + "'");
        }
        log("get " + videoname + "." + attr + " = " + result);
    }
    else {
        var obj = getFlashMovie(videoname);
        if (obj) {
            result = obj.getProperty(attr);
        }
        else {
            log("cannot get video object of name '" + videoname + "' to get property '" + attr + "'");
        }
    }
    return result;
};

/// SIP set <video id="html5-local-video"> with src url
Caller.prototype.onUserMediaSuccess = function(stream) {
    log("webrtc - accessing user media successfully");
    if (stream !== this._webrtc_local_stream) {
        this._webrtc_local_stream = stream;
        if (this.call_state == "inviting" || this.call_state == "accepting") {
            // need to start peer-connection also
            this.createdWebRtcLocalStream();
        }
    }
    
    var video = $("html5-local-video");
    if (video) {
        var url = webkitURL.createObjectURL(stream);
        log('webrtc - local-video.src="' + url + '"');
        video.setAttribute('src', url);
    }
};

Caller.prototype.onUserMediaError = function(error) {
    log("webrtc - failed to get access to local media: " + error.code);
    if (this.call_state == "inviting" || this.call_state == "accepting") {
        if (this.call_state == "accepting") {
            ua.sendResponse(ua.createResponse(603, 'Declined Media Devices'));
        }
        
        this.setProperty("call_state", "failed");
        this.dispatchMessage('Failed: cannot access user media devices');
        this.hungup();
    }
};

/// For Flash network
Caller.prototype.createMediaSockets = function() {
    var this_ = this;
    var handler = function(event) {
        if (event.property == "bound")
            this_.createdMediaSockets();
    };
    
    this._gw = new network.RealTimeGateway();
    this._gw.addEventListener("propertyChange", function(event) {
        if (event.property == "publishurl")
            getFlashMovie('local-video').setProperty('src', event.newValue);
        else if (event.property == "playurl")
            getFlashMovie('remote-video').setProperty('src', event.newValue);
    });
    
    if (this.has_audio || this.has_tones) {
        var as = new network.RealTimeSocket();
        this._rtp.push(as);
        as.addEventListener("propertyChange", handler);
        as.bind();
    }
    if (this.has_video) {
        var vs = new network.RealTimeSocket();
        this._rtp.push(vs);
        vs.addEventListener("propertyChange", handler);
        vs.bind();
    }
    if (this.has_text) {
        var ts = new network.RealTimeSocket();
        this._rtp.push(ts);
        ts.addEventListener("propertyChange", handler);
        ts.bind();
    }
};

/// For Flash network
Caller.prototype.createdMediaSockets = function() {
    var all = true;
    var streams = [];
    
    for (var i=0; i<this._rtp.length; ++i) {
        if (this._rtp[i].bound == false) {
           all = false;
           break;
        }
    }
    
    if (all) {
        var rtp = this._rtp.slice();
        if (this.has_audio || this.has_tones) {
            var fmt = [];
            if (this.has_audio) {
                fmt.push({pt: 0, name: "pcmu", rate: 8000});
                //fmt.push({pt: 8, name: "pcma", rate: 8000});
                //fmt.push({pt: 96, name: "speex", rate: 8000});
                //fmt.push({pt: 97, name: "speex", rate: 16000});
            }
            if (this.has_tones) {
                fmt.push({pt: 101, name: "telephone-event", rate: 8000});
            }
            var audio = new sip.SDP.media({media: "audio", port: rtp.shift().localPort, proto: "RTP/AVP", fmt: fmt});
            streams.push(audio);
        }
        if (this.has_video) {
            var fmt = [{pt: 100, name: "h264", rate: 90000}];
            var video = new sip.SDP.media({media: "video", port: rtp.shift().localPort, proto: "RTP/AVP", fmt: fmt});
            video['a'] = ['fmtp:100 profile-level-id=42801f;packetization-mode=1'];
            streams.push(video);
        }
        if (this.has_text) {
            var fmt = [{pt: 98, name: "t140", rate: 1000}, {pt: 99, name: "red", rate: 1000}];
            var text = new sip.SDP.media({media: "text", port: rtp.shift().localPort, proto: "RTP/AVP", fmt: fmt});
            text['a'] = ['fmtp:99 98/98/98/98'];
            streams.push(text);
        }
    
        if (this.call_state == "inviting") {
            this._local_sdp = sip.createOffer(streams);
            this._local_sdp['o'].address = this.listen_ip;
            this._local_sdp['c'] = new sip.SDP.connection({address: this.listen_ip});
            
            var m = this._call.createRequest('INVITE');
            var c = new sip.Header(this._stack.uri.toString(), 'Contact');
            c.value.uri.user = this.username;
            m.setItem('Contact', c);
            if (this.user_agent)
                m.setItem('User-Agent', new sip.Header(this.user_agent, 'User-Agent'));
                
            if (this.has_location && this.location) {
                var xml = this.locationToXML(this.location, this.target_value);
                var multipart = this.createMultipartBody('application/sdp', this._local_sdp.toString(), 'application/pidf+xml', xml);
                m.setItem('Content-Type', new sip.Header('multipart/mixed; boundary="' + multipart[0] + '"', 'Content-Type'));
                m.setBody(multipart[1]);
            }
            else {
                m.setItem('Content-Type', new sip.Header('application/sdp', 'Content-Type'));
                m.setBody(this._local_sdp.toString());
            }
        
            this._call.sendRequest(m);
        }
        else if (this.call_state == "accepting") {
            var offer = null
            var ua = this._call;
            
            if (ua.request.hasItem('Content-Type') && ua.request.first('Content-Type').value.toLowerCase() == 'application/sdp') {
                try {
                    offer = new sip.SDP(this._call.request.body);
                } catch (ex) {
                    log("Failed to create SDP: " + ex);
                }
            }
            this._remote_sdp = offer;
            this._local_sdp = sip.createAnswer(streams, offer);
            
            if (this._local_sdp == null) {
                this.dispatchMessage('Incompatible session description');
                ua.sendResponse(ua.createResponse(488, 'Incompatible Session Description'));
                this.setProperty('call_state', 'idle');
                this.hungup();
            }
            else {
                this._local_sdp['o'].address = this.listen_ip;
                this._local_sdp['c'] = new sip.SDP.connection({address: this.listen_ip});
                
                this.createMediaConnections();
                
                this.setProperty('call_state', 'active');
                var m = this._call.createResponse(200, 'OK');
                var c = new sip.Header(this._stack.uri.toString(), 'Contact');
                c.value.uri.user = this.username;
                m.setItem('Contact', c);
                if (this.server)
                    m.setItem('Server', new sip.Header(this.server, 'Server'));
                m.setItem('Content-Type', new sip.Header('application/sdp', 'Content-Type'));
                m.setBody(this._local_sdp.toString());
                this._call.sendResponse(m);
            }
        }
        else {
            log('invalid call state in createdMediaSockets: ' + this.call_state);
        }
    }
};

/// For WebRTC network
Caller.prototype.createWebRtcConnection = function() {
    try {
        if ((this.has_audio || this.has_video) && !this.has_html5_webrtc) {
            throw new String("missing WebRTC, cannot use audio or video")
        }
        if (this._webrtc_local_stream == null) {
            this.setVideoProperty("local-video", "live", true);
        }
        else {
            this.createdWebRtcLocalStream();
        }
    }
    catch (e) {
        if (this.call_state == "accepting") {
            ua.sendResponse(ua.createResponse(500, 'Error in getting user media'));
        }
        
        this.setProperty("call_state", "failed");
        this.dispatchMessage('Failed: "' + e + '"');
        this.hungup();
    }
};

Caller.prototype.getIceServers = function() {
    var parts = this.webrtc_servers.split(","); // comma separated list
    var result = [];
    for (var i=0; i<parts.length; ++i) {
        var part = parts[i];
        if (part) {
            var index = part.indexOf("|");
            if (index < 0)
                result.push({url: part});
            else
                result.push({url: part.substr(0, index), credential: part.substr(index+1)});
        }
    }
    return result;
};

/// To Extract SDP from the SIP message
Caller.prototype.getSDP = function(message) {
    var type = message.hasItem("Content-Type") ? message.first("Content-Type").value : null;
    return (type == "application/sdp" || message.body) ? message.body : null;
};

/// To Create HTML5 WebRTC PeerConnection
Caller.prototype.createdWebRtcLocalStream = function() {
    var phone = this;
    this._webrtc_peer_connection = new webkitRTCPeerConnection({iceServers: this.getIceServers()}, null);
    this._webrtc_peer_connection.onconnecting = function(message) { phone.onWebRtcConnecting(message); };
    this._webrtc_peer_connection.onopen = function(message) { phone.onWebRtcOpen(message) };
    this._webrtc_peer_connection.onaddstream = function(event) { phone.onWebRtcAddStream(event.stream); };
    this._webrtc_peer_connection.onremovestream = function(event) { phone.onWebRtcRemoveStream(); };
    //we use timeout instead of onicecandidate event handler
    //this._webrtc_peer_connection.onicecandidate = function(event) { console.log(event.candidate);};
    if (this.call_state == "accepting" && this._call != null && this._call.request != null) {
        var result = this.getSDP(this._call.request);
        if (result) {
            this._webrtc_peer_connection.setRemoteDescription(new RTCSessionDescription({type: "offer", sdp: result}));
        }
    }
    if (this._webrtc_local_stream != null) {
        this._webrtc_peer_connection.addStream(this._webrtc_local_stream);
    }
    
    if (this.call_state == "inviting") {
        this._webrtc_peer_connection.createOffer(function(offer) {
            phone._webrtc_peer_connection.setLocalDescription(offer);
            setTimeout(function() {
                phone.onWebRtcSendMessage();
            }, phone._sdp_timeout);
        });
    }
    else if (this.call_state == "accepting") {
        this._webrtc_peer_connection.createAnswer(function(offer) {
            phone._webrtc_peer_connection.setLocalDescription(offer);
            setTimeout(function() {
                phone.onWebRtcSendMessage();
            }, phone._sdp_timeout);
        });
    }
};

Caller.prototype.onWebRtcSendMessage = function() {
    if (this.call_state == "inviting") {
        this._local_sdp = this._webrtc_peer_connection.localDescription.sdp;
        var m = this._call.createRequest('INVITE');
        //var c = new sip.Header(this._stack.uri.toString(), 'Contact');
        //c.value.uri.user = this.username;
        var c = new sip.Header((new sip.Address(this.local_aor)).uri.toString(), 'Contact');
        m.setItem('Contact', c);
        if (this.user_agent)
            m.setItem('User-Agent', new sip.Header(this.user_agent, 'User-Agent'));
        m.setItem('Content-Type', new sip.Header("application/sdp", 'Content-Type'));
        m.setBody(this._local_sdp);
        this._call.sendRequest(m);
    }
    else if (this.call_state == "accepting") {
        this._local_sdp = this._webrtc_peer_connection.localDescription.sdp;
        var ua = this._call;
        
        this.setProperty('call_state', 'active');
        this.dispatchMessage('Connected');
        var m = this._call.createResponse(200, 'OK');
        //var c = new sip.Header(this._stack.uri.toString(), 'Contact');
        //c.value.uri.user = this.username;
        var c = new sip.Header((new sip.Address(this.local_aor)).uri.toString(), 'Contact');
        m.setItem('Contact', c);
        if (this.server)
            m.setItem('Server', new sip.Header(this.server, 'Server'));
        m.setItem('Content-Type', new sip.Header("application/sdp", 'Content-Type'));
        m.setBody(this._local_sdp);
        
        this._call.sendResponse(m);
    }
    else if (this.call_state == "active") {
        this._local_sdp = this._webrtc_peer_connection.localDescription.sdp;
        
        // need to send re-INVITE with new SDP
        var ua = this._call;
        
        var m = this._call.createRequest('INVITE');
        //var c = new sip.Header(this._stack.uri.toString(), 'Contact');
        //c.value.uri.user = this.username;
        var c = new sip.Header((new sip.Address(this.local_aor)).uri.toString(), 'Contact');
        m.setItem('Contact', c);
        if (this.user_agent)
            m.setItem('User-Agent', new sip.Header(this.user_agent, 'User-Agent'));
        m.setItem('Content-Type', new sip.Header("application/sdp", 'Content-Type'));
        m.setBody(this._local_sdp);
        this._call.sendRequest(m);
    }
    else {
        log('invalid call state in onWebRtcSendMessage: ' + this.call_state);
    }
};

Caller.prototype.onWebRtcConnecting = function(event) {
    log("webrtc - onconnecting(" + event + ")");
};

Caller.prototype.onWebRtcOpen = function(message) {
    log("webrtc - onopen(" + message + ")");
};

/// For <video id="remote-video"> src url
Caller.prototype.onWebRtcAddStream = function(stream) {
    log("webrtc - onaddstream(...)");
    var video = $("html5-remote-video");
    if (video) {
        var url = webkitURL.createObjectURL(stream);
        log('webrtc - remote-video.src="' + url + '"');
        video.setAttribute('src', url);
    }
};

Caller.prototype.onWebRtcRemoveStream = function() {
    log("webrtc - onremovestream()");
    var video = $("html5-remote-video");
    if (video) {
        log("webrtc - remote-video.src=null");
        video.setAttribute('src', '');
    }
};

Caller.prototype.hungup = function() {
    if (this._call != null) {
        this._call = null;
    }
    if (this.network_type == "WebRTC") {
        var local = $("html5-local-video");
        if (local) {
            local.setAttribute('src', '');
        }
        var remote = $("html5-remote-video");
        if (remote) {
            remote.setAttribute('src', '');
        }
        if (this._webrtc_peer_connection) {
            try {
                this._webrtc_peer_connection.close();
            }
            catch (error) {
                log("webrtc - error closing peer connection: " + error);
            }
            this._webrtc_peer_connection = null;
        }
    }
    else {
        getFlashMovie("local-video").setProperty("src", null);
        getFlashMovie("remote-video").setProperty("src", null);
        if (this._gw != null) {
            this._gw.close();
            this._gw = null;
        }
        if (this._rtp && this._rtp.length > 0) {
            for (var i=0; i<this._rtp.length; ++i) {
                this._rtp[i].close();
            }
            this._rtp.splice(0, this._rtp.length);
        }
    }
    this._local_sdp = null;
    this._remote_sdp = null;
}

Caller.prototype.getRouteHeader = function(username) {
    return new sip.Header("<sip:" + (username ? username + "@" : "") + this.outbound_proxy_address + ";lr>", 'Route');
};

Caller.prototype.receivedInviteResponse = function(ua, response) {
    if (response.isfinal()) {
        if (!response.is2xx()) {
            if (this.call_state == "inviting" || this.call_state == "ringback") {
                this.setProperty("call_state", "failed");
                this.dispatchMessage('Failed: "' + response.response + ' ' + response.responsetext + '"');
                this.hungup();
            }
        }
        else {
            if (this.network_type == "WebRTC") {
                if (this._webrtc_peer_connection) {
                    ua.autoack = true;
                    if (this.call_state == "inviting" || this.call_state == "ringback") {
                        this.setProperty("call_state", "active");
                        this.dispatchMessage('Connected');
                        
                        var result = this.getSDP(response);
                        if (result) {
                            this._webrtc_peer_connection.setRemoteDescription(new RTCSessionDescription({type: "answer", sdp: result}));
                        }
                    }
                }
                else {
                    // failed to get peer-connection
                    this.setProperty("call_state", "failed");
                    this.sendBye();
                    this.dispatchMessage("No peer connection found");
                }
            }
            else {
                if (response.hasItem('Content-Type') && response.first('Content-Type').value.toLowerCase() == 'application/sdp') {
                    var answer = new sip.SDP(response.body);
                    this._remote_sdp = answer;
                    this.createMediaConnections();
                    
                    if (this.call_state != 'active') {
                        this.setProperty("call_state", "active");
                        this.dispatchMessage('Connected');
                    }
                }
                else {
                    // failed to get SDP
                    this.dispatchMessage("Missing session description");
                    this.sendBye();
                    this.setProperty("call_state", "failed");
                }
            }
        }
    }
    else if (response.is1xx()) {
        if (response.response != 100) {
            this.dispatchMessage('Progress "' + response.response + ' ' + response.responsetext + '"');
            if (response.response >= 180) {
                this.setProperty("call_state", "ringback");
                this.playSound("ringback");
            }
        }
    }
};

Caller.prototype.receivedAck = function(ua, request) {
    if (this.network_type == "WebRTC") {
        if (this._webrtc_peer_connection) {
            // do not handle SDP in ACK
        }
    }
};

/// For Flash network
Caller.prototype.createMediaConnections = function() {
    log('create media connections');
    var rtp = this._rtp.slice();
    var audio_sock = null, video_sock = null;
    
    // TODO: need to dynamically create receive and send types
    if (this.has_audio || this.has_tones) {
        log('setting gateway audio');
        audio_sock = rtp.shift();
        this._gw.setAudio(audio_sock, {0: 'pcmu/8000'}, {'pcmu/8000': 0});
    }
    if (this.has_video) {
        log('setting gateway video');
        video_sock = rtp.shift();
        this._gw.setVideo(video_sock, {100: 'h264/90000'}, {'h264/90000': 100});
    }
    
    var media = this._remote_sdp['m'];
    var conn = this._remote_sdp['c'];
    for (var i=0; i<media.length; ++i) {
        var m = media[i];
        var address = m['c'] ? m['c'].address : conn.address;
        if (m.media == 'audio' && audio_sock) {
            if (m.port) {
                audio_sock.connect(address, m.port);
            }
        }
        else if (m.media == 'video' && video_sock) {
            if (m.port) {
                video_sock.connect(address, m.port);
            }
        }
    }
};

/// For HTML5 <audio id="html5-audio"> setting
Caller.prototype.playSound = function(value) {
    if (this.enable_sound_alert) {
        var audio = $("html5-audio");
        if (audio) {
            // TODO: use .ogg for chrome/firefox/opera and .mp3 for IE/safari
            audio.setAttribute('src',(value ? value + ".ogg" : value));
        }
    }
};

Caller.prototype.receivedInvite = function(ua, request) {
    if (this.call_state == "idle") {
        this._call = ua;
        this.setProperty("call_state", "incoming");
        var from = request.first('From').value;
        this.setProperty("target_value", from.uri.user + '@' + from.uri.host);
        this.dispatchMessage('Incoming call from ' + from.toString());
        ua.sendResponse(ua.createResponse(180, 'Ringing'));
        this.playSound("ringing");
    }
    else if (this.call_state == "active" && this._call == ua) {
        // received re-invite
        log("received re-INVITE");
        var m = this._call.createResponse(200, 'OK');
        //var c = new sip.Header(this._stack.uri.toString(), 'Contact');
        //c.value.uri.user = this.username;
        var c = new sip.Header((new sip.Address(this.local_aor)).uri.toString(), 'Contact');
        m.setItem('Contact', c);
        if (this.server)
            m.setItem('Server', new sip.Header(this.server, 'Server'));
        this._call.sendResponse(m);
        
        if (this._webrtc_peer_connection) {
            var result = this.getSDP(request);
            if (result) {
                this._webrtc_peer_connection.setRemoteDescription(new RTCSessionDescription({type: "offer", sdp: result}));
            }
        }
    }
    else {
        log("received INVITE in state " + this.call_state);
        var from = request.first('From').value;
        this.dispatchMessage('Missed call from ' + from.toString());
        ua.sendResponse(ua.createResponse(486, 'Busy Here'));
        this.playSound("alert");
    }
};

Caller.prototype.sendInviteResponse = function(code, text) {
    if (this._call) {
        if (code >= 200 && code < 300) {
            this.setProperty("call_state", "accepting");
            if (this.network_type == "WebRTC") {
                this.createWebRtcConnection();
            }
            else {
                this.createMediaSockets();
            }
        }
        else if (code >= 300) {
            this._call.sendResponse(this._call.createResponse(code, text));
            this.setProperty("call_state", "idle");
            this.hungup();
        }
    }
};

Caller.prototype.sendBye = function() {
    if (this._call) {
        var m = this._call.createRequest('BYE');
        this._call.sendRequest(m);
        this.hungup();
    }
};

Caller.prototype.receivedBye = function(ua, request) {
    if (this._call && this.call_state != "idle") {
        ua.sendResponse(ua.createResponse(200, 'OK'));
        this.dispatchMessage('Call closed by remote party');
        this.setProperty("call_state", "closed");
		//this.hungup();
    }
};

Caller.prototype.receivedByeResponse = function(ua, response) {
    log("ignoring BYE response: " + response.response + " " + response.responsetext);
}

Caller.prototype.sendCancel = function() {
    if (this._call) {
        this._call.sendCancel();
        this.setProperty("call_state", "idle");
        this.hungup();
    }
};



Caller.prototype.dispatchMessage = function(text, sender) {
    this.dispatchEvent({"type": "message", "text": text, "sender": sender});
};

Caller.prototype.sendMessage = function(text, ua) {
    if (ua === undefined) {
        ua = new sip.UserAgent(this._stack);
        ua.remoteParty = new sip.Address(this.target_aor);
        ua.localParty = new sip.Address(this.local_aor);
        if (this.outbound == "proxy") {
            ua.routeSet = [this.getRouteHeader(ua.remoteParty.uri.user)];
        }
    }
    
    this.dispatchMessage(text, this.displayname + " (you)");
    var m = ua.createRequest('MESSAGE');
    m.setItem('Content-Type', new sip.Header("text/plain", "Content-Type"));
    m.setBody(text);
    ua.sendRequest(m);
};

Caller.prototype.receivedMessageResponse = function(ua, response) {
    if (response.isfinal()) {
        if (!response.is2xx()) {
            this.dispatchMessage('Failed: "' + response.response + ' ' + response.responsetext + '"');
        }
    }
};

Caller.prototype.receivedMessage = function(ua, request) {
    log("received message");
    ua.sendResponse(ua.createResponse(200, 'OK'));
    
    var text, sender;
    if (request.hasItem("Content-Type") && request.first("Content-Type").value == "text/plain") {
        text = request.body;
    }
    sender = request.first("From").value.getDisplayable();
    if (text && sender) {
        this.dispatchMessage(text, sender);
        this.playSound("alert");
    }
    
    if (this.call_state == "idle") {
        var from = request.first("From").value;
        this.setProperty("target_scheme", from.uri.scheme);
        this.setProperty("target_value", from.uri.user + "@" + from.uri.host);
    }
};

Caller.prototype.onSockData = function(event) {
    log("<= " + event.srcAddress + ":" + event.srcPort + "\n" + event.data);
    this._stack.received(event.data, [event.srcAddress, event.srcPort]);
};

Caller.prototype.onSockError = function(event) {
    // do not use event to mean Flash Network's event
    this._sock = null;
    this.setProperty("sock_state", "idle");
    this.setProperty("register_state", "not registered");
    this._reg = null;
    this.setProperty("register_button", "Register");
    this.setProperty("call_state", "idle");
    this.hungup();
};

Caller.prototype.onWebSocketOpen = function() {
    log("websocket connected");
    this.onSockPropertyChange({"property": "connected", "oldValue": false, "newValue": true});
};

Caller.prototype.onWebSocketClose = function() {
    this.onSockPropertyChange({"property": "connected", "oldValue": true, "newValue": false});
};

Caller.prototype.onWebSocketError = function(error) {
    this.onSockError({"code": "websocket-error", "reason": error});
};

Caller.prototype.onWebSocketMessage = function(msg) {
    this.onSockData({"data": msg.data, "srcPort": 0, "srcAddress": "127.0.0.1"});
};

Caller.prototype.createServer = function(request, uri, stack) {
    log("Caller.createServer() for method=" + request.method);
    return (request.method != "CANCEL" ? new sip.UserAgent(this._stack, request) : null);
};

Caller.prototype.receivedRequest = function(ua, request, stack) {
    var method = request.method;
    log("Caller.receivedRequest() " + request.method);
    if (method == "INVITE") {
        this.receivedInvite(ua, request);
    }
    else if (method == "BYE") {
        this.receivedBye(ua, request);
    }
    else if (method == "MESSAGE") {
        this.receivedMessage(ua, request);
    }
    else if (method == "ACK") {
        this.receivedAck(ua, request);
    }
    else {
        log("ignoring received request method=" + method);
        if (method != 'ACK') 
            ua.sendResponse(ua.createResponse(501, "Not Implemented"));
    }
};

Caller.prototype.receivedResponse = function(ua, response, stack) {
    var method = ua.request.method;
    if (method == 'REGISTER') {
        this.receivedRegisterResponse(ua, response);
    }
    else if (method == "INVITE") {
        this.receivedInviteResponse(ua, response);
    }
    else if (method == "BYE") {
        this.receivedByeResponse(ua, response);
    }
    else if (method == "MESSAGE") {
        this.receivedMessageResponse(ua, response);
    }
    else {
        log("ignoring response for method=" + method);
    }
};

Caller.prototype.cancelled = function(ua, request, stack) {
    log("cancelled");
    if (this._call && this.call_state == "incoming") {
        if (ua == this._call) {
            this.dispatchMessage("Incoming call cancelled");
            this.setProperty("call_state", "idle");
            this.hungup();
        }
        else {
            log("invalid ua for cancel");
        }
    }
};

Caller.prototype.dialogCreated = function(dialog, ua, stack) {
    log("dialog created");
    if (ua == this._call) {
        this._call = dialog;
    }
};

Caller.prototype.authenticate = function(ua, header, stack) {
    log("phone.authenticate() called");
    header.username = $('authname').value;
    header.password = $('password').value;
    return true;
};

Caller.prototype.createTimer = function(obj, stack) {
    return new sip.TimerImpl(obj);
};

Caller.prototype.resolve = function(host, type, callback, stack) {
    var resolver = new network.DNSResolver();
    resolver.addEventListener("error", function(event) {
        log("cannot resolve DNS host " + host + " type " + type);
        callback(host, []); 
    });
    resolver.addEventListener("lookup", function(event) {
        log("resolved DNS host " + event.host + " length=" + event.resourceRecords.length);
        callback(host, event.resourceRecords);
    });
    resolver.resolve(host, type);
};

Caller.prototype.send = function(data, addr, stack) {
    log("=> " + addr[0] + ":" + addr[1] + "\n" + data);
    this._sock.send(data, addr[0], addr[1]);
};

Caller.prototype.sendDigit = function(digit) {
    if (this.call_state == "idle") {
        if (this.target_scheme != "tel") {
            this.setProperty("target_scheme", "tel");
            this.setProperty("target_value", "");
        }
        this.setProperty("target_value", (this.target_value || "") + digit);
    }
    else {
        // send the digit as RFC 4733.
        log("sending digit in a call is not implemented");
    }
};

Caller.prototype.sendChar = function(event) {
    //log("send char " + event.keyCode + " " + String.fromCharCode(event.keyCode));
};

Caller.prototype.sendText = function(text) {
    if (this.call_state == "active") {
        log("send session text " + text);
        if (this.has_text) {
            // send using real-time text
            log("sending using real-time text is not implemented");
        }
        else {
            // send using SIP MESSAGE
            this.sendMessage(text, this._call);
        }
    }
    else {
        log("send paging text " + text);
        if (!this._sock) {
            this._next_message = text;
            this.createSocket();
        }
        else {
            this.sendMessage(text);
        }
    }
};

Caller.prototype.setLocation = function() {
    var location = prompt("Enter your civic location or geodatic co-ordinates\nFor example,  \"40.8097 -73.9608\" or\n\"A1=US,A2=CA,A3=San Francisco,HNO=542,RD=5th,STS=Ave\"");
    this.setProperty("location", location);
};

Caller.prototype.locationToXML = function(location, userhost) {
    var geo = /^([\d\.\-]+)\s+([\d\.\-]+)(\s+([\d\.]+))?$/;
    var match = location.match(geo);
    var result = null;
    if (match) {
        var latitude = match[1], longitude = match[2], radius = match[4];
        if (radius) {
            result = '<location id="location0" profile="geodetic-2d"><Circle id="point0" xmlns="http://www.opengis.net/pidflo/1.0" srsName="urn:ogc:def:crs:EPSG::4326"><pos xmlns="http://www.opengis.net/gml">' + latitude + ' ' + longitude + '</pos><radius  uom="urn:ogc:def:uom:EPSG::9001">' + radius + '</radius></Circle></location>';
        }
        else {
            result = '<location id="location0" profile="geodetic-2d"><Point id="point0" xmlns="http://www.opengis.net/gml" srsName="urn:ogc:def:crs:EPSG::4326"><pos>' + latitude + ' ' + longitude + '</pos><radius uom="urn:ogc:def:uom:EPSG::9001">' + radius + '</radius></Point></location>';
        }
    }
    else {
        var parts = location.split(',');
        var attrs = [];
        for (var i=0; i<parts.length; ++i) {
            var part = parts[i];
            var index = part.indexOf('=');
            if (index > 0) {
                var aname = part.substr(0, index), avalue = part.substr(index+1);
                attrs.push('<' + aname + '>' + avalue + '</' + aname + '>');
            }
        }
        result = '<location id="location0" profile="civic"><civicAddress xmlns="urn:ietf:params:xml:ns:pidf:geopriv10:civicAddr">' + attrs.join('') + '</civicAddress></location>';
    }
    
    return '<presence entity="pres:' + userhost + '" xmlns="urn:ietf:params:xml:ns:pidf"><tuple><status><geopriv xmlns="urn:ietf:params:xml:ns:pidf:geopriv10"><location-info>' + result + '</location-info></geopriv></status></tuple></presence>';
};

// must have even number of arguments: type, content, type, content, ...
Caller.prototype.createMultipartBody = function() {
    var boundary = "boundary-" + Math.floor(Math.random() * 1000000000);
    var parts = [];
    for (var i=0; i<arguments.length; i+=2) {
        var type = arguments[i];
        var content = arguments[i+1];
        parts.push('Content-Type: ' + type + '\r\n\r\n' + content);
    }
    var multipart = '--' + boundary + '\r\n' + parts.join('\r\n--' + boundary + '\r\n') + '\r\n--' + boundary + '--';
    return [boundary, multipart];
};

Caller.prototype.print = function(content) {
    content = cleanHTML(content).replace(/\n/g, "<br/>\n");
    var docprint = window.open("", "", "toolbar=yes,location=no,directories=no,menubar=no,scrollbars=yes,width=650,height=600,left=100,top=25");
    docprint.document.open();
    docprint.document.write('<html><head><title>Program Log</title>');
    docprint.document.write('</head><body onLoad="self.print()">');
    docprint.document.write(content);
    docprint.document.write('</body></html>');
    docprint.document.close(); 
    docprint.focus(); 
};

Caller.prototype.toggleControls = function(name) {
    try {
        this.setVideoProperty(name, 'controls', !this.getVideoProperty(name, 'controls'));
        //var obj = getFlashMovie(name);
        //var controls = obj.getProperty('controls');
        //obj.setProperty('controls', !controls);
        return false;
    }
    catch (ex) {
        log(ex.stack);
    }
};

Caller.prototype.help = function(name) {
    var text = null;
    if (!text)
        text = 'Help text for this feature is not written';
	var help = $("help");
	if (help)
		help.innerHTML = text;
    return true;
};

