import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import serial
from matplotlib import font_manager

# 手动指定字体路径（如果需要中文显示）
zh_font = '/System/Library/Fonts/PingFang.ttc'  # 替换为实际字体路径
font_prop = font_manager.FontProperties(fname=zh_font)

# 串口配置
serial_port = '/dev/cu.usbmodem14301'  # 替换为实际串口名称
baud_rate = 115200  # 串口波特率
ser = serial.Serial(serial_port, baud_rate, timeout=1)

# 初始化图形
fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')
plt.ion()  # 打开交互模式

# 假设对应的x, y坐标（3x3）
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

# 实时渲染函数
def render(data):
    # 数据格式化为点云
    points = np.stack((x_coords.flatten(), y_coords.flatten(), data.flatten()), axis=1)

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

    # 清空并重新绘制
    ax.clear()
    ax.scatter(points[:, 0], points[:, 1], points[:, 2], c='r', label='点云', s=50)
    ax.plot_surface(x_grid, y_grid, z_grid, alpha=0.5, color='blue', label='拟合平面')

    # 设置图形外观
    ax.set_xlabel("X轴 (mm)", fontproperties=font_prop)
    ax.set_ylabel("Y轴 (mm)", fontproperties=font_prop)
    ax.set_zlabel("Z轴 (mm)", fontproperties=font_prop)
    ax.set_title("点云与拟合平面", fontproperties=font_prop)
    ax.legend(loc='best', prop=font_prop)


    plt.draw()
    plt.pause(0.001)

# 主循环
try:
    while True:
        line = ser.readline().decode('utf-8').strip()  # 按行读取
        if line:
            try:
                # 将读取的数据转换为int
                data = np.array([int(x) for x in line.split()]).reshape((3, 3))
                render(data)  # 渲染图形
                print(data)  # 打印数据
            except ValueError:
                print(f"无效数据: {line}")
except KeyboardInterrupt:
    print("终止程序")
finally:
    ser.close()
    plt.ioff()
    plt.show()
