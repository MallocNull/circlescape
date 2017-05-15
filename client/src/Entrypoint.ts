/// <reference path="FileCache.ts" />
/// <reference path="Utilities.ts" />

class Entrypoint {
    private static InitStatus = {
        FileCache: false
    }

    private static InitCheck(): void {
        var done = true;
        for(var i in Entrypoint.InitStatus)
            done = done && Entrypoint.InitStatus[i];

        if(done)
            Entrypoint.Initialized();
    }

    public static Start(): void {
        FileCache.Initialize(
            // SUCCESS 
            () => { 
                Entrypoint.InitStatus.FileCache = true;
                this.InitCheck();
            },         

            // FAILURE
            (error: string) => { 
                CriticalStop.Redirect(error);
            } 
        ); 
    }

    private static Initialized(): void {

    }
}