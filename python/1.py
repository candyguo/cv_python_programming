from PIL import Image
from pylab import *

if __name__ == "__main__":
    image = np.array(Image.open("./image/1.jpg").convert('L'), 'f') / 255.0
    image = uint8(image * 255.0)
    print(image[100, 200])
    print(image.shape, image.dtype)
    imshow(image)
    x = [100, 200]
    y = [100, 200]
    plot(x, y, 'r*')
    x = ginput(3)
    print(x)
    show()
    #image.show()