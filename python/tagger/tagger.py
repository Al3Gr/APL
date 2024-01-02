from torchvision.models import AlexNet_Weights, alexnet
from torchvision import transforms
from PIL import Image
import torch

# La classe Tagger modella il tagger
class Tagger():

    def __init__(self, n_tags):
        # In fase di creazione, oltre ad inizializzare la rete che si occuperà
        # dell'inferenza dell'immagine, setto anche quanti tag restituirà il tagger
        self.n_tags = n_tags
        self.tags = []
        self.model = alexnet(weights=AlexNet_Weights.DEFAULT)
        self.model.eval()
        self.preprocess = transforms.Compose([
            transforms.Resize(256),
            transforms.CenterCrop(224),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.485, 0.456, 0.406],
                                 std=[0.229, 0.224, 0.225]),
        ])

    def elaborate_image(self, image):
        # In questa funzione effettuo l'inferenza dell'immagine
        input_image = Image.open(image)
        input_tensor = self.preprocess(input_image)
        input_batch = input_tensor.unsqueeze(0)
        if torch.cuda.is_available():
            input_batch = input_batch.to('cuda')
            self.model.to('cuda')
        with torch.no_grad():
            output = self.model(input_batch)

        self.probabilities = torch.nn.functional.softmax(output[0], dim=0)

    def get_tags(self):
        # In questa funzione restituisco i tag relativi all'immagine inviata
        # In particolare questi tag vengono letti dal seguente file
        with open("imagenet_classes.txt", "r") as f:
            categories = [s.strip() for s in f.readlines()]
        top_n_prob, top_n_catid = torch.topk(
            self.probabilities, self.n_tags)

        tags = []

        for i in range(0, self.n_tags):
            tags.append(categories[top_n_catid[i]])

        return tags
