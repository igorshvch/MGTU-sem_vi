import pathlib as path
from datetime import datetime as dt

from PIL import Image
import cv2
import numpy as np
import matplotlib.pyplot as plt

IMG_DEFAULT = "../static/bp.jpg"
SAVE_FOLDER = "./save"

def make_plot(hist, folder, timestamp):
    x = list(range(len(hist)))
    y = hist
    plt.bar(x,y,align='center', width=0.8) # A bar chart
    plt.xlabel('grayscale')
    plt.ylabel('Num of pixels')
    plt.savefig(folder+"/"+"HIST_custom_bin1_"+timestamp+".png", bbox_inches='tight')

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
    ret, th = cv2.threshold(g_img, 0, 255, cv2.THRESH_OTSU)
    return ret, th

def binary_otsu(g_img):
    pixel_number = g_img.shape[0] * g_img.shape[1]
    mean_weigth = 1.0/pixel_number
    his, bins = np.histogram(g_img, np.array(range(0, 256)))
    final_thresh = -1
    final_variance = -1
    bins = bins[:-1]
    for t in range(1, 254):
        Wb = np.sum(his[:t]) * mean_weigth #backgound pixels
        Wf = np.sum(his[t:]) * mean_weigth #foreground pixels

        mub = np.sum(bins[:t]*his[:t])/ np.sum(his[:t]) #mean intencity of backgound
        muf = np.sum(bins[t:]*his[t:])/ np.sum(his[t:]) #mean intencity of foreground

        variance = Wb * Wf * (mub - muf) ** 2 #class variance

        if variance > final_variance:
            final_thresh = t
            final_variance = variance
    final_img = g_img.copy()
    #print(final_thresh)
    final_img[g_img > final_thresh] = 255
    final_img[g_img < final_thresh] = 0
    return final_thresh, final_img

def binary_bradley(g_img):
    
    h, w = g_img.shape

    S = w/8
    s2 = S/2
    T = 15.0

    #integral img
    int_img = np.zeros_like(g_img, dtype=np.uint32)
    for col in range(1, w):
        for row in range(1, h):
            int_img[row,col] = g_img[0:row,0:col].sum()

    #output img
    out_img = np.zeros_like(g_img)

    for col in range(w):
        for row in range(h):
            #SxS region
            y0 = int(max(row-s2, 0))
            y1 = int(min(row+s2, h-1))
            x0 = int(max(col-s2, 0))
            x1 = int(min(col+s2, w-1))

            count = (y1-y0)*(x1-x0)

            sum_ = int_img[y1, x1]-int_img[y0, x1]-int_img[y1, x0]+int_img[y0, x0]

            if g_img[row, col]*count < sum_*(100.-T)/100.:
                out_img[row,col] = 0
            else:
                out_img[row,col] = 255

    return out_img


def main(save_path, image_path):
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
    
    timestamp = str(dt.timestamp(dt.now())).replace('.', '')

    final_thresh_lib, gray_img_lib_bin = get_lib_Otsu_bin(gray_img)
    img_lib = Image.fromarray(gray_img_lib_bin)
    img_lib.save(save_path+"/"+"img_lib_bin_"+timestamp+".jpg")
    print(">>> lib result file was successfully saved! Threshold: ", final_thresh_lib)

    print("\n\t>>> WARNING! Custom binarization functions evaluation may take 1-2 minutes! <<<\n")

    final_thresh_otsu, gray_img_custom_bin_1 = binary_otsu(gray_img)
    img_custom = Image.fromarray(gray_img_custom_bin_1)
    img_custom.save(save_path+"/"+"img_custom_bin1_"+timestamp+".jpg")
    make_plot(np.histogram(gray_img, np.array(range(0, 256)))[0], save_path, timestamp)
    print(">>> custom Otsu alg result file was successfully saved! Threshold: ", final_thresh_otsu)

    gray_img_custom_bin_2 = binary_bradley(gray_img)
    img_custom = Image.fromarray(gray_img_custom_bin_2)
    img_custom.save(save_path+"/"+"img_custom_bin2_"+timestamp+".jpg")
    print(">>> custom Bradley adaptive thresholding alg result file was successfully saved!")

if __name__ == "__main__":
    print("\n\t>>> Custom Gauss Blur <<<\n")
    save_path = input("Enter absolute or relative path to save results (to use default press 0) >>>\t")
    while True:
        image_path = input("Enter absolute or relative path to file to convert (to use default press 0) >>>\t")
        main(save_path, image_path)
        stop = input("\nPress Enter to continue. To exit press 0 >>>\t")
        if stop == '0':
            print("\nProgram terminated...")
            break
