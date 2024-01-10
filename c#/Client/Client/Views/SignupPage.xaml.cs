using Client.Views.ViewModels;

namespace Client.Views;

public partial class SignupPage : ContentPage
{
	public SignupPage()
	{
		InitializeComponent();

        //effettuo il binding della page con la viewmodel 
        BindingContext = new SignupViewModel();
	}
}