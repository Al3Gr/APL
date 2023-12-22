import torch
import base64
import socket
import os
import json
from mongoDB import MongoDB
from PIL import Image
from torchvision import transforms
from torchvision.models import AlexNet_Weights, alexnet
import threading

# import ssl
# ssl._create_default_https_context = ssl._create_unverified_context

with open("config_tagger.json", "r") as j:
    config = json.load(j)

HOST = config["Host"]
PORT = config["Port"]

model = alexnet(weights=AlexNet_Weights.DEFAULT)
model.eval()

preprocess = transforms.Compose([
    transforms.Resize(256),
    transforms.CenterCrop(224),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.485, 0.456, 0.406],
                         std=[0.229, 0.224, 0.225]),
])


def session(conn, addr):
    id = threading.get_ident()
    with conn:
        print(f'Connected with {addr}')
        size_b = conn.recv(4)
        size = int.from_bytes(size_b, "little")
        print(f"Image's dimension: {size}")
        with open(f"server_image_{id}.jpeg", "wb") as f:
            while size > 0:
                image_64 = conn.recv(2048)
                image_data = base64.b64decode(image_64)
                f.write(image_data)
                size = size - len(image_64)
            print("Server image received")
        input_image = Image.open(f"server_image_{id}.jpeg")
        input_tensor = preprocess(input_image)
        input_batch = input_tensor.unsqueeze(0)

        if torch.cuda.is_available():
            input_batch = input_batch.to('cuda')
            model.to('cuda')
        with torch.no_grad():
            output = model(input_batch)

        probabilities = torch.nn.functional.softmax(output[0], dim=0)

        with open("imagenet_classes.txt", "r") as f:
            # strip() rimuove gli spazi bianchi dietro e davanti
            categories = [s.strip() for s in f.readlines()]

        top_n_prob, top_n_catid = torch.topk(probabilities, config["N_tags"])
        for i in range(top_n_prob.size(0)):
            print(categories[top_n_catid[i]], top_n_prob[i].item())

        print("Send response to client")
        conn.sendall(categories[top_n_catid[0]].encode())
        os.remove(f"server_image_{id}.jpeg")


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    print("Listen incoming connections")
    s.bind((HOST, PORT))
    s.listen()
    while True:
        conn, addr = s.accept()
        print("Start thread")
        threading.Thread(target=session, args=(conn, addr)).start()
