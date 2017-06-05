namespace CircleScape.DAL {
    using System;
    using System.Data.Entity;
    using System.ComponentModel.DataAnnotations.Schema;
    using System.Linq;

    [DbConfigurationType(typeof(MySql.Data.Entity.MySqlEFConfiguration))]
    public partial class ScapeDb : DbContext {
        static ScapeDb() {
            DbConfiguration.SetConfiguration(new MySql.Data.Entity.MySqlEFConfiguration());
        }
        
        public ScapeDb()
            : base("name=ScapeDbDebug")
        {

        }

        public DbSet<User> Users { get; set; }
        public DbSet<Origin> Origins { get; set; }


        protected override void OnModelCreating(DbModelBuilder modelBuilder) {
            base.OnModelCreating(modelBuilder);
        }
    }
}
