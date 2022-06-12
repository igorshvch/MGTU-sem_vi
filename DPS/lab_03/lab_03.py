import pathlib as path
from datetime import datetime as dt

from PIL import Image
import cv2
import numpy as np
import matplotlib.pyplot as plt

IMG_DEFAULT = {
    "1": "../static/captcha2.jpg",
    "2": "../static/captcha2.jpg",
    "3": "../static/captcha2.jpg",
    "4": "../static/captcha2.jpg",
    "5": "../static/captcha2.jpg",
    "6": "../static/cowboy.jpg"
}
SAVE_FOLDER = "./save"

def create_save_folder(save_folder=SAVE_FOLDER):
    '''
    Creates subfolder in a current working directory
    '''
    p = path.Path(save_folder)
    p.mkdir(parents=True, exist_ok=True)
    return str(p.absolute())

def get_gray_img(source=IMG_DEFAULT):
    img = cv2.imread(source)
    g_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return g_img

def get_lib_Otsu_bin(g_img):
    _, th = cv2.threshold(g_img, 0, 255, cv2.THRESH_OTSU)
    return th

def get_kernel(option=1, size=3):
    options = {
        "1": lambda size: cv2.getStructuringElement(cv2.MORPH_RECT,(size,size)),
        "2": lambda size: cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(size,size)),
        "3": lambda size: cv2.getStructuringElement(cv2.MORPH_CROSS,(size,size))
    }
    return options[option](size)

def dilation(bin_image, kernel, iters):
    return cv2.dilate(bin_image, kernel, iterations=iters)

def erosion(bin_image, kernel, iters):
    return cv2.erode(bin_image, kernel, iterations=iters)

def opening(bin_image, kernel, iters):
    return cv2.dilate(
        cv2.erode(bin_image, kernel, iterations=iters),
            kernel,
            iterations = iters
    )

def closing(bin_image, kernel, iters):
    return cv2.erode(
        cv2.dilate(bin_image, kernel, iterations=iters),
        kernel,
        iterations = iters
    )

def cond_dilat(bin_image, kernel, iters = 100):
    temp_img = cv2.erode(bin_image, kernel, iterations=3)
    for i in range(iters):
        previous = temp_img
        temp_img = cv2.dilate(temp_img, kernel, iterations=1)
        result = np.minimum(bin_image, temp_img)
        if np.array_equal(result, previous):
            print(">>> Condition dilataion ended after %d itearations", i)
            return result
    return result

def sceletonization(bin_image, kernel):
    skeleton = np.zeros(bin_image.shape, np.uint8)
    while True:
        erode_img = cv2.erode(bin_image, kernel)
        dilate_img = cv2.dilate(erode_img, kernel)
        subtract = cv2.subtract(bin_image, dilate_img)
        skeleton = cv2.bitwise_or(skeleton, subtract)
        bin_image = erode_img.copy()
        if cv2.countNonZero(bin_image) == 0:
            break
    return skeleton


def main(save_path, image_path, proc_opt, kernel_opt, kernel_size, iters):
    proc_options = {
        "1": lambda x, y, z: dilation(x, y, z),
        "2": lambda x, y, z: erosion(x, y, z),
        "3": lambda x, y, z: opening(x, y, z),
        "4": lambda x, y, z: closing(x, y, z),
        "5": lambda x, y, z: cond_dilat(x, y),
        "6": lambda x, y, z: sceletonization(x, y)
    }
    proc_options_str = {
        "1": "dilation",
        "2": "erosion",
        "3": "opening",
        "4": "closing",
        "5": "cond_dilat",
        "6": "sceletonization"
    }
    kernel_options_str = {
        "0": "rect",
        "1": "rect",
        "2": "elps",
        "3": "cross",
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
            gray_img = get_gray_img(image_path)
            bin_image = get_lib_Otsu_bin(gray_img)
        else:
            raise ValueError("Incorrect path to image!")
    else:
        p_img = IMG_DEFAULT[proc_opt]
        gray_img = get_gray_img(p_img)
        bin_image = get_lib_Otsu_bin(gray_img)
    
    #proc_opt check
    if int(proc_opt) not in list(range(1,7)):
        raise ValueError("Incorrect processing option!")
    else:
        proc_func = proc_options[proc_opt]
    
    #kernel
    if int(kernel_opt) not in list(range(1,4)) and kernel_opt != "0":
        raise ValueError("Incorrect kernel option!")
    else:
        if kernel_size == "0":
                kernel_size = 3
        if kernel_opt == "0":
            kernel = get_kernel("1", int(kernel_size))
        else:
            kernel = get_kernel(kernel_opt, int(kernel_size))
    
    #iters
    if iters == "0":
        iters = 1
    else:
        iters = int(iters)

    timestamp = str(dt.timestamp(dt.now())).split('.')[-1]
    file_name = (
        p_img.split('/')[-1].split(".")[0]
        if "/" in p_img
        else p_img.split("\\")[-1].split(".")[0]
    )
    file_name = "_".join(
        (file_name,
        proc_options_str[proc_opt],
        kernel_options_str[kernel_opt],
        "s"+str(kernel_size),
        "i"+str(iters))
    )

    bin_img_proc = proc_func(bin_image, kernel, iters)
    img = Image.fromarray(bin_img_proc)
    img.save(save_path+"/"+file_name+"_"+timestamp+".jpg")
    #print(">>> lib result file was successfully saved! Threshold: ", final_thresh_lib)

if __name__ == "__main__":
    print("\n\t>>> Math morphology <<<\n")
    save_path = input("Enter absolute or relative path to save results (to use default press 0) >>>\t")
    while True:
        image_path = input("Enter absolute or relative path to file to convert (to use default press 0) >>>\t")
        #proc_opt
        print(">>> Choose processing option (insert line number):")
        print(
            ">>>\t1. Dilatation\n"
            +">>>\t2. Erosion\n"
            +">>>\t3. Opening\n"
            +">>>\t4. Closing\n"
            +">>>\t5. Conditional dilatation\n"
            +">>>\t6. Sceletonization"
        )
        proc_opt = input(">>> ")
        #kernel form
        print(">>> Choose kernel type (insert line number, to use default press 0):")
        print(
            ">>>\t1. Rectangle\n"
            +">>>\t2. Ellipse\n"
            +">>>\t3. Cross"
        )
        kernel_opt = input(">>> ")
        #kernel size
        kernel_size = input(">>> Choose kernel size (to use default press 0): ")
        #iters
        iters = input(">>> Enter number of iterations (to use default press 0) >>>\t")
        #main
        main(save_path, image_path, proc_opt, kernel_opt, kernel_size, iters)
        stop = input("\n>>> Press Enter to continue. To exit press 0 >>>\t")
        if stop == '0':
            print("\nProgram terminated...")
            break
