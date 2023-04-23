from facenet_pytorch import MTCNN, InceptionResnetV1
from PIL import Image
import sys
import numpy as np

def openImg(img_path):
    return Image.open(img_path)

def inference(img):
    mtcnn = MTCNN()
    resnet = InceptionResnetV1(pretrained='vggface2').eval()

    img_cropped = mtcnn(img)
    img_embedding = resnet(img_cropped.unsqueeze(0))
    return img_embedding.detach().numpy()

if __name__ == '__main__':
    # client = 1; server = 2;
    img = openImg('../img/angelina_jolie_0{party}.jpg'.format(party = sys.argv[1]))
    img_embedding = inference(img).flatten()

    np.savetxt("./encode/embedding_vector_{party}.txt".format(party = sys.argv[1]), img_embedding)
    print(img_embedding.shape)
    print(img_embedding)
    
