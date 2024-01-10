using Client.Exceptions;
using Client.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;

using System.Threading.Tasks;

namespace Client.Services
{
    public class RestService
    {
        private static RestService _instance;
        public static RestService Instance
        {
            get
            {
                if (_instance == null)
                    _instance = new RestService();
                return _instance;
            }
        }

        private const string urlServer = "http://localhost:1984/";

        private readonly HttpClient _client;

        public RestService()
        {
            _client = new HttpClient()
            {
                MaxResponseContentBufferSize = 10 * 1024 * 1024
            };
        }

        public async Task<bool> Login(string username, string password)
        {
            HttpResponseMessage risposta = await TalkWithServerJson(HttpMethod.Post, urlServer + "login", new UserModel
            {
                username = username,
                password = password
            });
            if (risposta.IsSuccessStatusCode)
            {
                //se il login ha successo salvo il token
                UserService.Instance.Token = await risposta.Content.ReadAsStringAsync();
                return true;
            }
            else
                return false;
        }

        public async Task<bool> Signup(string username, string password)
        {
            HttpResponseMessage risposta = await TalkWithServerJson(HttpMethod.Post, urlServer + "signup", new UserModel
            {
                username = username,
                password = password
            });
            if (risposta.IsSuccessStatusCode)
            {
                //se il signup ha successo salvo il token
                UserService.Instance.Token = await risposta.Content.ReadAsStringAsync();
                return true;
            }
            else
                return false;
        }

        public async Task<bool> UploadImage(string description, byte[] image)
        {

            HttpResponseMessage risposta = await TalkWithServerJson(HttpMethod.Post, urlServer + "upload", new ImageUploadModel
            {
                username = UserService.Instance.Username,
                description = description,
                image = Convert.ToBase64String(image) //converto i bytes dell'immagine in una stringa base 64
            });

            if (risposta.IsSuccessStatusCode)
                return true;
            else
                return false;
        }

        public async Task<List<PhotoInfoModel>> GetPosts(string query, int skip)
        {
            //creo l'endpoint corretto
            string endpoint = "get?";
            if (!string.IsNullOrEmpty(query))
                endpoint += "tag=" + query + "&";
            if (skip > 0)
                endpoint += "skip=" + skip;

            HttpResponseMessage risposta = await TalkWithServer(HttpMethod.Get, urlServer + endpoint);

            if (risposta.IsSuccessStatusCode)
            {
                //resituisco la lista di post scaricati
                var stringa = await risposta.Content.ReadAsStringAsync();
                return Utility.DeserializeJSON<List<PhotoInfoModel>>(stringa);
            }
            else
                throw new RestServiceException(risposta);
        }

        public async Task<bool> LikePost(string id, bool like)
        {

            HttpResponseMessage risposta = await TalkWithServerJson(HttpMethod.Put, urlServer + "like", new LikePostModel()
            {
                id = id,
                like = like ? "1" : "0"
            });

            if (risposta.IsSuccessStatusCode)
                return true;
            else
                return false;
        }

        // --------------- Talk with server ---------------
        private async Task<HttpResponseMessage> TalkWithServer(HttpMethod httpVerb, string url) => await TalkWithServerFinally(httpVerb, url, null);
        private async Task<HttpResponseMessage> TalkWithServerJson(HttpMethod httpVerb, string url, object request) => await TalkWithServerFinally(httpVerb, url, Utility.SerializeJSON(request));

        private async Task<HttpResponseMessage> TalkWithServerFinally(HttpMethod httpVerb, string url, string json)
        {
            try
            {
                //creo la richiesta
                HttpRequestMessage richiesta = new HttpRequestMessage
                {
                    Method = httpVerb,
                    RequestUri = new Uri(url),
                    Content = null
                };
                if (!string.IsNullOrEmpty(UserService.Instance.Token))
                    richiesta.Headers.Add("Authorization", UserService.Instance.Token); //aggiungo l'eventuale token, se disponibile
                if (!string.IsNullOrEmpty(json))
                    richiesta.Content = new StringContent(json, Encoding.UTF8, "application/json"); //aggiungo l'eventuale contenuto

                HttpResponseMessage risposta = await _client.SendAsync(richiesta);

                return risposta;
            }
            catch (Exception e)
            {
                return new HttpResponseMessage(HttpStatusCode.BadRequest);
            }
        }


    }
}
