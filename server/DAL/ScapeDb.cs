using System;
using System.Data.Entity;
using System.ComponentModel.DataAnnotations.Schema;
using System.IO;
using System.Linq;
using Config = SockScape.Configuration;

namespace SockScape.DAL {
    [DbConfigurationType(typeof(MySql.Data.Entity.MySqlEFConfiguration))]
    internal partial class ScapeDb : DbContext {
        private static readonly string Dsn;

        static ScapeDb() {
            DbConfiguration.SetConfiguration(new MySql.Data.Entity.MySqlEFConfiguration());

            // fix for migrations
            if(!Directory.GetCurrentDirectory().ToLower().Contains(":\\windows")) {
                Dsn = "server=" + Config.Database["Server"]
                    + ";port=" + (Config.Database["Port"] ?? "3306")
                    + ";user id=" + Config.Database["Username"]
                    + ";password=" + Config.Database["Password"]
                    + ";persistsecurityinfo=True;"
                    + "database=" + Config.Database["Database"];
            } else {
                Dsn = "name=ScapeDb";
            }
        }

        public ScapeDb() : base(Dsn) { }

        public DbSet<User> Users { get; set; }
        public DbSet<Origin> Origins { get; set; }
        public DbSet<Session> Sessions { get; set; }
        public DbSet<ItemMaster> ItemMaster { get; set; }
        public DbSet<Item> Items { get; set; }

        protected override void OnModelCreating(DbModelBuilder builder) {
            base.OnModelCreating(builder);
            
            builder.Properties<string>()
                .Configure(s => s.HasMaxLength(256).HasColumnType("varchar"));

            builder.Entity<ItemMaster>().ToTable("ItemMaster");
        }
    }
}
