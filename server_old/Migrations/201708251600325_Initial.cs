namespace SockScape.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class Initial : DbMigration
    {
        public override void Up()
        {
            CreateTable(
                "ItemMaster",
                c => new
                    {
                        Id = c.Int(nullable: false, identity: true),
                        Name = c.String(nullable: false, maxLength: 64, unicode: false),
                        Description = c.String(nullable: false, maxLength: 256, unicode: false),
                        Stackable = c.Boolean(nullable: false),
                        MaxStack = c.Long(nullable: false),
                        Droppable = c.Boolean(nullable: false),
                        Sheet = c.String(nullable: false, maxLength: 64, unicode: false),
                        Row = c.Int(nullable: false),
                        Column = c.Int(nullable: false),
                        Object = c.String(maxLength: 64, unicode: false),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateTable(
                "Items",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        MasterId = c.Int(nullable: false),
                        UserId = c.Long(nullable: false),
                        Quantity = c.Long(nullable: false),
                        Sequence = c.Long(nullable: false),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("ItemMaster", t => t.MasterId, cascadeDelete: true)
                .ForeignKey("Users", t => t.UserId, cascadeDelete: true)
                .Index(t => t.MasterId)
                .Index(t => t.UserId);
            
            CreateTable(
                "Users",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        Username = c.String(nullable: false, maxLength: 16, unicode: false),
                        Password = c.Binary(nullable: false, storeType: "binary(36)"),
                        Joined = c.DateTime(nullable: false, precision: 0),
                        LastLogin = c.DateTime(precision: 0),
                    })
                .PrimaryKey(t => t.Id);
            
            CreateTable(
                "Origins",
                c => new
                    {
                        Id = c.Long(nullable: false, identity: true),
                        UserId = c.Long(nullable: false),
                        RawIp = c.Binary(nullable: false, storeType: "binary(16)"),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("Users", t => t.UserId, cascadeDelete: true)
                .Index(t => new { t.UserId, t.RawIp }, unique: true, name: "IX_RawIp_UserId_Unique");
            
            CreateTable(
                "Sessions",
                c => new
                    {
                        Id = c.Long(nullable: false),
                        Secret = c.Binary(nullable: false, storeType: "binary(16)"),
                        ServerId = c.Int(nullable: false),
                    })
                .PrimaryKey(t => t.Id)
                .ForeignKey("Users", t => t.Id)
                .Index(t => t.Id)
                .Index(t => t.Secret, unique: true);
            
        }
        
        public override void Down()
        {
            DropForeignKey("Items", "UserId", "Users");
            DropForeignKey("Sessions", "Id", "Users");
            DropForeignKey("Origins", "UserId", "Users");
            DropForeignKey("Items", "MasterId", "ItemMaster");
            DropIndex("Sessions", new[] { "Secret" });
            DropIndex("Sessions", new[] { "Id" });
            DropIndex("Origins", "IX_RawIp_UserId_Unique");
            DropIndex("Items", new[] { "UserId" });
            DropIndex("Items", new[] { "MasterId" });
            DropTable("Sessions");
            DropTable("Origins");
            DropTable("Users");
            DropTable("Items");
            DropTable("ItemMaster");
        }
    }
}
