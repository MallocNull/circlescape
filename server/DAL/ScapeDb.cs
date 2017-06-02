using System;
using System.Data.Entity;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Data.Entity.Migrations;
using System.Data.SQLite.EF6.Migrations;

namespace CircleScape.DAL {
    public partial class ScapeDb : DbContext {
        static ScapeDb() {
            Database.SetInitializer(
                new MigrateDatabaseToLatestVersion<ScapeDb, ScapeDbMigrationConfig>(true)
            );
        }

        public ScapeDb() : base("name=Database") {

        }

        public virtual DbSet<Session> Sessions { get; set; }
        public virtual DbSet<User> Users { get; set; }
        public virtual DbSet<Item> Items { get; set; }
        public virtual DbSet<ItemDefinition> ItemDefinitions { get; set; }

        protected override void OnModelCreating(DbModelBuilder modelBuilder) {
            modelBuilder.Entity<User>()
                .HasMany(e => e.Sessions)
                .WithRequired(e => e.User)
                .HasForeignKey(e => e.UserId)
                .WillCascadeOnDelete(true);

            modelBuilder.Entity<User>()
                .HasMany(e => e.Items)
                .WithRequired(e => e.User)
                .HasForeignKey(e => e.UserId)
                .WillCascadeOnDelete(true);

            modelBuilder.Entity<ItemDefinition>()
                .HasMany(e => e.Items)
                .WithRequired(e => e.Definition)
                .HasForeignKey(e => e.DefinitionId)
                .WillCascadeOnDelete(true);
        }
    }

    internal sealed class ScapeDbMigrationConfig 
        : DbMigrationsConfiguration<ScapeDb> 
    {
        public ScapeDbMigrationConfig() {
            AutomaticMigrationsEnabled = true;
            AutomaticMigrationDataLossAllowed = true;
            SetSqlGenerator("System.Data.SQLite", new SQLiteMigrationSqlGenerator());
        }
    }
}
