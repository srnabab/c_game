import numpy as np
from scipy.optimize import least_squares
import matplotlib.pyplot as plt

# 填写闭合曲线的坐标点 (x, y)
data = np.array([
    [1.0, 2.0],
    [2.5, 3.0],
    [3.0, 1.5],
    [1.5, 1.0],
    [0.5, 0.8],
    [0.0, 1.0],
    [-0.5, 0.8],
    [-1.0, 1.0],
    [-0.5, 1.2],
    [0.0, 1.5],
    [0.5, 1.2],
    [1.0, 1.0],
    [1.5, 1.0],
    [2.0, 1.2],
    [2.5, 1.5],
    [3.0, 2.0],
    [2.5, 2.5],
    [2.0, 2.5],
    [1.5, 2.2],
    [1.0, 2.0]
])

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

# 可视化拟合结果
x_fit, y_fit = fourier_series(t, fitted_coeffs, N=5)

plt.figure(figsize=(8, 8))
plt.plot(data[:,0], data[:,1], 'bo', label='原始数据点')
plt.plot(x_fit, y_fit, 'r-', label='傅里叶拟合曲线')
plt.legend()
plt.axis('equal')
plt.title('傅里叶级数拟合闭合曲线')
plt.xlabel('x')
plt.ylabel('y')
plt.show()
