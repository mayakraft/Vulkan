rm all_files.txt
touch all_files.txt

find ./engine ./shaders -type f ! \( -name "*.o" -o -name "*.spv" -o -name "*.DS_Store" -o -name "stb_image.h" -o -name "tiny_obj_loader.h" \) | sort | while read -r file; do
    if [ -f "$file" ]; then 
        echo "$file" 
        filename=$(basename -- "$file")
        echo "" >> all_files.txt
        echo "///////////////" >> all_files.txt
        echo "// $filename" >> all_files.txt
        echo "///////////////" >> all_files.txt
        cat "$file" >> all_files.txt
    fi 
done
