#!/bin/bash

# Список папок для обработки
folders=("0-1" "1-2" "2-3" "3-4" "4-5" "5-6" "6-7" "7-8" "8-9" "9-0")

for folder in "${folders[@]}"; do
    echo "Конвертирую папку: $folder со сжатием RLE..."
    
    # Добавлен флаг --compress RLE
    python3 LVGLImage.py --ofmt C --cf RGB565 --compress RLE "$folder"
    
done

echo "Все готово!"
