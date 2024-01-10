using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client
{
    public static class Utility
    {
        public static string SerializeJSON(object oggetto) => JsonConvert.SerializeObject(oggetto);

        public static T DeserializeJSON<T>(string stringa) => JsonConvert.DeserializeObject<T>(stringa);
    }
}
