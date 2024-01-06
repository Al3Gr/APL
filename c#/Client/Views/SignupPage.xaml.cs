using Client.Views.ViewModels;

namespace Client.Views;

public partial class SignupPage : ContentPage
{
	public SignupPage()
	{
		InitializeComponent();

		BindingContext = new SignupViewModel();
	}
}