using Client.Services;
using Client.Views;

namespace Client;

public partial class App : Application
{
	public App()
	{
		InitializeComponent();

		Inizializzazione();
	}

	private async void Inizializzazione()
	{
		if (UserService.Instance.IsUserSigned())
			if (await RestService.Instance.Login(UserService.Instance.Username, UserService.Instance.Password)) {
				MainPage = new MainTabbedPage();
				return;
			}

        MainPage = new NavigationPage(new LoginPage());
    }
}
