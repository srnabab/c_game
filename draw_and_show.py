import matplotlib.pyplot as plt
import numpy as np

# 初始化空数组，用于存储点坐标
points = []

# 鼠标点击事件处理函数
def onclick(event):
    # 如果点击发生在有效的图形区域内
    if event.inaxes:
        # 获取鼠标点击的坐标
        x, y = event.xdata, event.ydata
        points.append((x, y))
        # 在点击位置绘制一个点
        plt.plot(x, y, 'bo')  
        plt.draw()  # 更新绘图
        print(f"Captured point: ({x:.2f}, {y:.2f})")

# 完成绘制的事件（按键按下）
def on_key(event):
    if event.key == 'enter':
        plt.close()  # 关闭绘图窗口

# 创建图形和坐标轴
fig, ax = plt.subplots()
ax.set_xlim([-10, 10])  # X轴范围
ax.set_ylim([-10, 10])  # Y轴范围
ax.set_title('Use mouse to draw points, press Enter when finished')

# 连接鼠标点击事件和按键事件
cid = fig.canvas.mpl_connect('button_press_event', onclick)
kid = fig.canvas.mpl_connect('key_press_event', on_key)

# 显示绘图窗口
plt.show()

# 绘制完成后的点坐标
print("All captured points:", points)

# 生成指定数量的点，可以通过插值计算
num_points = 100  # 例如生成100个点
points = np.array(points)
t = np.linspace(0, 1, num_points)
x_interp = np.interp(t, np.linspace(0, 1, len(points)), points[:, 0])
y_interp = np.interp(t, np.linspace(0, 1, len(points)), points[:, 1])
generated_points = np.column_stack((x_interp, y_interp))

# 打印生成的点坐标
print("Generated points:", generated_points)

import numpy as np
from scipy.optimize import least_squares
import matplotlib.pyplot as plt

# 填写闭合曲线的坐标点 (x, y)
data = generated_points

# 参数 t 均匀分布在 [0, 2π) 上
t = np.linspace(0, 2*np.pi, len(data), endpoint=False)

# 定义傅里叶级数模型
def fourier_series(t, coeffs, N):
    # x(t) = a0 + sum_{n=1}^N [a_n cos(n t) + b_n sin(n t)]
    # y(t) = c0 + sum_{n=1}^N [c_n cos(n t) + d_n sin(n t)]
    
    # 提取 x 的系数
    a0 = coeffs[0]
    x = a0 * np.ones_like(t)
    for n in range(1, N+1):
        a_n = coeffs[2*n -1]
        b_n = coeffs[2*n]
        x += a_n * np.cos(n * t) + b_n * np.sin(n * t)
    
    # 提取 y 的系数
    c0 = coeffs[2*N +1]
    y = c0 * np.ones_like(t)
    for n in range(1, N+1):
        c_n = coeffs[2*N +1 + 2*n -1]
        d_n = coeffs[2*N +1 + 2*n]
        y += c_n * np.cos(n * t) + d_n * np.sin(n * t)
    
    return x, y

# 定义误差函数
def error_function(coeffs):
    x_model, y_model = fourier_series(t, coeffs, N=5)
    return np.concatenate([x_model - data[:,0], y_model - data[:,1]])

# 初始猜测
# 对于 N=5，每个 x 和 y 分别有 11 个系数（包括常数项），总共 22 个系数
N = 5
initial_coeffs = np.zeros(2 + 2*N + 2*N)  # 2*(1 + 2*N)

# 最小化误差
result = least_squares(error_function, initial_coeffs, method='lm')  # 方法选择 'lm' 适合小型问题

# 检查优化是否成功
if result.success:
    print("拟合成功！")
else:
    print("拟合失败：", result.message)

# 获取拟合后的系数
fitted_coeffs = result.x

# 打印拟合后的系数
print("拟合后的系数：", fitted_coeffs)

# 假设这是拟合后的傅里叶系数
coeffs = fitted_coeffs
N = 5 # N为谐波数

def print_fourier_series(coeffs, N):
    terms = []
    
    # 打印常数项 a_0
    if coeffs[0] != 0:
        terms.append(f"{coeffs[0]:.2f}")  # a_0
    
    # 打印余弦和正弦项
    for n in range(1, N + 1):
        a_n = coeffs[2*n-1]  # a_n
        b_n = coeffs[2*n]    # b_n
        
        if a_n != 0:
            terms.append(f"{a_n:.2f} * cos({n} * t)")
        if b_n != 0:
            terms.append(f"{b_n:.2f} * sin({n} * t)")
    
    # 连接所有项
    fourier_series = " + ".join(terms)
    print("Fourier Series:")
    print(f"x(t) = {fourier_series}")


    terms2 = []
    if coeffs[2*N +1] != 0:
        terms2.append(f"{coeffs[2*N +1]:.2f}")
    
    for n in range(1, N + 1):
        c_n = coeffs[2*N +1 + 2*n -1]
        d_n = coeffs[2*N +1 + 2*n]
        
        if c_n != 0:
            terms2.append(f"{c_n:.2f} * cos({n} * t)")
        if d_n != 0:
            terms2.append(f"{d_n:.2f} * sin({n} * t)")

    # 连接所有项
    fourier_series = " + ".join(terms2)
    print("Fourier Series:")
    print(f"y(t) = {fourier_series}")



# 可视化拟合结果
x_fit, y_fit = fourier_series(t, fitted_coeffs, N=5)

# 调用函数打印傅里叶级数
print_fourier_series(coeffs, N)

plt.figure(figsize=(8, 8))
plt.plot(data[:,0], data[:,1], 'bo', label='orignal points')
plt.plot(x_fit, y_fit, 'r-', label='fourier line')
plt.legend()
plt.axis('equal')
plt.title('line')
plt.xlabel('x')
plt.ylabel('y')
plt.show()
