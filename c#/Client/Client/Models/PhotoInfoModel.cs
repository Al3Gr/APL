using Client.Services;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Models
{
    public class PhotoInfoModel : INotifyPropertyChanged
    {
        public ObjectID _id { get; set; }
        public string username { get; set; }
        public string description { get; set; }
        public string url { get; set; }
        public List<string> tags { get; set; }
        public List<string> likes { get; set; }

        //utilizzata dall'interfaccia grafica per visualizzare l'immagine dall'url
        public ImageSource Image
        {
            get
            {
                try
                {
                    return ImageSource.FromUri(new Uri(url));
                }
                catch (Exception)
                {
                    //nel caso in cui l'url non esista o generi errori visualizzo un'immagine di default
                    return ImageSource.FromFile("nofoto.png");
                }
            }
        }

        //utilizzata dall'interfaccia grafica per visualizzare la stringa con i tag collegati all'immagine
        public string TagsString
        {
            get
            {
                string output = "";
                if (tags.Count == 0)
                    output = "Non ci sono tag!";

                foreach (string tag in tags)
                    output += tag + " - ";
                if (tags.Count > 0)
                    output = output.Substring(0, output.Length - 3);

                return output;
            }
        }

        //restituisce se l'utente loggato ha messo like al post
        public bool HasMyLike
        {
            get
            {
                string username = UserService.Instance.Username;
                if (username == null)
                    return false;
                return likes.Contains(username);
            }
        }

        //utilizzata dall'interfaccia grafica per visualizzare la presenza del proprio like
        public ImageSource LikeImageSource
        {
            get => HasMyLike ? ImageSource.FromFile("mipiace.png") : ImageSource.FromFile("nomipiace.png");
        }

        //utilizzata dall'interfaccia grafica per visualizzare il numero di like del post
        public int NLike
        {
            get => likes.Count();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        //utilizzata dalla view model per notificare i cambiamenti delle informazioni sui like del post
        public void NotifyHasMyLikeChanged()
        {
            NotifyPropertyChanged(nameof(LikeImageSource));
            NotifyPropertyChanged(nameof(NLike));
        }

        //metodo per notificare gli observer che osservano l'evento PropertyChanged
        private void NotifyPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }


        //metodo per evitare che il parsing del json dia errore a causa di proprietà in più date dal server
        [JsonExtensionData]
        public IDictionary<string, JToken> AdditionalData { get; set; }
    }

    public class ObjectID
    {
        [JsonProperty("$oid")]
        public string Id { get; set; }
    }
}
