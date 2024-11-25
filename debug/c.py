import numpy as np

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

# 归一化法向量
normal = np.array([A, B, C])
normal /= np.linalg.norm(normal)

# 计算俯仰和滚动角
pitch = np.arcsin(-normal[1]) * 180 / np.pi  # 转换为角度
roll = np.arctan2(normal[0], normal[2]) * 180 / np.pi

print(f"法向量: {normal}")
print(f"俯仰角: {pitch:.2f}°")
print(f"滚动角: {roll:.2f}°")

