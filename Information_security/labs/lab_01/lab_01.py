from string import punctuation, ascii_uppercase, ascii_lowercase

ENGLISH_LETTERS = list(ascii_uppercase) + list(ascii_lowercase)
PUNCTUATION = list(punctuation + ' ')

class Scrambler():
    def __init__(self):
        return 0
    def __scramble(self):
        '''
        Internal enode or decode methode
        '''
        raise NotImplementedError("!")
    def __process_word(self):
        '''
        Internal word processor
        '''
        raise NotImplementedError("!")
    def encode(self):
        '''
        Public method for encoding
        '''
        raise NotImplementedError("!")
    def decode(self):
        '''
        Public method for decoding
        '''
        raise NotImplementedError("!")
    def show_internal_representation(self):
        '''
        Public method for pretty printing of internal data:
        '''
        raise NotImplementedError("!")

class Caesars_cipher(Scrambler):
    '''
    Caesar's cipher implementation
    https://ru.wikipedia.org/wiki/%D0%A8%D0%B8%D1%84%D1%80_%D0%A6%D0%B5%D0%B7%D0%B0%D1%80%D1%8F
    '''
    def __init__(self, lang):
        super().__init__()
        if lang=="latin":
            alph = (
                ENGLISH_LETTERS
                + PUNCTUATION
            )
        elif lang=="rus":
            alph = (
                [chr(i) for i in range(1040, 1046)]
                + ['Ё']
                + [chr(i) for i in range(1046, 1078)]
                + ['ё'] 
                + [chr(i) for i in range(1078, 1104)]
                + PUNCTUATION
            )
        elif lang=="bilingual":
            alph = (
                ENGLISH_LETTERS
                + [chr(i) for i in range(1040, 1046)]
                + ['Ё']
                + [chr(i) for i in range(1046, 1078)]
                + ['ё'] 
                + [chr(i) for i in range(1078, 1104)]
                + PUNCTUATION
            )
        else:
            raise ValueError(f"Incorrect argument for lang:{lang}")
        self.__encoder_table = alph
        self.__decoder_table = {alph[i]:i for i in range(0, len(alph))}
        print(f"Caesars_cipher instance created successfully. Lang: {lang}")

    def __scramble(self, letter, bias, mode="encode"):
        position = self.__decoder_table[letter]
        if mode=="encode":
            position = (position+bias)%len(self.__encoder_table)
        elif mode=="decode":
            position = (position-bias)%len(self.__encoder_table)
        else:
            raise ValueError(f"Incorrect argument for mode: {mode}")
        return self.__encoder_table[position]
    
    def __process_word(self, word, bias, mode):
        crypto = ""
        for letter in word:
            crypto += self.__scramble(letter, bias, mode=mode)
        return crypto
    
    def encode(self, word, bias):
        encrypted = self.__process_word(word, bias, "encode")
        return encrypted
    
    def decode(self, word, bias):
        decrypted = self.__process_word(word, bias, "decode")
        return decrypted


class Vigenere_cipher(Scrambler):
    '''
    Vigenère cipher implementation
    https://ru.wikipedia.org/wiki/%D0%A8%D0%B8%D1%84%D1%80_%D0%92%D0%B8%D0%B6%D0%B5%D0%BD%D0%B5%D1%80%D0%B0
    '''
    def __init__(self, lang):
        super().__init__()
        if lang=="latin":
            alph = (
                ENGLISH_LETTERS
                + PUNCTUATION
            )
        elif lang=="rus":
            alph = (
                [chr(i) for i in range(1040, 1046)]
                + ['Ё']
                + [chr(i) for i in range(1046, 1078)]
                + ['ё'] 
                + [chr(i) for i in range(1078, 1104)]
                + PUNCTUATION
            )
        elif lang=="bilingual":
            alph = (
                ENGLISH_LETTERS
                + [chr(i) for i in range(1040, 1046)]
                + ['Ё']
                + [chr(i) for i in range(1046, 1078)]
                + ['ё'] 
                + [chr(i) for i in range(1078, 1104)]
                + PUNCTUATION
            )
        else:
            raise ValueError(f"Incorrect argument for lang:{lang}")
        self.__encoder_table = alph
        self.__decoder_table = {alph[i]:i for i in range(0, len(alph))}
        print(f"Vigenere_cipher instance created successfully. Lang: {lang}")

    def __scramble(self, letter, key_letter, mode="encode"):
        position = self.__decoder_table[letter]
        bias = self.__decoder_table[key_letter]
        if mode=="encode":
            position = (position+bias)%len(self.__encoder_table)
        elif mode=="decode":
            position = (position-bias)%len(self.__encoder_table)
        else:
            raise ValueError(f"Incorrect argument for mode: {mode}")
        return self.__encoder_table[position]
    
    def __process_word(self, word, key, mode):
        len1 = len(word)
        len2 = len(key)
        norm_key = [key[i] if i<len2 else key[i%len2] for i in range(len1)]
        norm_word = list(word)
        print("{:9s}:".format("Слово"), "\t", " ".join(norm_word))
        print("{:9s}:".format("Ключ"), "\t", " ".join(norm_key))
        crypto = ""
        for (letter, key_letter) in zip(norm_word, norm_key):
            crypto += self.__scramble(letter, key_letter, mode=mode)
        print("{:9s}:".format("Результат"), "\t", " ".join(list(crypto)))
        return crypto
    
    def encode(self, word, key):
        encrypted = self.__process_word(word, key, "encode")
        return encrypted
    
    def decode(self, word, key):
        decrypted = self.__process_word(word, key, "decode")
        return decrypted
    
    def show_internal_representation(self):
        alph = self.__encoder_table
        print("      ", end="")
        for i in range(len(alph)):
            print("{0:2d}".format(i), end="|")
        print()
        print("      ", end="")
        for i in range(len(alph)):
            print("==", end="|")
        print()
        print()
        for i in range(len(alph)):
            print("{0:2d}".format(i), end="||")
            print("  ", end="")
            for letter in (alph[i:]+alph[:i]):
                print(f" {letter}", end='|')
            print()
        print()


