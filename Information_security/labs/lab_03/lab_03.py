import random as rd
import pickle as pck

import numpy as np
from PIL import Image

def open_image(src):
    options = {
        'RGB': 3,
        'RGBA': 4
    }
    with Image.open(src, 'r') as im:
        width, height = im.size
        im_array = np.array(list(im.getdata()))
        mode = im.mode
    total_pixels = im_array.size//options[mode]
    return im_array, width, height, mode, total_pixels

def to_bytes(message):
    return ''.join([format(ord(letter), "016b") for letter in message])

def from_bytes(b_string):
    b_letters = [b_string[i:i+16] for i in range(0, len(b_string), 16)]
    letters = [chr(int(b_letter, 2)) for b_letter in b_letters]
    return ''.join(letters)

def save_keys(keys, path='keys.data'):
    with open(path, mode='wb') as fle:
        pck.dump(keys, fle)
    print('Ключи успешно сохранены')

def load_keys(path='keys.data'):
    with open(path, mode='rb') as fle:
        keys = pck.load(fle)
    print('Ключи успешно загружены')
    return keys

def encode(src, message, dest):

    array, width, height, mode, total_pixels = open_image(src)
    b_message = to_bytes(message)
    req_pixels = len(b_message)

    if req_pixels > total_pixels:
        print("Длина сообщения превышает количество пикселей!")
        return -1

    else:
        pixels = rd.sample(range(total_pixels), k=req_pixels)
        keys = []
        index=0
        for p in pixels:
            for c in range(0, 3):
                if index < req_pixels:
                    array[p][c] = int(bin(array[p][c])[2:9] + b_message[index], 2)
                    index += 1
                    keys.append((p, c))


    array = array.reshape(height, width, len(mode))
    enc_im = Image.fromarray(array.astype('uint8'), mode)
    enc_im.save(dest)
    print("Сообщение успешно закодировано")
    save_keys(keys)
    
def decode(src, keys):
    array, _, _, mode, total_pixels = open_image(src)
    hidden_bits = ""
    for p, c in keys:
        hidden_bits += (bin(array[p][c])[2:][-1])
    message = from_bytes(hidden_bits)
    return message

def start_func():
    print("--Стеганография--")
    while True:
        print("1. Кодировать")
        print("2. Декодировать")
        print("3. Выйти")

        func = input(">>> ")

        if func == '1':
            print("Введите путь к изображению")
            src = input(">>> ")
            print("Введите секретное сообщение")
            message = input(">>> ")
            print("Введите путь и наименование файла для сохранения")
            dest = input(">>> ")
            print("Кодирование...")
            encode(src, message, dest)

        elif func == '2':
            keys = load_keys()
            print("Введите путь к картинке с зашифованным сообщением")
            src = input(">>> ")
            print("Декодирование...")
            print(">>>", decode(src, keys))
        
        elif func == '3':
            break

        else:
            print("ERROR: Invalid option chosen")

if __name__ == "__main__":
    start_func()
