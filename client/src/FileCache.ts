class FileCache {
    static dbHandle: IDBDatabase = null;

    static Initialize(): void {
        var request = window.indexedDB.open("fileCache", 1);

        request.onupgradeneeded = function(event: any) {
            var db: IDBDatabase = event.target.result;
            db.createObjectStore("files", {keyPath: "name"});
        };

        request.onsuccess = function(event: any) {
            
        }
    }

    static 
}