using Microsoft.Maui.Storage;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Client.Services
{
    public class ImagenetService
    {

        private static ImagenetService instance;

        public static ImagenetService Instance
        {
            get
            {
                if (instance == null)
                    instance = new ImagenetService();
                return instance;
            }
        }

        private List<string> listaTag = new List<string>();

        private ImagenetService()
        {
            using (Stream stream = GetStreamFromFile("imagenet_classes.txt"))
            {
                if (stream == null)
                    return;

                using (StreamReader reader = new StreamReader(stream))
                {
                    while (true)
                    {
                        //leggo linea per linea e aggiungo i tag in un array
                        string line = reader.ReadLine();
                        if (string.IsNullOrEmpty(line))
                            break;

                        listaTag.Add(line);
                    }
                }
            }
        }

        public string FindTag(string query)
        {
            //pulisco la query togliendo gli spazi iniziali e finali e rendendola tutta minuscola
            query = query.ToLower().Trim();

            //se esiste un tag con esattamente quel nome lo restituisco
            if (listaTag.Contains(query))
                return query;

            //cerco il primo che contiene quella parte di tag
            return listaTag.Where(x => x.ToLower().Contains(query)).FirstOrDefault();
        }

        //ottengo lo stream del file embedded nell'app da aprire
        public static Stream GetStreamFromFile(string filename)
        {
            var assembly = typeof(App).GetTypeInfo().Assembly;
            var assemblyName = assembly.GetName().Name;

            var stream = assembly.GetManifestResourceStream($"{assemblyName}.{filename}");

            return stream;
        }

    }
}
