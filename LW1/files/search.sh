#!/bin/bash

# Проверяем количество аргументов командной строки
if [ $# -ne 3 ]; then
    echo "Использование: $0 <имя_файла> <каталог> <расширение>"
    exit 1
fi

# Имя файла, каталога и расширение
output_file="$1"
directory="$2"
extension="$3"

# Проверяем, существует ли указанный каталог
if [ ! -d "$directory" ]; then
    echo "Указанный каталог не существует."
    exit 1
fi

# Переходим в указанный каталог
cd "$directory" || exit

# Проверяем, есть ли файлы с указанным расширением в каталоге
files=$(find . -maxdepth 1 -type f -name "*.$extension")

# Проверяем, найдены ли файлы
if [ -z "$files" ]; then
    echo "Файлов с расширением .$extension не найдено в каталоге $directory."
    exit 0
fi

# Записываем имена файлов в указанный файл
echo "Файлы с расширением .$extension в каталоге $directory:" > "$output_file"
echo "$files" >> "$output_file"

echo "Имена файлов были записаны в файл $output_file."
