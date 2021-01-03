import os
from PIL import Image
import numpy as np

def get_imglist(path, suffix = "jpg"):
    return [os.path.join(path, f) for f in os.listdir(path) if f.endswith(suffix)]

def im_resize(im, sz):
    pil_image = Image.fromarray(np.uint8(im))
    return np.array(pil_image.resize(sz))

def compute_average(im_list):
    average_im = np.array(Image.open(im_list[0]), 'f')
    for im_name in im_list[1:]:
        try:
            average_im += np.array(Image.open(im_name), 'f')
        except:
            print("{} skipped".format(im_name))
    average_im /= len(im_list)
    return np.array(average_im, np.uint8)        

if __name__ == "__main__":
    files = get_imglist("/home/ccfy/study/data/CelebA/CelebA/Img/img_align_celeba")
    print(files)

