class FileCache {
    private static DB: IDBDatabase = null;

    public static Initialize(success: ()=>void, error: (error: string)=>void): void {
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
            FileCache.DB = request.result;
            success();
        };
    }

    public static GetHash(fileName: string, success: (hash: string)=>void, error: (error: string)=>void): void {
        var query = FileCache.DB.transaction("metadata");
        var store = query.objectStore("metadata");
        var request = store.get(fileName);

        request.onsuccess = () => {
            success(request.result);
        };

        request.onerror = (event: any) => {
            error(event);
        };
    }

    public static SetHash(fileName: string, hash: string) {
        var query = FileCache.DB.transaction("metadata", "readwrite");
        var store = query.objectStore("metadata");
        store.put({Name: fileName, Hash: hash});
    }

    public static GetFile(fileName: string, success: (data: Uint8Array)=>void, error: (error: string)=>void): void {
        var query = FileCache.DB.transaction("files");
        var store = query.objectStore("files");
        var request = store.get(fileName);

        request.onsuccess = () => {
            success(request.result);
        };

        request.onerror = (event: any) => {
            error(event);
        };
    }

    public static SetFile(fileName: string, data: Uint8Array) {
        var query = FileCache.DB.transaction("files", "readwrite");
        var store = query.objectStore("files");
        store.put(data, fileName);
    }
}