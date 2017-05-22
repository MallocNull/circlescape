/// <reference path="def/UTF8.d.ts" />

// ** STRING EXTENSIONS ** \\

interface String {
    replaceAll(needle: string[], replace: string, ignoreCase?: boolean): string;
    replaceAll(needle: string[], replace: string[], ignoreCase?: boolean): string;
    replaceAll(needle: string, replace: string, ignoreCase?: boolean): string;
    contains(needle: string, ignoreCase?: boolean): boolean;

    stripCharacters(chars: string): string;

    hasUnicodeCharacters(): boolean;
    toByteArray(): Uint8Array;
    byteLength(): number;
}

String.prototype.replaceAll = function(needle: any, replace: any, ignoreCase: boolean = false): string {
    if((typeof needle) == "string")
        return this.replace(new RegExp(needle.replace(/([\/\,\!\\\^\$\{\}\[\]\(\)\.\*\+\?\|\<\>\-\&])/g,"\\$&"),(ignoreCase?"gi":"g")),(typeof(replace)=="string")?replace.replace(/\$/g,"$$$$"):replace);
    else {
        var retval = this;
        for(var i in needle) {
            if((typeof replace) == "string")
                retval = retval.replaceAll(needle[i], replace, ignoreCase);
            else
                retval = retval.replaceAll(needle[i], replace[i], ignoreCase);
        }
        return retval;
    }
};

String.prototype.contains = function(needle: string, ignoreCase: boolean = false): boolean {
    return ignoreCase
        ? this.toLowerCase().indexOf(needle.toLowerCase()) != -1
        : this.indexOf(needle) != -1;
};

String.prototype.stripCharacters = function(chars: string) {
    var copy = this;
    if(chars != "")
        copy = copy.replaceAll(chars.split(""), "");

    return copy;
};

String.prototype.hasUnicodeCharacters = function() {
    for(var i = 0; i < this.length; i++) {
        if(this.charCodeAt(i) > 127) return true;
    }
    return false;
};

String.prototype.byteLength = function() {
    return utf8.encode(this).length;
};

String.prototype.toByteArray = function() {
    var str = utf8.encode(this);
    var ret = new Uint8Array(str.length);
    for(var i = 0; i < str.length; i++)
        ret[i] = str.charCodeAt(i);

    return ret;
};


// ** DATE EXTENSIONS ** \\

interface DateConstructor {
    unixNow(): number;
}

interface Date {
    toUnixTime(): number;
    /*ToDateTimeString(): string;
    ToTimeString(): string;*/
}

Date.unixNow = function() {
    return (new Date()).toUnixTime();
};

Date.prototype.toUnixTime = function() {
    return Math.floor(this.getTime()/1000);
};

/*Date.prototype.ToDateTimeString = function() {
    return this.toDateString() +" @ "+ this.getHours().zeroPad() +":"+ this.getMinutes().zeroPad() +":"+ this.getSeconds().zeroPad();
};

Date.prototype.ToTimeString = function() {
    return this.getHours().zeroPad() +":"+ this.getMinutes().zeroPad() +":"+ this.getSeconds().zeroPad();
};*/


// ** NUMBER EXTENSIONS ** \\

interface Number {
    zeroPad(mag?: number): string;
    packBytes(bytes: number): Uint8Array;
}

Number.prototype.zeroPad = function(mag: number = 1): string {
    var ret = ""+ this;
    for(; this < Math.pow(10, mag) && mag != 0; --mag)
        ret = "0" + ret;
    return ret;
};

Number.prototype.packBytes = function(bytes: number) {
    var ret = new Uint8Array(bytes);
    for(var i = 0; i < bytes; i++)
        ret[i] = (this & (0xFF << 8 * (bytes - 1 - i))) >>> 8 * (bytes - 1 - i);
    return ret;
};


// ** UINT8ARRAY EXTENSIONS ** \\

interface Uint8Array {
    unpackBytes(): number;
}

Uint8Array.prototype.unpackBytes = function(): number {
    var ret = 0;
    for(var i = 0; i < this.length; i++)
        ret = ret | ((this[i] & 0xFF) << 8*(this.length - 1 - i));
    return ret;
};

Uint8Array.prototype.toString = function(): string {
    var chunkSize = 4096;
    var raw = "";
    for(var i = 0;; i++) {
        if(this.length < chunkSize*i) break;
        raw += String.fromCharCode.apply(null, this.subarray(chunkSize*i, chunkSize*i + chunkSize));
    }
    return utf8.decode(raw);
};