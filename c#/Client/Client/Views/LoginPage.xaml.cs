using Client.Views.ViewModels;

namespace Client.Views;

public partial class LoginPage : ContentPage
{
	public LoginPage()
	{
		InitializeComponent();

		//effettuo il binding della page con la viewmodel
		BindingContext = new LoginViewModel();
	}
}