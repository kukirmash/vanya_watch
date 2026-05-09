#!/bin/bash
folders=("0-1" "1-2" "2-3" "3-4" "4-5" "5-6" "6-7" "7-8" "8-9" "9-0")

for folder in "${folders[@]}"; do
    echo "Конвертирую папку: $folder в формат A8 (Альфа-маска)..."
    
    # Меняем --cf L8 на --cf A8
    python3 LVGLImage.py --ofmt C --cf A8 --compress NONE "$folder"
done
