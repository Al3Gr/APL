using Client.Services;
using Microsoft.Maui.Storage;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace Client.Views.ViewModels
{
    public class ProfiloViewModel : INotifyPropertyChanged
    {

        private string description;
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

        public string Description
        {
            get => description;
            set
            {
                description = value;
                //utilizzata per refreshare la GUI quando resetto la descrizione dopo l'upload con successo
                NotifyPropertyChanged(nameof(Description));
            }
        }

        private byte[] image;

        //utilizzata dall'interfaccia grafica per visualizzare l'immagine selezionata
        public ImageSource ImageSource { get {
                if (image == null)
                    return ImageSource.FromFile("nofoto.png"); //immagine per "NO FOTO"
                return ImageSource.FromStream(() => new MemoryStream(image));
            }
        }

        public ICommand PickImage { get; set; }
        public ICommand Upload { get; set; }
        public ICommand Logout { get; set; }

        public ProfiloViewModel()
        {
            Username = UserService.Instance.Username;

            PickImage = new Command(PickImageClicked);
            Upload = new Command(UploadImageClicked);
            Logout = new Command(() =>
            {
                UserService.Instance.Logout();
                App.Current.MainPage = new AppShell();
            });
        }

        public event PropertyChangedEventHandler PropertyChanged;

        //metodo per notificare gli observer che osservano l'evento PropertyChanged
        private void NotifyPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private async void UploadImageClicked()
        {
            if(image == null)
            {
                await App.Current.MainPage.DisplayAlert("Attenzione!", "Caricare la foto dalla galleria!", "Ok");
                return;
            }

            IsLoading = true;

            if (await RestService.Instance.UploadImage(Description, image))
            {
                //resetto le informazioni per la creazione di un post
                Description = "";

                image = null;
                NotifyPropertyChanged(nameof(ImageSource));

                await App.Current.MainPage.DisplayAlert("Pubblicata!", "Foto pubblicata con successo", "Ok");
            }
            else
                await App.Current.MainPage.DisplayAlert("Attenzione!", "Qualcosa è andata storto", "Ok");

            IsLoading = false;
        }

        private async void PickImageClicked()
        {
            //con .net maui prendo un'immagine dal dispositivo
            FileResult file = await MediaPicker.PickPhotoAsync(new MediaPickerOptions
            {
                Title = "Seleziona un'immagine"
            });

            if (file != null)
            {
                //apro il file
                var imageStream = File.OpenRead(file.FullPath);

                image = new byte[imageStream.Length];
                imageStream.Position = 0;
                imageStream.Read(image, 0, image.Length); //leggo i byte

                //notifico il cambiamento dell'immagine per la visualizzazione prima dell'upoload
                NotifyPropertyChanged(nameof(ImageSource));
            }
        }

    }
}
