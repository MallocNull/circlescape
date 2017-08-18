using System;
using System.Data.Entity;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using Config = SockScape.Configuration;

namespace SockScape.DAL {
    [DbConfigurationType(typeof(MySql.Data.Entity.MySqlEFConfiguration))]
    public partial class ScapeDb : DbContext {
        static ScapeDb() {
            DbConfiguration.SetConfiguration(new MySql.Data.Entity.MySqlEFConfiguration());
        }
        
        public ScapeDb()
            : base("server="+ Config.Database["Server"] 
                  +";user id="+ Config.Database["Username"] 
                  +";password="+ Config.Database["Password"]
                  +";persistsecurityinfo=True;"
                  +"database="+ Config.Database["Database"])
        {

        }

        public DbSet<User> Users { get; set; }
        public DbSet<Origin> Origins { get; set; }
        public DbSet<Session> Sessions { get; set; }


        protected override void OnModelCreating(DbModelBuilder modelBuilder) {
            base.OnModelCreating(modelBuilder);
        }
    }
}
