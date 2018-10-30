let sections = {};
let ws;

const MAHOU = [0xB0, 0x0B];

const kClientToMaster = {
    "LoginRequest": 0,
    "RegisterRequest": 1,
    "ServerListRequest": 2,
};

const kMasterToClient = {
    "LoginResponse": 0,
    "RegisterResponse": 1,
    "ServerList": 2
};

function show_section(name) {
    for(let i in sections) {
        if(!sections.hasOwnProperty(i))
            continue;

        if(i === name)
            sections[i].classList.remove("hidden");
        else
            sections[i].classList.add("hidden");
    }
}

function pack(id, regions) {
    if(!Array.isArray(regions))
        throw Error("PACK MUST BE GIVEN ARRAY OF STRINGS AND UINT8ARRAYS");

    let pck = null;
    let size = 8;
    for(let i in regions) {
        if(typeof regions[i] === "string")

    }

    return pck;
}

function unpack(data) {
    if(!data.subarray(0, 2).every((v, i) => v === MAHOU[i]))
        return null;

    let id = data[6];
    let pck_length = data.unpackUint32(2);
    let region_cnt = data[7];
    let region_lengths = [];
    let regions = [];
    let ptr = 8;

    try {
        for(let i = 0; i < region_cnt; ++i) {
            if (data[ptr] < 254) {
                region_lengths.push(data[ptr]);
                ++ptr;
            } else if (data[ptr] === 254) {
                region_lengths.push(data.unpackUint16(ptr + 1));
                ptr += 3;
            } else {
                region_lengths.push(data.unpackUint32(ptr + 1));
                ptr += 5;
            }
        }
    } catch(ex) {
        return null;
    }

    try {
        for (let i = 0; i < region_cnt; ++i) {
            regions.push(data.subarray(ptr, region_lengths[i]));
            ptr += region_lengths[i];
        }
    } catch(ex) {
        return null;
    }

    return {
        "id": id,
        "regions": regions
    };
}

function parse(data) {
    let packet = unpack(data);
    if(packet === null)
        return;

    switch(packet.id) {
        case kMasterToClient.LoginResponse:

            break;
        case kMasterToClient.RegisterResponse:

            break;
        case kMasterToClient.ServerList:

            break;
    }
}

function conn_open() {
    ws = new WebSocket("wss://localhost:8008");
    ws.binaryType = "arraybuffer";

    ws.onopen = function (e) {

    };

    ws.onmessage = function(e) {
        parse(e.data);
    };

    ws.onclose = function (e) {
        conn_retry();
    }
}

function conn_retry() {
    if(ws.readyState < 3) {
        ws.close();
        return;
    }

    let loading = document.getElementById("loading");
    loading.innerHTML = "Connection lost.<br/>Retrying...";
    loading.classList.remove("hidden");

    conn_open();
}

window.onload = function() {
    let raw_sections = document.body.getElementsByClassName("section");
    for(let i in raw_sections)
        sections[raw_sections[i].id] = raw_sections[i];

    conn_open();
};

/****************************/
/***  NECESSARY BULLSHIT  ***/
/****************************/

/*** STRING EXTENSIONS ***/

String.prototype.replaceAll = function(needle, replace, ignoreCase = false) {
    if((typeof needle) === "string")
        return this.replace(new RegExp(needle.replace(/([\/\,\!\\\^\$\{\}\[\]\(\)\.\*\+\?\|\<\>\-\&])/g,"\\$&"),(ignoreCase?"gi":"g")),(typeof(replace)=="string")?replace.replace(/\$/g,"$$$$"):replace);
    else {
        let retval = this;
        for(let i in needle) {
            if((typeof replace) === "string")
                retval = retval.replaceAll(needle[i], replace, ignoreCase);
            else
                retval = retval.replaceAll(needle[i], replace[i], ignoreCase);
        }
        return retval;
    }
};

String.prototype.contains = function(needle, ignoreCase = false) {
    return ignoreCase
        ? this.toLowerCase().indexOf(needle.toLowerCase()) !== -1
        : this.indexOf(needle) !== -1;
};

String.prototype.stripCharacters = function(chars) {
    let copy = this;
    if(chars !== "")
        copy = copy.replaceAll(chars.split(""), "");

    return copy;
};

String.prototype.hasUnicodeCharacters = function() {
    for(let i = 0; i < this.length; i++) {
        if(this.charCodeAt(i) > 127) return true;
    }
    return false;
};

