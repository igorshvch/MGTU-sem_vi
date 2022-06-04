class Node:
    '''
    Класс для представления узла дерева
    '''
    def __init__(self, prob, symbol, left=None, right=None):
        self.prob = prob
        self.symbol = symbol
        self.left = left
        self.right = right
        self.code = ''

codes = dict()

def calculate_codes(node, val=''):
    '''
    Расчет кода для текущего узла
    '''
    new_val = val + str(node.code)

    if(node.left):
        calculate_codes(node.left, new_val)
    if(node.right):
        calculate_codes(node.right, new_val)
    if(not node.left and not node.right):
        codes[node.symbol] = new_val
         
    return codes        

def calculate_probability(data):
    '''
    Расчет частоты встречаемости символов
    '''
    symbols = dict()
    for element in data:
        if symbols.get(element) == None:
            symbols[element] = 1
        else: 
            symbols[element] += 1     
    return symbols

def output_encoded(data, coding):
    '''
    Получаем закодированные данные
    '''
    encoding_output = []
    for c in data:
        encoding_output.append(coding[c])
        
    string = ''.join([str(item) for item in encoding_output])    
    return string
        
def total_gain(data, coding):
    '''
    Подсчет выигрыша от сжатия
    '''
    before_compression = len(data) * 16
    after_compression = 0
    symbols = coding.keys()
    for symbol in symbols:
        count = data.count(symbol)
        after_compression += count * len(coding[symbol])
    print("Занимаемая память до сжатия (в битах):", before_compression)    
    print("Занимаемая память после сжатия (в битах):",  after_compression)           

def encode(data):
    symbol_with_probs = calculate_probability(data)
    symbols = symbol_with_probs.keys()
    probabilities = symbol_with_probs.values()
    sym_n_pbobs = sorted(zip(symbols, probabilities), key=lambda x: x[1], reverse=True)
    
    print()
    print("Частота встречаеости символов")
    print("{:^10}|{:^15}".format("символ", "частота"))
    print("-"*26)
    for s, p in sym_n_pbobs:
        print("{:^10s}|{:15d}".format(s, p))
    print("-"*26)

    nodes = []
    
    for symbol in symbols:
        nodes.append(Node(symbol_with_probs.get(symbol), symbol))
    
    while len(nodes) > 1:
        nodes = sorted(nodes, key=lambda x: x.prob)
        right = nodes[0]
        left = nodes[1]
    
        left.code = 0
        right.code = 1
    
        newNode = Node(left.prob+right.prob, left.symbol+right.symbol, left, right)
    
        nodes.remove(left)
        nodes.remove(right)
        nodes.append(newNode)
            
    huffman_encoding = calculate_codes(nodes[0])
    print()
    print("Таблица кодов")
    print("{:^10}|{:^15}".format("символ", "код"))
    print("-"*26)
    for key, _ in sym_n_pbobs:
        print("{:^10s}|{:>15s}".format(key, huffman_encoding[key]))
    print("-"*26)

    total_gain(data, huffman_encoding)
    encoded_output = output_encoded(data, huffman_encoding)
    return encoded_output, nodes[0]  
    
 
def decode(encoded_data, huffman_tree):
    tree_head = huffman_tree
    decoded_output = []
    for x in encoded_data:
        if x == '1':
            huffman_tree = huffman_tree.right   
        elif x == '0':
            huffman_tree = huffman_tree.left
        try:
            if huffman_tree.left.symbol == None and huffman_tree.right.symbol == None:
                pass
        except AttributeError:
            decoded_output.append(huffman_tree.symbol)
            huffman_tree = tree_head
        
    string = ''.join([str(item) for item in decoded_output])
    return string

if __name__ == "__main__":
    while True:
        print("--Код Хаффмана--")
        print("1. Ввести сообщение")
        print("2. Попробовать демо-версию текста")
        print("3. Выход")

        holder = int(input(">>> "))

        if holder == 1:
            print("Введите текст:")
            text = input(">>> ")
            encoding, tree = encode(text)
            print()
            print("Закодированный текст:\n", encoding, sep='')
            print("Декодированый текст:\n", decode(encoding, tree), sep='')
            print()
        elif holder == 2:
            print("В качестве демо-верссии использован текст из романа 'Война и Мир'")
            with open('test.txt', mode='r', encoding='utf-8') as fle:
                text = fle.read()
            encoding, tree = encode(text)
            print()
            print("Закодированный текст:\n{}...".format(encoding[:100]))
            print("Декодированый текст:\n{}...".format(decode(encoding, tree)[:100]))
            print()
        elif holder == 3:
            break
        else:
            print("Неверная команда!")