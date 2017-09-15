class SlaveProtocol {
    public static get packetHandlers(): PacketHandle[] {
        return [
            { id: 1, event: this.userLoginResponse }
        ];
    }

    private static userLoginResponse(data: Packet, conn: Connection): void {
        console.log("mario has logged in");
    }
}