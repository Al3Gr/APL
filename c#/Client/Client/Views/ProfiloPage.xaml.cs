namespace Client.Views;
using Client.Views.ViewModels;

public partial class ProfiloPage : ContentPage
{
	public ProfiloPage()
	{
		InitializeComponent();


        //effettuo il binding della page con la viewmodel 
        BindingContext = new ProfiloViewModel();
	}
}