if __name__ == "__main__":
    CS_latin = Caesars_cipher("latin")
    CS_rus = Caesars_cipher("rus")
    CS_bilingual = Caesars_cipher("bilingual")
    VC_latin = Vigenere_cipher("latin")
    VC_rus = Vigenere_cipher("rus")
    VC_bilingual = Vigenere_cipher("bilingual")
    print()
    while True:
        processor = int(input("Выберите процессор шифрования, введя пододящую цифру: 1(Цезарь)/2(Виженер) >>> "))
        
        if processor == 1:
            while True:
                lang = int(input("Выберите алфавит, введя подходящую цифру: 1(latin)/2(русский)/3(билингвальный) >>> "))
                if lang == 1:
                    bias_border = 62
                elif lang == 2:
                    bias_border = 76
                elif lang == 3:
                    bias_border = 129
                else:
                    raise ValueError("Incorrect language type. Programm ended")
                word = input("Введите слово, фразу или предложение с использованием выбранного алфавита в любом регистре и знаков препинания >>>\n")
                bias = int(input(f"Введите ключ шифрования (число от 1 до {bias_border} включительно) >>> "))
                if bias>bias_border or bias<1:
                    raise ValueError("Incorrect bias value. Programm ended")
                if lang==1:
                    Enigma_Machine = CS_latin
                elif lang == 2:
                    Enigma_Machine = CS_rus
                elif lang == 3:
                    Enigma_Machine = CS_bilingual
                encoded_word = Enigma_Machine.encode(word, bias)
                print(f"\n===> Вы ввели '{word}', сдвиг шифрования равен {bias}")
                print(f"===> Результат шифрования: {encoded_word}")
                print(f"===> Результат дешифровки: {Enigma_Machine.decode(encoded_word, bias)}\n")
                
                breaker = int(input("Закодировать новое слово используя текущий процессор (Цезарь)? [1(Yes)/2(No)] >>> "))
                if breaker == 1:
                    continue
                elif breaker == 2:
                    break
                else:
                    raise ValueError("Incorrect command. Programm ended")

        elif processor == 2:
            while True:
                lang = int(input("Выберите алфавит, введя подходящую цифру: 1(latin)/2(русский)/3(билингвальный, без вывод Символьной таблицы) >>> "))
                if lang == 3:
                    print("\n\t===== Внимание! Из-за большой длины массива символов Символьная таблица выводиться не будет! =====\n")
                if lang < 1 or lang > 3:
                    raise ValueError("Incorrect language type. Programm ended")
                word = input("Введите слово, фразу или предложение с использованием выбранного алфавита в любом регистре и знаков препинания >>>\n")
                if lang==1:
                    Enigma_Machine = VC_latin
                elif lang == 2:
                    Enigma_Machine = VC_rus
                elif lang == 3:
                    Enigma_Machine = VC_bilingual
                key = input(f"Введите ключ шифрования (любую последовательность из букв в выбранном алфавите и знаков препинания) >>>\n")
                if lang != 3:
                    Enigma_Machine.show_internal_representation()
                print(f"===> Результат шифрования:\n")
                encoded_word = Enigma_Machine.encode(word, key)
                print(f"\n===> Результат дешифровки:\n")
                Enigma_Machine.decode(encoded_word, key)

                breaker = int(input("\nЗакодировать новое слово используя текущий процессор (Виженер)? [1(Yes)/2(No)] >>> "))
                if breaker == 1:
                    continue
                elif breaker == 2:
                    break
                else:
                    raise ValueError("Incorrect command. Programm ended")

        else:
            raise ValueError("Incorrect command. Programm ended")

        breaker = int(input("Выбрать другой процессор? [1(Yes)/2(No)] >>> "))
        if breaker == 1:
            continue
        elif breaker == 2:
            break
        else:
            raise ValueError("Incorrect command. Programm ended")