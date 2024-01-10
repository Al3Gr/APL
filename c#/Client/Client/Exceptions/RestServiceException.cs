using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Exceptions
{
    public class RestServiceException : Exception
    {
        public HttpResponseMessage risposta;

        public RestServiceException(HttpResponseMessage risposta)
        {
            this.risposta = risposta;
        }
    }
}
