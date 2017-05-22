class Connection {
    private static Sock: WebSocket = null;
    private static _IsOpen: boolean = false;
    public static get IsOpen(): boolean {
        return Connection._IsOpen;
    }

    public static Initialize(): void {
        Connection.Sock
    }

    public static Close(): void {

    }

    private static 
}