String.prototype.byteLength = function() {
    return utf8.encode(this).length;
};

String.prototype.toByteArray = function() {
    let str = utf8.encode(this);
    let ret = new Uint8Array(str.length);
    for(let i = 0; i < str.length; i++)
        ret[i] = str.charCodeAt(i);

    return ret;
};

/*** NUMBER EXTENSIONS ***/

Number.prototype.zeroPad = function(mag = 1) {
    let ret = ""+ this;
    for(; this < Math.pow(10, mag) && mag !== 0; --mag)
        ret = "0" + ret;
    return ret;
};

Number.prototype.packInt16 = function() {
    let buffer = new ArrayBuffer(2);
    new DataView(buffer).setInt16(0, this, false);
    return new Uint8Array(buffer);
};

Number.prototype.packUint16 = function() {
    let buffer = new ArrayBuffer(2);
    new DataView(buffer).setUint16(0, this, false);
    return new Uint8Array(buffer);
};

Number.prototype.packInt32 = function() {
    let buffer = new ArrayBuffer(4);
    new DataView(buffer).setInt32(0, this, false);
    return new Uint8Array(buffer);
};

Number.prototype.packUint32 = function() {
    let buffer = new ArrayBuffer(4);
    new DataView(buffer).setUint32(0, this, false);
    return new Uint8Array(buffer);
};

Number.prototype.packFloat = function() {
    let buffer = new ArrayBuffer(4);
    new DataView(buffer).setFloat32(0, this, false);
    return new Uint8Array(buffer);
};

Number.prototype.packDouble = function() {
    let buffer = new ArrayBuffer(8);
    new DataView(buffer).setFloat64(0, this, false);
    return new Uint8Array(buffer);
};

/*** UINT8ARRAY EXTENSIONS ***/

Uint8Array.prototype.unpackInt16 = function(offset = 0) {
    let buffer = this.buffer;
    return new DataView(buffer).getInt16(offset, false);
};

Uint8Array.prototype.unpackUint16 = function(offset = 0) {
    let buffer = this.buffer;
    return new DataView(buffer).getUint16(offset, false);
};

Uint8Array.prototype.unpackInt32 = function(offset = 0) {
    let buffer = this.buffer;
    return new DataView(buffer).getInt32(offset, false);
};

Uint8Array.prototype.unpackUint32 = function(offset = 0) {
    let buffer = this.buffer;
    return new DataView(buffer).getUint32(offset, false);
};

Uint8Array.prototype.unpackFloat = function(offset = 0) {
    let buffer = this.buffer;
    return new DataView(buffer).getFloat32(offset, false);
};

Uint8Array.prototype.unpackDouble = function(offset = 0) {
    let buffer = this.buffer;
    return new DataView(buffer).getFloat64(offset, false);
};

Uint8Array.prototype.toString = function() {
    let chunkSize = 4096;
    let raw = "";
    for(let i = 0;; i++) {
        if(this.length < chunkSize*i) break;
        raw += String.fromCharCode.apply(null,
            this.subarray(chunkSize*i, chunkSize*i + chunkSize)
        );
    }
    return utf8.decode(raw);
};

Uint8Array.prototype.toHexString = function() {
    let ret = "";
    for(let i = 0; i < this.byteLength; ++i) {
        let byte = this[i].toString(16);
        if(byte.length < 2)
            byte = "0"+ byte;

        ret += byte +" ";
    }

    return ret.trim();
};

/*** UTF8 LIBRARY ***/

let utf8 = {
    'encode': null,
    'decode': null
};

