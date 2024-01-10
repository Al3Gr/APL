using Client.Views.ViewModels;

namespace Client.Views;

public partial class PostsPage : ContentPage
{
	public PostsPage()
	{
		InitializeComponent();

        //effettuo il binding della page con la viewmodel e inserisco nessun tag
        BindingContext = new PostsViewModel(this, "");

	}

    //metodo chiamato nel momento in cui la pagina appare a schermo
    //utile quando carico una nuova immagine e voglio vederla immediatamente
    protected override void OnAppearing()
    {
        base.OnAppearing();

		PostsViewModel postsViewModel = BindingContext as PostsViewModel;
		postsViewModel.Refresh();
    }
}