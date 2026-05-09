#!/bin/bash
folders=("0-1" "1-2" "2-3" "3-4" "4-5" "5-6" "6-7" "7-8" "8-9" "9-0")

for folder in "${folders[@]}"; do
    echo "Конвертирую папку: $folder в формат L8 (Оттенки серого)..."
    
    # Меняем --cf на L8 и --compress на NONE
    python3 LVGLImage.py --ofmt C --cf L8 --compress NONE "$folder"
done
