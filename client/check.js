window.onload = function() {
    var support = {
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

    if(!support.canvas || !support.webgl || !support.idb) {
        window.location.href = "error.html?err="+ (+support.canvas) 
                                             +""+ (+support.webgl) 
                                             +""+ (+support.idb);
        return;
    }

    Entrypoint.Main();
}