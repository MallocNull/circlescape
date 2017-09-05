// feature checking trashcode (c) obrado 1989

window.addEventListener("load", function() {
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
    Rendering.context = canvas.getContext("webgl");
    if(!Rendering.context) {
        Rendering.context = canvas.getContext("experimental-webgl");
        if(!Rendering.context)
            support.webgl = false;
    }

    // check for animation frame support
    window.requestAnimationFrame = window.requestAnimationFrame 
                                || window.webkitRequestAnimationFrame 
                                || window.mozRequestAnimationFrame 
                                || window.oRequestAnimationFrame 
                                || window.msRequestAnimationFrame;
    if(!window.requestAnimationFrame)
        support.anim = false;

    // check for indexedDB support
    window.indexedDB       = window.indexedDB 
                          || window.mozIndexedDB 
                          || window.webkitIndexedDB 
                          || window.msIndexedDB;
    window.IDBTransaction  = window.IDBTransaction 
                          || window.webkitIDBTransaction 
                          || window.msIDBransaction;
    window.IDBKeyRange     = window.IDBKeyRange 
                          || window.webkitIDBKeyRange 
                          || window.msIDBKeyRange;
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
});
