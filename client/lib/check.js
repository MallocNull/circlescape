// feature checking trashcode (c) obrado 1989

window.onload = function() {
    var support = {
        anim: true,
        canvas: true,
        webgl: true,
        idb: true
    };

    if(!document.getElementById("cs").getContext)
        support.canvas = false;

    // check for webgl support
    var canvas = document.getElementById("cs");
    if(!(canvas.getContext("webgl") || canvas.getContext("experimental-webgl")))
        support.webgl = false;

    // check for indexedDB support
    window.indexedDB = window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
    window.IDBTransaction = window.IDBTransaction || window.webkitIDBTransaction || window.msIDBransaction;
    window.IDBKeyRange = window.IDBKeyRange || window.webkitIDBKeyRange || window.msIDBKeyRange;
    if(!window.indexedDB)
        support.idb = false;

    var supported = true;
    for(var i in support)
        supported = supported && support[i];

    if(!supported) {
        var supportStr = "";
        for(var i in support)
            supportStr += ""+ (+support[i]);

        window.location.href = "error.html?err="+ supportStr;
        return;
    }

    Entrypoint.start();
}
