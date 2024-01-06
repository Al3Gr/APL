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
    public class ProfiloViewModel
    {

        private string description;
        private bool isLoading;

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
                NotifyPropertyChanged(nameof(Description));
            }
        }

        private byte[] image;

        public ImageSource ImageSource { get {
                if (image == null)
                    return ImageSource.FromFile(""); //immagine per "NO FOTO"
                return ImageSource.FromStream(() => new MemoryStream(image));
            }
        }

        public ICommand PickImage { get; set; }
        public ICommand Upload { get; set; }

        public ProfiloViewModel()
        {
            Username = UserService.Instance.Username;

            PickImage = new Command(PickImageClicked);
            Upload = new Command(UploadImageClicked);
        }

        public event PropertyChangedEventHandler PropertyChanged;

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

            if (await RestService.Instance.UploadImage(Username, Description, image))
            {
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
            FileResult file = await MediaPicker.PickPhotoAsync(new MediaPickerOptions
            {
                Title = "Seleziona un'immagine"
            });

            if (file != null)
            {
                var imageStream = File.OpenRead(file.FullPath);

                image = new byte[imageStream.Length];
                imageStream.Position = 0;
                imageStream.Read(image, 0, image.Length);

                NotifyPropertyChanged(nameof(ImageSource));
            }
        }

    }
}
