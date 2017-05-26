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
        Key.init();

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
        
        Connection.open();
    }

    private static ready(): void {

    }
}