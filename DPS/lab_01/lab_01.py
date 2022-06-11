import pathlib as path
from datetime import datetime as dt

from PIL import Image
import cv2
import numpy as np

IMG_DEFAULT = "../static/bl2049_small.jpg"
SAVE_FOLDER = "./save"

def create_save_folder(save_folder=SAVE_FOLDER):
    '''
    Creates subfolder in a current working directory
    '''
    p = path.Path(save_folder)
    p.mkdir(parents=True, exist_ok=True)
    return str(p.absolute())

def get_gray_img(source="../static/bl2049_small.jpg"):
    img = cv2.imread(source)
    g_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return g_img

def get_lib_Gauss_blur(g_img, kernel_n=7, sigma=6):
    return cv2.GaussianBlur(g_img,
                            (kernel_n,kernel_n),
                            sigma, sigma)

def Gauss_func(x, mu, sigma):
    return (1 / (sigma*(np.sqrt(2*np.pi)))) * np.exp((np.power((x-mu)/sigma, 2)) / (-2))

def Gauss_kernel(kernel_n, sigma=1):
    kernel_1d = np.linspace(-(kernel_n // 2), kernel_n // 2, kernel_n)
    for i in range(kernel_n):
        kernel_1d[i] = Gauss_func(kernel_1d[i], 0, sigma)
    kernel_2d = np.outer(kernel_1d.T, kernel_1d.T)

    kernel_2d *= 1.0 / kernel_2d.max()

    return kernel_2d

def kernel_index(n):
    '''
    Instead of "range(3) -> 0, 1, 2"
    makes "range(3) -> -1, 0, 1"
    '''
    if n % 2 == 0:
        raise ValueError("Kernel matrix size must be odd number!")
    half = n//2
    for i in range(n):
        yield i-half

def get_custom_Gauss_blur(g_img, kernel_n=7, sigma=6):
    new_img = np.zeros(g_img.shape)
    kernel = Gauss_kernel(kernel_n, sigma)

    for i in range(g_img.shape[0]):
        for j in range(g_img.shape[1]):
            tmp_kernel = np.zeros(kernel.shape)
            gen_kern_ind_x = kernel_index(kernel_n) #create gen which produces -n, -n+1,... 0,... n-1, n
            for k in range(kernel_n):
                x = next(gen_kern_ind_x) #bias for x coord
                gen_kern_ind_y = kernel_index(kernel_n) #create gen which produces -n, -n+1,... 0,... n-1, n
                for m in range(kernel_n):
                    y = next(gen_kern_ind_y) #bias for y coord
                    try:
                        tmp_kernel[k,m] = kernel[k,m]*g_img[i+x,j+y]
                    except: #if it is a border pixel, fill appropriate tmp_kernel cell with 0
                        tmp_kernel[k,m] = 0
            new_img[i,j] = np.sum(tmp_kernel)/(kernel_n**2)
    
    return np.uint8(new_img)

def main(save_path, image_path, kernel_size, sigma):
    kernel_size = int(kernel_size)
    if kernel_size > 7:
        print(">>> WARNING! It will took some time to apply Custom Gauss filter!")
    sigma = int(sigma)

    if kernel_size%2==0:
        raise ValueError("Kernel matrix size must be odd number!")

    if save_path != '0':
        p_save = path.Path(save_path)
        if p_save.is_dir():
            save_path = create_save_folder(save_folder=save_path)
        else:
            raise ValueError("Incorrect path to save folder!")
    else:
        save_path = create_save_folder()
    
    if image_path != '0':
        p_img = path.Path(image_path)
        if p_img.is_file():
            gray_img = get_gray_img(image_path)
        else:
            raise ValueError("Incorrect path to image!")
    else:
        gray_img = get_gray_img(IMG_DEFAULT)
    
    gray_img_lib_blur = get_lib_Gauss_blur(gray_img, kernel_n=kernel_size, sigma=sigma)
    img_lib = Image.fromarray(gray_img_lib_blur)
    timestamp = str(dt.timestamp(dt.now())).replace('.', '')
    img_lib.save(save_path+"/"+"img_blur_lib_"+timestamp+".jpg")
    print(">>> lib result file was successfully saved!")

    gray_img_custom_blur = get_custom_Gauss_blur(gray_img, kernel_n=kernel_size, sigma=sigma)
    img_custom = Image.fromarray(gray_img_custom_blur)
    timestamp = str(dt.timestamp(dt.now())).replace('.', '')
    img_custom.save(save_path+"/"+"img_blur_custom_"+timestamp+".jpg")
    print(">>> custom result file was successfully saved!")

if __name__ == "__main__":
    print("\n\t>>> Custom Gauss Blur <<<\n")
    save_path = input("Enter absolute or relative path to save results (to use default press 0) >>>\t")
    while True:
        image_path = input("Enter absolute or relative path to file to convert (to use default press 0) >>>\t")
        kernel_size = input("Enter kernel matrix size (must be odd) >>>\t")
        sigma = input("Enter sigma >>>\t")
        main(save_path, image_path, kernel_size, sigma)
        stop = input("\nPress Enter to continue. To exit press 0 >>>\t")
        if stop == '0':
            print("\nProgram terminated...")
            break