(function() {
    var stringFromCharCode = String.fromCharCode;

    function ucs2decode(string) {
        var output = [];
        var counter = 0;
        var length = string.length;
        var value;
        var extra;
        while (counter < length) {
            value = string.charCodeAt(counter++);
            if (value >= 0xD800 && value <= 0xDBFF && counter < length) {
                extra = string.charCodeAt(counter++);
                if ((extra & 0xFC00) == 0xDC00) {
                    output.push(((value & 0x3FF) << 10) + (extra & 0x3FF) + 0x10000);
                } else {
                    output.push(value);
                    counter--;
                }
            } else {
                output.push(value);
            }
        }
        return output;
    }

    function ucs2encode(array) {
        var length = array.length;
        var index = -1;
        var value;
        var output = '';
        while (++index < length) {
            value = array[index];
            if (value > 0xFFFF) {
                value -= 0x10000;
                output += stringFromCharCode(value >>> 10 & 0x3FF | 0xD800);
                value = 0xDC00 | value & 0x3FF;
            }
            output += stringFromCharCode(value);
        }
        return output;
    }

    function checkScalarValue(codePoint) {
        if (codePoint >= 0xD800 && codePoint <= 0xDFFF) {
            throw Error(
                'Lone surrogate U+' + codePoint.toString(16).toUpperCase() +
                ' is not a scalar value'
            );
        }
    }

    function createByte(codePoint, shift) {
        return stringFromCharCode(((codePoint >> shift) & 0x3F) | 0x80);
    }

    function encodeCodePoint(codePoint) {
        if ((codePoint & 0xFFFFFF80) == 0) {
            return stringFromCharCode(codePoint);
        }
        var symbol = '';
        if ((codePoint & 0xFFFFF800) == 0) {
            symbol = stringFromCharCode(((codePoint >> 6) & 0x1F) | 0xC0);
        }
        else if ((codePoint & 0xFFFF0000) == 0) {
            checkScalarValue(codePoint);
            symbol = stringFromCharCode(((codePoint >> 12) & 0x0F) | 0xE0);
            symbol += createByte(codePoint, 6);
        }
        else if ((codePoint & 0xFFE00000) == 0) {
            symbol = stringFromCharCode(((codePoint >> 18) & 0x07) | 0xF0);
            symbol += createByte(codePoint, 12);
            symbol += createByte(codePoint, 6);
        }
        symbol += stringFromCharCode((codePoint & 0x3F) | 0x80);
        return symbol;
    }

    function utf8encode(string) {
        var codePoints = ucs2decode(string);
        var length = codePoints.length;
        var index = -1;
        var codePoint;
        var byteString = '';
        while (++index < length) {
            codePoint = codePoints[index];
            byteString += encodeCodePoint(codePoint);
        }
        return byteString;
    }

    function readContinuationByte() {
        if (byteIndex >= byteCount) {
            throw Error('Invalid byte index');
        }

        var continuationByte = byteArray[byteIndex] & 0xFF;
        byteIndex++;

        if ((continuationByte & 0xC0) == 0x80) {
            return continuationByte & 0x3F;
        }

        throw Error('Invalid continuation byte');
    }

    function decodeSymbol() {
        var byte1;
        var byte2;
        var byte3;
        var byte4;
        var codePoint;

        if (byteIndex > byteCount) {
            throw Error('Invalid byte index');
        }

        if (byteIndex == byteCount) {
            return false;
        }

        byte1 = byteArray[byteIndex] & 0xFF;
        byteIndex++;

        if ((byte1 & 0x80) == 0) {
            return byte1;
        }

        if ((byte1 & 0xE0) == 0xC0) {
            var byte2 = readContinuationByte();
            codePoint = ((byte1 & 0x1F) << 6) | byte2;
            if (codePoint >= 0x80) {
                return codePoint;
            } else {
                throw Error('Invalid continuation byte');
            }
        }

        if ((byte1 & 0xF0) == 0xE0) {
            byte2 = readContinuationByte();
            byte3 = readContinuationByte();
            codePoint = ((byte1 & 0x0F) << 12) | (byte2 << 6) | byte3;
            if (codePoint >= 0x0800) {
                checkScalarValue(codePoint);
                return codePoint;
            } else {
                throw Error('Invalid continuation byte');
            }
        }

        if ((byte1 & 0xF8) == 0xF0) {
            byte2 = readContinuationByte();
            byte3 = readContinuationByte();
            byte4 = readContinuationByte();
            codePoint = ((byte1 & 0x0F) << 0x12) | (byte2 << 0x0C) |
                (byte3 << 0x06) | byte4;
            if (codePoint >= 0x010000 && codePoint <= 0x10FFFF) {
                return codePoint;
            }
        }

        throw Error('Invalid UTF-8 detected');
    }

    var byteArray;
    var byteCount;
    var byteIndex;
    function utf8decode(byteString) {
        byteArray = ucs2decode(byteString);
        byteCount = byteArray.length;
        byteIndex = 0;
        var codePoints = [];
        var tmp;
        while ((tmp = decodeSymbol()) !== false) {
            codePoints.push(tmp);
        }
        return ucs2encode(codePoints);
    }

    utf8.encode = utf8encode;
    utf8.decode = utf8encode;
})();
