using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace Glove {
    public static class CryptoExtensions {
        public static byte[] SHA1(this string str)
            => Encoding.UTF8.GetBytes(str).SHA1();

        public static byte[] SHA1(this byte[] bytes) {
            using(var hasher = new SHA1CryptoServiceProvider()) {
                return hasher.ComputeHash(bytes);
            }
        }

        public static byte[] MD5(this string str)
            => Encoding.UTF8.GetBytes(str).MD5();

        public static byte[] MD5(this byte[] bytes) {
            using(var hasher = new MD5CryptoServiceProvider()) {
                return hasher.ComputeHash(bytes);
            }
        }

        private const int HashIterations = 3000;

        public static byte[] HashPassword(this string pwd) {
            byte[] salt, hash = new byte[36];
            RNG.NextBytes(salt = new byte[16]);

            using(var hasher = new Rfc2898DeriveBytes(pwd, salt, HashIterations)) {
                byte[] rawHash = hasher.GetBytes(20);
                Array.Copy(salt, 0, hash, 0, 16);
                Array.Copy(rawHash, 0, hash, 16, 20);
            }

            return hash;
        }

        public static bool CheckPassword(this string pwd, byte[] hash) {
            byte[] salt = hash.Take(16).ToArray();

            using(var hasher = new Rfc2898DeriveBytes(pwd, salt, HashIterations)) {
                byte[] phash = hasher.GetBytes(20);
                return hash.Skip(16).SequenceEqual(phash);
            }
        }
    }
}
