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
    public class SignupViewModel : INotifyPropertyChanged
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
        public string ConfirmPassword { get; set; }
        public ICommand Signup { get; set; }

        public SignupViewModel(string username) : this()
        {
            Username = username;
        }

        public SignupViewModel() { 
            Signup = new Command(SignupClicked);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        //metodo per notificare gli observer che osservano l'evento PropertyChanged
        private void NotifyPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private async void SignupClicked()
        {
            //controlli di correttezza dei dati
            if (string.IsNullOrEmpty(Username) || string.IsNullOrEmpty(Password))
            {
                await App.Current.MainPage.DisplayAlert("Attenzione!", "Riempire i campi username e password", "Ok");
                return;
            }
            if(ConfirmPassword != Password)
            {
                await App.Current.MainPage.DisplayAlert("Attenzione!", "Il campo password e conferma password devonon coincidere", "Ok");
                return;
            }

            IsLoading = true;

            if (await RestService.Instance.Signup(Username, Password))
            {
                //se il signup ha successo setto le credenziali e visualizzo la tabbedpage
                UserService.Instance.Username = Username;
                UserService.Instance.Password = Password;
                App.Current.MainPage = new MainTabbedPage();
            }
            else
                await App.Current.MainPage.DisplayAlert("Attenzione", "Nome utente già in uso", "Ok");

            IsLoading = false;
        }
    }
}
