using Client.Services;
using Client.Views;

namespace Client;

public partial class AppShell : Shell
{
	public AppShell()
	{
		InitializeComponent();
	}

    protected override async void OnAppearing()
    {
        base.OnAppearing();
        await Inizializzazione();
    }

    private async Task Inizializzazione()
    {
        //se ci sono credenziali salvate effettuo il login
        if (UserService.Instance.IsUserSigned())
            if (await RestService.Instance.Login(UserService.Instance.Username, UserService.Instance.Password))
            {
                App.Current.MainPage = new MainTabbedPage();
                return;
            }
            else //le credenziali sono cambiate
                UserService.Instance.Logout();

        App.Current.MainPage = new NavigationPage(new LoginPage());
    }
}

