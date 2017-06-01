namespace CircleScape.DAL {
    using System;
    using System.Data.Entity;
    using System.ComponentModel.DataAnnotations.Schema;
    using System.Linq;

    public partial class Database : DbContext {
        public Database()
            : base("name=Database") {
        }

        public virtual DbSet<Session> Sessions { get; set; }
        public virtual DbSet<User> Users { get; set; }

        protected override void OnModelCreating(DbModelBuilder modelBuilder) {
            modelBuilder.Entity<User>()
                .HasMany(e => e.Sessions)
                .WithRequired(e => e.User)
                .WillCascadeOnDelete(false);
        }
    }
}
