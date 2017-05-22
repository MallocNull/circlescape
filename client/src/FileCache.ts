class FileCache {
    private static dbHandle: IDBDatabase = null;

    public static initCache(success: ()=>void, error: (error: string)=>void): void {
        var request = window.indexedDB.open("fileCache", 2);

        request.onupgradeneeded = (event: any) => {
            var db: IDBDatabase = event.target.result;
            if(db.objectStoreNames.contains("hashes"))
                db.deleteObjectStore("hashes");

            if(!db.objectStoreNames.contains("files"))
                db.createObjectStore("files", {keyPath: "Name", autoIncrement: false});

            if(!db.objectStoreNames.contains("metadata"))
                db.createObjectStore("metadata", {keyPath: "Name", autoIncrement: false});
        };

        request.onerror = (event: any) => {
            error("Could not upgrade the client database to the most recent version.");
        };

        request.onsuccess = (event: any) => {
            FileCache.dbHandle = request.result;
            success();
        };
    }

    public static getMeta(fileName: string, success: (meta: FileMeta)=>void, error: (error: string)=>void): void {
        var query = FileCache.dbHandle.transaction("metadata");
        var store = query.objectStore("metadata");
        var request = store.get(fileName);

        request.onsuccess = () => {
            success(request.result);
        };

        request.onerror = (event: any) => {
            error(event);
        };
    }

    public static setMeta(meta: FileMeta) {
        var query = FileCache.dbHandle.transaction("metadata", "readwrite");
        var store = query.objectStore("metadata");
        store.put(meta);
    }

    public static getFile(fileName: string, success: (data: Uint8Array)=>void, error: (error: string)=>void): void {
        var query = FileCache.dbHandle.transaction("files");
        var store = query.objectStore("files");
        var request = store.get(fileName);

        request.onsuccess = () => {
            success(request.result);
        };

        request.onerror = (event: any) => {
            error(event);
        };
    }

    public static setFile(fileName: string, data: Uint8Array) {
        var query = FileCache.dbHandle.transaction("files", "readwrite");
        var store = query.objectStore("files");
        store.put(data, fileName);
    }
}

class FileMeta {

}