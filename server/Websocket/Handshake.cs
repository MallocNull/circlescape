﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CircleScape.Websocket {
    class Handshake {
        private const string HttpVersion = "1.1";

        public enum kStatusCode {
            Switching_Protocols     = 101,

            Bad_Request             = 400,
            Unauthorized            = 401,
            Forbidden               = 403,
            Not_Found               = 404,
            Method_Not_Allowed      = 405,
            Not_Acceptable          = 406,
            Request_Timeout         = 408,
            Conflict                = 409,
            Gone                    = 410,

            Internal_Server_Error   = 500,
            Not_Implemented         = 501,
            Bad_Gateway             = 502,
            Service_Unavailable     = 503,
            Gateway_Timeout         = 504
        }

        public kStatusCode StatusCode { get; private set; } = kStatusCode.Switching_Protocols;
        protected string StatusCodeText {
            get {
                return Enum.GetName(typeof(kStatusCode), StatusCode).Replace('_', ' ');
            }
        }
        private Dictionary<string, string> Headers =
            new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        public string Content { get; set; } = null;

        public Handshake(string rawData) {
            var headerLength = rawData.IndexOf("\r\n\r\n");
            if(headerLength == -1)
                throw new FormatException("Header delimeter not found in raw data");

            var header = rawData.Substring(0, headerLength);
            if(!header.StartsWith("HTTP/"))
                throw new FormatException("Protocol defined in status line not understood");

            var lines = header.Split('\n');
            foreach(var line in lines) {
                string[] parts;
                if(line.StartsWith("HTTP/")) {
                    parts = line.Trim().Split(' ');
                    if(parts.Length < 2)
                        throw new FormatException("Status line in header malformed");

                    int code;
                    if(!int.TryParse(parts[1], out code))
                        throw new FormatException("Status code sent is not a number");

                    if(!Enum.IsDefined(typeof(kStatusCode), code))
                        throw new NotSupportedException("Status code not supported");

                    StatusCode = (kStatusCode)code;
                } else {
                    parts = line.Trim().Split(new char[] {':'}, 2);
                    if(parts.Length == 2)
                        Headers.Add(parts[0].Trim(), parts[1].Trim());
                }
            }

            if(Headers.ContainsKey("Content-Length")) {
                rawData.Substring(headerLength + 4, int.Parse(Headers["Content-Length"]));
            } else {
                if(rawData.Length > headerLength + 4)
                    Content = rawData.Substring(headerLength + 4);
            }
        }

        public Handshake(kStatusCode statusCode = kStatusCode.Switching_Protocols, string content = null) {
            StatusCode = statusCode;
            Content = content;
        }

        public static Handshake AcceptRequest(Handshake request) {
            const string nonce = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
            var key = request.GetHeader("Sec-WebSocket-Key");
            var connectionHash = (key + nonce).SHA1().Base64Encode(false);

            var shake = new Handshake(kStatusCode.Switching_Protocols);
            shake.SetHeader("Upgrade", "websocket")
                 .SetHeader("Connection", "Upgrade")
                 .SetHeader("Sec-WebSocket-Accept", connectionHash);
            return shake;
        }

        public static Handshake DenyRequest(kStatusCode statusCode = kStatusCode.Bad_Request, string message = "Handshake failed.") {
            return new Handshake(statusCode, message);
        }

        public string GetRaw() {
            if(Content != null) {
                SetHeader("Content-Length", Encoding.ASCII.GetByteCount(Content).ToString());
                if(GetHeader("Content-Type") == null)
                    SetHeader("Content-Type", "text/plain");
            }

            var raw = "HTTP/"+ HttpVersion +" "+ (int)StatusCode + " "+ StatusCodeText +"\r\n";
            foreach(var header in Headers)
                raw += header.Key.Trim() + ": " + header.Value.Trim() + "\r\n";
            return raw += "\r\n";
        }

        public string GetHeader(string name) {
            if(Headers.ContainsKey(name))
                return Headers[name];
            else return null;
        }

        public Handshake SetHeader(string name, string value) {
            if(Headers.ContainsKey(name))
                Headers[name] = value;
            else
                Headers.Add(name, value);

            return this;
        }

        public Handshake AppendContent(string content) {
            Content += content;
            return this;
        }

        public void ClearContent() {
            Content = null;
            return this;
        }
    }
}
