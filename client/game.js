var FileCache = (function () {
    function FileCache() {
    }
    FileCache.Initialize = function () {
        var request = window.indexedDB.open("fileCache", 1);
        request.onupgradeneeded = function (event) {
        };
    };
    return FileCache;
}());
FileCache.dbHandle = null;
/// <reference path="FileCache.ts" />
var Entrypoint = (function () {
    function Entrypoint() {
    }
    Entrypoint.Main = function () {
        FileCache.Initialize();
    };
    return Entrypoint;
}());
