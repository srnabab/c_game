import os, sys

def generate_code(input_file, output_file_h, output_file_c):
    # 用于存储生成的代码
    include_lines = []
    enum_lines = []
    function_lines = []

    # 读取输入文件内容
    with open(input_file, "r") as f:
        lines = f.readlines()

    include_lines.append("#include \"SDL3/SDL_stdinc.h\"\n")
    include_lines.append("#ifndef G_PATH_COMPARE_H")
    include_lines.append("#define G_PATH_COMPARE_H 1\n")
    include_lines.append("typedef enum _PathType\n{")

    # 遍历每一行生成对应的代码
    for i, line in enumerate(lines):
        line = line.strip()
        if not line:
            continue  # 跳过空行
        # 提取方括号内容和 type 值
        if line.startswith("[") and "]" in line:
            start = line.find("[") + 1
            end = line.find("]")
            keyword = line[start:end]

            enum_lines.append(f'    {keyword},')
            
            # 生成代码
            if i == 0:
                function_lines.append(f'    if ((SDL_strcmp(buffer, "[{keyword}]") == 0))')
            else:
                function_lines.append(f'    else if ((SDL_strcmp(buffer, "[{keyword}]") == 0))')
            function_lines.append("    {")
            function_lines.append(f'        return {keyword};')
            function_lines.append("    }")

    with open(output_file_h, "w") as f:
        for line in include_lines:
            f.write(line + "\n")
        f.write("    None,\n")
        for line in enum_lines:
            f.write(line + "\n")
        f.write("    MAX_PATH_TYPE = 0x7FFFFFFF\n")
        f.write("} PathType;\n")
        f.write("\nextern PathType SDLCALL pathCompare(char * buffer);\n\n")
        f.write("#endif")

    # 保存生成的代码到输出文件
    with open(output_file_c, "w") as f:
        f.write("#include \"G_file/path_compare.h\"\n\n")
        f.write("PathType pathCompare(char * buffer)\n{\n")
        for line in function_lines:
            f.write(line + "\n")
        f.write("\n    return None;\n")
        f.write("}")

    print(f"Code has been generated and saved to {output_file_h}. {output_file_c}.")

def write_if_changed(filepath, newfilepath):
    existing_content = ""
    new_content = ""
    with open(filepath, "r") as f:
        existing_content = f.read()
    with open(newfilepath, "r") as f:
        new_content = f.read()
    if existing_content == new_content:
        print(f"{filepath} unchanged, skipping.")
        return
    with open(filepath, "w") as f:
        f.write(new_content)
    print(f"{filepath} updated.")

def create_new_file(filepath):
    with open(filepath, "a"):
        return

# 调用函数


# 输入文件路径
# input_file = "C:/D/code/c_game/run/Path"
try:
    input_file = sys.argv[1]
except:
    print('need Path file')
    exit(-1)

root_index1 = sys.argv[1].rfind('/')
root_index2 = sys.argv[1][:root_index1].rfind('/')
root = sys.argv[1][:root_index2 + 1]

# 输出文件路径
# output_file_h = "C:/D/code/c_game/include/G_file/path_compare.h"
output_file_h = root + "include/G_file/path_compare.h"
# output_file_c = "C:/D/code/c_game/src/G_file/path_compare.c"
output_file_c = root + "src/G_file/path_compare.c"

# output_file_h_temp = "C:/D/code/c_game/include/G_file/path_compare_temp.h"
output_file_h_temp = root + "include/G_file/path_compare_temp.h"
# output_file_c_temp = "C:/D/code/c_game/src/G_file/path_compare_temp.c"
output_file_c_temp = root + "src/G_file/path_compare_temp.c"

create_new_file(output_file_h_temp)
create_new_file(output_file_c_temp)
create_new_file(output_file_h)
create_new_file(output_file_c)
generate_code(input_file, output_file_h_temp, output_file_c_temp)
write_if_changed(output_file_c, output_file_c_temp)
write_if_changed(output_file_h, output_file_h_temp)
os.remove(output_file_c_temp)
os.remove(output_file_h_temp)