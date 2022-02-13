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
        Scrambler.__init__(self)
        if lang=="latin":
            alph = (
                [chr(i) for i in range(65, 91)]
                + [chr(i) for i in range(97, 123)]
                + ['.', ',', '!', ':', '?', '-', '(', ')', '"', "'", ' ']
            )
        elif lang=="rus":
            alph = (
                [chr(i) for i in range(1040, 1046)]
                + ['Ё']
                + [chr(i) for i in range(1046, 1078)]
                + ['ё'] 
                + [chr(i) for i in range(1078, 1104)]
                + ['.', ',', '!', ':', '?', '-', '(', ')', '"', "'", ' ']
            )
        else:
            raise ValueError("Incorrect argument for lang:", "lang")
        self.__encoder_table = alph
        self.__decoder_table = {alph[i]:i for i in range(0, len(alph))}
        print("Caesars_cipher instance created successfully")

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


if __name__ == "__main__":
    CS_latin = Caesars_cipher("latin")
    CS_rus = Caesars_cipher("rus")
    while True:
        lang = int(input("Выберите алфавит, введя подходящую цифру: 1(latin)/2(русский) >>> "))
        if lang == 1:
            bias_border = 62
        elif lang == 2:
            bias_border = 76
        else:
            raise ValueError("Incorrect language type. Programm ended")
        bias = int(input(f"Введите ключ шифрования (число от 1 до {bias_border} включительно) >>> "))
        if bias>bias_border or bias<1:
            raise ValueError("Incorrect bias value. Programm ended")
        word = input("Введите слово, фразу или предложение с использованием выбранного алфавита в любом регистре и наков препинания >>>\n")
        if lang==1:
            Enigma_Machine = CS_latin
        elif lang == 2:
            Enigma_Machine = CS_rus
        encoded_word = Enigma_Machine.encode(word, bias)
        print(f"\n===> Вы ввели '{word}', сдвиг шифрования равен {bias}")
        print(f"===> Результат шифрования: {encoded_word}")
        print(f"===> Результат дешифровки: {Enigma_Machine.decode(encoded_word, bias)}\n")
        breaker = int(input("Закодировать новое слово? [1(Yes)/2(No)] >>> "))
        if breaker == 1:
            continue
        elif breaker == 2:
            break
        else:
            raise ValueError("Incorrect command. Programm ended")
