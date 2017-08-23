namespace SockScape.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class Initial : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "dbo.Origins",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        UserId = c.Long(nullable: false),
                        RawIp = c.Binary(nullable: false, storeType: "binary(16)"),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("dbo.Users", t => t.UserId, cascadeDelete: true)
                .Index(t => new { t.UserId, t.RawIp }, unique: true, name: "IX_RawIp_UserId_Unique");
            
            CreateTable(
                "dbo.Users",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        Username = c.String(nullable: false, maxLength: 16, storeType: "varchar"),
                        Password = c.Binary(nullable: false, storeType: "binary(36)"),
                        Joined = c.DateTime(nullable: false, precision: 0),
                        LastLogin = c.DateTime(precision: 0),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateTable(
                "dbo.Sessions",
                c => new
                    {
                        Id = c.Long(nullable: false),
                        Secret = c.Binary(nullable: false, storeType: "binary(16)"),
                        ServerId = c.Int(nullable: false),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("dbo.Users", t => t.Id)
                .Index(t => t.Id)
                .Index(t => t.Secret, unique: true);
            
        }
        
        public override void Down()
        {
            DropForeignKey("dbo.Origins", "UserId", "dbo.Users");
            DropForeignKey("dbo.Sessions", "Id", "dbo.Users");
            DropIndex("dbo.Sessions", new[] { "Secret" });
            DropIndex("dbo.Sessions", new[] { "Id" });
            DropIndex("dbo.Origins", "IX_RawIp_UserId_Unique");
            DropTable("dbo.Sessions");
            DropTable("dbo.Users");
            DropTable("dbo.Origins");
        }
    }
}
