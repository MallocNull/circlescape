class MasterProtocol {
    public static get packetHandlers(): PacketHandle[] {
        return [
            { id: 1, event: this.keyExchange }
        ];
    }

    private static keyExchange(data: Packet, conn: Connection): void {
        var response = Key.generateResponsePacket(data);
        if(Key.succeeded) {
            Cipher.init(Key.privateKey);
            conn.send(response);
        } else
            CriticalStop.redirect("Could not establish an encrypted connection with the server.");
    }

    public static loginAttempt(username: string, password: string): void {
        
    }
}