import torch
import socket
from PIL import Image
from torchvision import transforms
from torchvision.models import AlexNet_Weights, alexnet
import time
import threading

# import ssl
# ssl._create_default_https_context = ssl._create_unverified_context

HOST = "127.0.0.1"
PORT = 10001

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
    # time.sleep(10) mi serve per simulare più connessioni insieme
    with conn:
        print(f'Connected with {addr}')
        size_b = conn.recv(8)
        # print(f"Image's dimension: {size_b}")
        size = int.from_bytes(size_b, "little")
        print(f"Image's dimension: {size}")
        with open(f"server_image_{id}.jpeg", "wb") as f:
            # print(size)
            # image_data = conn.recv(size)
            # print(image_data)
            # f.write(image_data)
            while size > 0:
                # print("elaboro")
                image_data = conn.recv(2048)
                f.write(image_data)
                size = size - len(image_data)
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

        top5_prob, top5_catid = torch.topk(probabilities, 1)
        for i in range(top5_prob.size(0)):
            print(categories[top5_catid[i]], top5_prob[i].item())

        print("Send response to client")
        conn.sendall(categories[top5_catid[0]].encode())


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    print("Listen incoming connections")
    s.bind((HOST, PORT))
    s.listen()
    while True:
        conn, addr = s.accept()
        print("Start thread")
        threading.Thread(target=session, args=(conn, addr)).start()