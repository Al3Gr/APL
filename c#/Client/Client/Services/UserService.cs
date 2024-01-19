using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Services
{
    public class UserService
    {

        private static UserService instance;

        public static UserService Instance
        {
            get
            {
                if (instance == null)
                    instance = new UserService();
                return instance;
            }
        }

        //solo per sovrascrivere il costruttore di default che è pubblico
        private UserService()
        {
        }

        //il token per le richieste successive al login/signup
        public string Token { get; set; }

        public bool IsUserSigned() => Username != null && Password != null;

        public string Username
        {
            get => Preferences.Get("username", null);
            set => Preferences.Set("username", value);
        }

        public string Password
        {
            get => Preferences.Get("password", null);
            set => Preferences.Set("password", value);
        }

        public void Logout() => Preferences.Clear();

    }
}
