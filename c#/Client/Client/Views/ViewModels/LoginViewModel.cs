using Client.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace Client.Views.ViewModels
{
    public class LoginViewModel : INotifyPropertyChanged
    {
        private bool isLoading;

        //proprietà per indicare che la finestra sta caricando
        public bool IsLoading
        {
            get => isLoading;
            set
            {
                isLoading = value;
                NotifyPropertyChanged(nameof(IsLoading));
                NotifyPropertyChanged(nameof(IsNotLoading));
            }
        }

        public bool IsNotLoading
        {
            get => !isLoading;
        }

        public string Username { get; set; }
        public string Password { get; set; }
        public ICommand Login { get; set; }
        public ICommand Signup { get; set; }

        public LoginViewModel()
        {
            Login = new Command(execute: LoginClicked, canExecute: () =>
            {
                return !IsLoading;
            });

            Signup = new Command(execute: async () =>
                {
                    SignupPage page = new SignupPage();
                    if (!string.IsNullOrEmpty(Username)) //se l'utente ha già inserito l'username nel login lo facilito nel signup inserendolo in automatico nella nuova pagina
                        page.BindingContext = new SignupViewModel(Username);
                    await App.Current.MainPage.Navigation.PushAsync(page);
                }, canExecute: () =>
                {
                    return !IsLoading;
                });
        }

        public event PropertyChangedEventHandler PropertyChanged;

        //metodo per notificare gli observer che osservano l'evento PropertyChanged
        private void NotifyPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private async void LoginClicked()
        {
            IsLoading = true;

            if (await RestService.Instance.Login(Username, Password))
            {
                //se il login ha successo setto le credenziali e visualizzo la tabbedpage

                UserService.Instance.Username = Username;
                UserService.Instance.Password = Password;
                App.Current.MainPage = new MainTabbedPage();
            }
            else
                await App.Current.MainPage.DisplayAlert("Attenzione", "Nome utente o password non validi", "Ok");

            IsLoading = false;
        }
    }
}
