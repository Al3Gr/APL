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

        private const string urlServer = "http://localhost:/1984";

        private readonly HttpClient _client = new HttpClient()
        {
            MaxResponseContentBufferSize = 10 * 1024 * 1024
        };

        public async Task<bool> Login(string username, string password)
        {
            HttpResponseMessage risposta = await TalkWithServerJson(HttpMethod.Post, urlServer + "login", new UserModel
            {
                username = username,
                password = password
            });
            if (risposta.IsSuccessStatusCode)
                return true;
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
                return true;
            else
                return false;
        }

        public async Task<bool> UploadImage(string username, string description, byte[] image)
        {

            HttpResponseMessage risposta = await TalkWithServerJson(HttpMethod.Post, urlServer + "upload", new ImageUploadModel
            {
                username = username,
                description = description,
                image = Convert.ToBase64String(image)
            });

            if (risposta.IsSuccessStatusCode)
                return true;
            else
                return false;
        }

        // --------------- Talk with server ---------------
        private async Task<HttpResponseMessage> TalkWithServerJson(HttpMethod httpVerb, string url, object request) => await TalkWithServerFinally(httpVerb, url, Utility.SerializeJSON(request));

        private async Task<HttpResponseMessage> TalkWithServerFinally(HttpMethod httpVerb, string url, string json)
        {
            try
            {
                if (!string.IsNullOrEmpty(UserService.Instance.Token))
                    _client.DefaultRequestHeaders.Authorization = new System.Net.Http.Headers.AuthenticationHeaderValue("", UserService.Instance.Token);

                HttpRequestMessage richiesta = new HttpRequestMessage
                {
                    Method = httpVerb,
                    RequestUri = new Uri(url),
                    Content = null,
                };
                if (!string.IsNullOrEmpty(json))
                    richiesta.Content = new StringContent(json, Encoding.UTF8, "application/json");

                HttpResponseMessage risposta = await _client.SendAsync(richiesta);

                if (risposta.IsSuccessStatusCode)
                {
                    if (risposta.Headers.TryGetValues("X-Token", out IEnumerable<string> stringhe))
                        UserService.Instance.Token = stringhe.FirstOrDefault();
                }

                return risposta;
            }
            catch (Exception e)
            {
                return new HttpResponseMessage(HttpStatusCode.BadRequest);
            }
        }


    }
}
