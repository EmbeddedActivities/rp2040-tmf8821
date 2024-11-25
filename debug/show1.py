import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# 示例3x3数据
distance_data = np.array([
    [1.1, 1.2, 1.3],
    [1.0, 1.1, 1.2],
    [0.9, 1.0, 1.1]
])

# 假设对应的x, y坐标
x_coords = np.array([
    [-1, 0, 1],
    [-1, 0, 1],
    [-1, 0, 1]
])

y_coords = np.array([
    [1, 1, 1],
    [0, 0, 0],
    [-1, -1, -1]
])

# 转换为点云
points = np.stack((x_coords.flatten(), y_coords.flatten(), distance_data.flatten()), axis=1)

# 拟合平面
M = np.c_[points[:, 0], points[:, 1], np.ones(points.shape[0])]
Z = points[:, 2]
coeffs, _, _, _ = np.linalg.lstsq(M, Z, rcond=None)
A, B, D = coeffs
C = -1  # 假设z是负方向

# 创建网格，用于绘制拟合平面
x = np.linspace(-1.5, 1.5, 10)
y = np.linspace(-1.5, 1.5, 10)
x_grid, y_grid = np.meshgrid(x, y)
z_grid = A * x_grid + B * y_grid + D

# 创建3D图
fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')

# 绘制原始点云
ax.scatter(points[:, 0], points[:, 1], points[:, 2], c='r', label='点云', s=50)

# 绘制拟合平面
ax.plot_surface(x_grid, y_grid, z_grid, alpha=0.5, color='blue', label='拟合平面')

# 设置图形外观
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")
ax.set_title("点云与拟合平面")
ax.legend()

plt.show()

