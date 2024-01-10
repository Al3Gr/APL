using Client.Services;
using Client.Views;

namespace Client;

public partial class App : Application
{
    public App()
    {
        InitializeComponent();

        //carico un'AppShell per fare i dovuti controlli sulle credenziali salvate
        MainPage = new AppShell();

    }

   
}