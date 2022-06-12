import pathlib as path
from datetime import datetime as dt

from PIL import Image
import cv2
import numpy as np

IMG_DEFAULT = {
    "1": "../static/bl2049_small.jpg",
}
SAVE_FOLDER = "./save"

COLOR = (0, 255, 0)

def create_save_folder(save_folder=SAVE_FOLDER):
    '''
    Creates subfolder in a current working directory
    '''
    p = path.Path(save_folder)
    p.mkdir(parents=True, exist_ok=True)
    return str(p.absolute())

def get_img(source):
    return cv2.cvtColor(cv2.imread(source), cv2.COLOR_BGR2RGB)

def get_gray_img(img):
    g_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return g_img

def moravec_detector(img_gray, img, block_size, threshold):
    r = block_size // 2
    img_gray = np.asarray(img_gray, dtype=np.float32)
    row = img_gray.shape[0]
    col = img_gray.shape[1]

    min_val = np.zeros(img_gray.shape)
    corners = 0

    for y in range(r, row - r):
        for x in range(r, col - r):
            v1 = 0
            v2 = 0
            v3 = 0
            v4 = 0
            for k in range(-r, r):
                v1 += np.square(img_gray[y, x + k] - img_gray[y, x + k + 1])
                v2 += np.square(img_gray[y + k, x] - img_gray[y + k + 1, x])
                v3 += np.square(img_gray[y + k, x + k] - img_gray[y + k + 1, x + k + 1])
                v4 += np.square(img_gray[y + k, x - k] - img_gray[y + k + 1, x - k - 1])

            min_val[y, x] = min(v1, v2, v3, v4)

            if min_val[y, x] < threshold:
                min_val[y, x] = 0
            else:
                corners += 1

    for i in range(block_size, row - block_size):
        for j in range(block_size, col - block_size):
            mat = min_val[i - block_size:i + block_size, j - block_size:j + block_size]
            if np.max(mat) != 0:
                pos = np.unravel_index(np.argmax(mat), mat.shape)
                corn_y = i + pos[0] - block_size
                corn_x = j + pos[1] - block_size
                cv2.circle(img, (corn_x, corn_y), 3, COLOR)

    return corners, img

def harris_detector(img_gray, img, block_size, aperture_size, k):
    img_gray = np.float32(img_gray)

    dest = cv2.cornerHarris(img_gray, block_size, aperture_size, k)
    thresh = 0.01 * dest.max()
    corners = np.sum(dest > thresh)
    dest = cv2.dilate(dest, None)

    for i in range(dest.shape[0]):
        for j in range(dest.shape[1]):
            if int(dest[i, j]) > thresh:
                cv2.circle(img, (j, i), 3, COLOR)

    return corners, img

def fast_detector(img_gray, img, threshold, non_max_suppression):
    fast = cv2.FastFeatureDetector_create(threshold, non_max_suppression)

    corners = fast.detect(img_gray, None)
    img = cv2.drawKeypoints(img, corners, None, color=COLOR)

    return len(corners), img


def main(save_path,
         image_path,
         mode,
         block_size,
         threshold,
         aperture_size=None,
         k=None,
         non_max_suppression=None):
    funcs = {
        "1": lambda x,y: moravec_detector(x, y, block_size, threshold),
        "2": lambda x,y: harris_detector(x, y, block_size, aperture_size, k),
        "3": lambda x,y: fast_detector(x, y, threshold, non_max_suppression),
    }
    options = {
        "1": "Moravec",
        "2": "Harris",
        "3": "Fast",
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
            img = get_img(p_img)
            gray_image = get_gray_img(img)
        else:
            raise ValueError("Incorrect path to image!")
    else:
        p_img = IMG_DEFAULT["1"]
        img = get_img(p_img)
        gray_image = get_gray_img(img)
    
    #mode check
    if mode == "0":
        detector = funcs["3"]
    else:
        detector = funcs[mode]
    
    timestamp = str(dt.timestamp(dt.now())).split('.')[-1]
    #create file_name string
    file_name = (
        p_img.split('/')[-1].split(".")[0]
        if "/" in p_img
        else p_img.split("\\")[-1].split(".")[0]
    )
    file_name = "_".join(
        (file_name,
        options[mode])
    )

    corners, image = detector(gray_image.copy(), img.copy())
    print("Num of detected corners:", corners)
    img = Image.fromarray(image)
    img.save(save_path+"/"+file_name+"_"+str(corners)+"_"+timestamp+".jpg")

if __name__ == "__main__":
    print("\n\t>>> Corner detection <<<\n")
    save_path = input("Enter absolute or relative path to save results (to use default press 0) >>>\t")
    while True:
        image_path = input("Enter absolute or relative path to file to convert (to use default press 0) >>>\t")
        #proc_opt
        print(">>> Choose corner detector (insert line number or press 0 to use default):")
        print(
            ">>>\t1. Moravec\n"
            +">>>\t2. Harris\n"
            +">>>\t3. Fast\n"
        )
        mode = input(">>> ")
        block_size = threshold = aperture_size = k = non_max_suppression = None
        if mode == "1":
            block_size = int(input("\tChoose block size (integer) >>> "))
            threshold = float(input("\tChoose threshold (float) >>> "))
        elif mode == "2":
            block_size = int(input("\tChoose block size (integer) >>> "))
            aperture_size = int(input("\tChoose aperture (integer) >>> "))
            k = float(input("\tChoose k (float) >>> "))
        elif mode == "3":
            threshold = int(input("\tChoose threshold (int) >>> "))
            non_max_suppression = bool(int(input("\tChoose max suppression (bool, 0=False) >>> ")))
        else:
            raise ValueError("Incorrect detector type!")
        #main
        main(save_path, image_path, mode, block_size, threshold, aperture_size, k, non_max_suppression)
        stop = input("\nPress Enter to continue. To exit press 0 >>>\t")
        if stop == '0':
            print("\nProgram terminated...")
            break
