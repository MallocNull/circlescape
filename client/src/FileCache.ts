class FileCache {
    static dbHandle: IDBDatabase = null;

    static Initialize(): void {
        var request = window.indexedDB.open("fileCache", 1);

        request.onupgradeneeded = function(event) {
            
        };
    }
}