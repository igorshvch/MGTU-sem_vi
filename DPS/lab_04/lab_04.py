import pathlib as path
from datetime import datetime as dt

from PIL import Image
import cv2
import numpy as np
from scipy import ndimage

IMG_DEFAULT = {
    "1": "../static/bl2049_small.jpg",
    "2": "../static/bird_small.jpg"
}
SAVE_FOLDER = "./save"

def create_save_folder(save_folder=SAVE_FOLDER):
    '''
    Creates subfolder in a current working directory
    '''
    p = path.Path(save_folder)
    p.mkdir(parents=True, exist_ok=True)
    return str(p.absolute())

def get_gray_img(source):
    img = cv2.imread(source)
    g_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return g_img

def get_bin_img(g_img):
    _, b_img = cv2.threshold(g_img, 0, 255, cv2.THRESH_OTSU)
    return b_img

def edge_detector(b_img, mode):
    if mode == "C":
        return cv2.Canny(b_img, 50, 150)
    if mode == "R":
        conv_x = np.array( [[1, 0],
                            [0,-1]] )
        
        conv_y = np.array( [[0, 1],
                            [-1, 0]] )
    elif mode=="P":
        conv_x = np.array( [[1, 0, -1],
                            [1, 0, -1],
                            [1, 0, -1]] )
    
        conv_y = np.array([[1, 1, 1],
                           [0, 0, 0],
                           [-1, -1, -1]])
    elif mode=="S":
        conv_x = np.array([[1, 0, -1],
                           [2, 0, -2],
                           [1, 0, -1]])
    
        conv_y = np.array([[1, 2, 1],
                           [0, 0, 0],
                           [-1, -2, -1]])
    elif mode=="H":
        conv_x = np.array([[3, 0, -3],
                           [10, 0, -10],
                           [3, 0, -3]])
    
        conv_y = np.array([[3, 10, 3],
                           [0, 0, 0],
                           [-3, -10, -3]])
    elif mode=="L":
        conv_x = np.array([[1, 1, 1],
                           [1, -8, 1],
                           [1, 1, 1]])
    
        conv_y = np.array([[1, 1, 1],
                           [1, -8, 1],
                           [1, 1, 1]])
        b_img = cv2.GaussianBlur(b_img, (3, 3), 0)
    else:
        raise ValueError("Incorrect mode argument!")

    
    img = b_img.astype('float64')
    vertical = ndimage.convolve(img, conv_x)
    horizontal = ndimage.convolve(img, conv_y)
    
    edged_img = np.sqrt(np.square(horizontal) + np.square(vertical))
    return edged_img.astype('uint8')

def main(save_path, image_path, mode, grayscale):
    options = {
        "1": ("C", "Canny"),
        "2": ("R", "Roberts"),
        "3": ("P", "Prewitt"),
        "4": ("S", "Sobel"),
        "5": ("H", "Scharr"),
        "6": ("L", "Laplace")
    }

    #save path check
    if save_path != '0':
        p_save = path.Path(save_path)
        if p_save.is_dir():
            save_path = create_save_folder(save_folder=save_path)
        else:
            raise ValueError("Incorrect path to save folder!")
    else:
        save_path = create_save_folder()
    
    #img path check
    if image_path != '0':
        p_img = path.Path(image_path)
        if p_img.is_file():
            p_img = str(p_img)
            gray_image = get_gray_img(p_img)
        else:
            raise ValueError("Incorrect path to image!")
    else:
        p_img = IMG_DEFAULT["1"]
        gray_image = get_gray_img(p_img)
    
    #mode check
    if mode == '0':
        mode = "C"    
    #grayscale chech
    if grayscale == "0":
        grayscale_str = ""
        bin_image = get_bin_img(gray_image)
    else:
        grayscale_str = "grayscale"
        bin_image = gray_image
    
    timestamp = str(dt.timestamp(dt.now())).split('.')[-1]
    #create file_name string
    file_name = (
        p_img.split('/')[-1].split(".")[0]
        if "/" in p_img
        else p_img.split("\\")[-1].split(".")[0]
    )
    file_name = "_".join(
        (file_name,
        grayscale_str,
        options[mode][1])
    )

    bin_img_proc = edge_detector(bin_image, options[mode][0])
    img = Image.fromarray(bin_img_proc)
    img.save(save_path+"/"+file_name+"_"+timestamp+".jpg")

if __name__ == "__main__":
    print("\n\t>>> Math morphology <<<\n")
    save_path = input("Enter absolute or relative path to save results (to use default press 0) >>>\t")
    while True:
        image_path = input("Enter absolute or relative path to file to convert (to use default press 0) >>>\t")
        #proc_opt
        print(">>> Choose border detector (insert line number or press 0 to use default):")
        print(
            ">>>\t1. Canny\n"
            +">>>\t2. Roberts\n"
            +">>>\t3. Prewitt\n"
            +">>>\t4. Sobel\n"
            +">>>\t5. Scharr\n"
            +">>>\t6. Laplace"
        )
        mode = input(">>> ")
        grayscale = input("Use grayscale mode?[0-No] >>>\t")
        #main
        main(save_path, image_path, mode, grayscale)
        stop = input("\nPress Enter to continue. To exit press 0 >>>\t")
        if stop == '0':
            print("\nProgram terminated...")
            break
