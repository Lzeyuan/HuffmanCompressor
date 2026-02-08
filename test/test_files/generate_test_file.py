import random
import string
import sys

def generate_readable_file(filename, size_bytes=1):    
    # 定义可打印字符集
    # 方法1: 字母和数字
    chars = string.ascii_letters + string.digits
    
    # 方法2: 所有可打印字符（包括标点和空格）
    # chars = string.printable.strip()  # 去掉换行和制表符
    
    # 方法3: 自定义字符集
    # chars = string.ascii_letters + string.digits + " !@#$%^&*()-_=+[]{}|;:,.<>?/~`"
    
    with open(filename, 'w', encoding='utf-8') as f:
        written = 0
        chunk_size = 65536  # 每次写入64KB
        
        while written < size_bytes:
            # 计算本次写入大小
            remaining = size_bytes - written
            current_chunk = min(chunk_size, remaining)
            
            # 生成随机字符串
            random_chars = ''.join(random.choices(chars, k=current_chunk))
            f.write(random_chars)
            written += current_chunk
            
            # 显示进度
            if written % (1024 * 1024) == 0:  # 每1MB显示一次
                mb_written = written / 1024 / 1024
                print(f"\rGenerated {mb_written:.1f}/{size_bytes} MB", end='')
    
    print(f"\nGenerated {filename} ({size_bytes} MB)")

if __name__ == "__main__":
    filename = "compress_testfile.txt"    
    generate_readable_file(filename, 1024)