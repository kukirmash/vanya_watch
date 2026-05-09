#!/bin/bash
folders=("0-1" "1-2" "2-3" "3-4" "4-5" "5-6" "6-7" "7-8" "8-9" "9-0")

for folder in "${folders[@]}"; do
    echo "Конвертирую папку: $folder в формат A1 (1-bit маска)..."
    
    # Меняем --cf A8 на --cf A1
    python3 LVGLImage.py --ofmt C --cf A1 --compress NONE "$folder"
done
