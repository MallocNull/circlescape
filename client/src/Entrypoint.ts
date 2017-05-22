/// <reference path="FileCache.ts" />
/// <reference path="Utilities.ts" />

class Entrypoint {
    private static initStatus = {
        fileCache: false
    }

    private static initCheck(): void {
        var done = true;
        for(var i in Entrypoint.initStatus)
            done = done && Entrypoint.initStatus[i];

        if(done)
            Entrypoint.ready();
    }

    public static start(): void {
        FileCache.initCache(
            // SUCCESS 
            () => { 
                Entrypoint.initStatus.fileCache = true;
                this.initCheck();
            },         

            // FAILURE
            (error: string) => { 
                CriticalStop.redirect(error);
            } 
        ); 
    }

    private static ready(): void {

    }
}