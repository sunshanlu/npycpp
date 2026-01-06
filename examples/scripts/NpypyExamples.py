import numpy as np
import logging
import os
import argparse

# 配置日志
logging.basicConfig(
  level=logging.INFO,
  format='[%(asctime)s] [%(levelname)s] [%(filename)s:%(lineno)d] %(message)s'
)
logger = logging.getLogger(__name__)


def read_cpp_npy_file(npy_path: str) -> None:
  """
  读取由 C++ 保存的 NPY 文件并验证其内容

  此函数加载指定路径的 NPY 文件，
  并记录数组的形状、数据类型和内容到日志中。

  :arg:
      npy_path (str): 要读取的 NPY 文件的路径

  :returns:
      None: 此函数不返回任何值，只输出日志信息

  :raises:
      FileNotFoundError: 当指定的文件不存在时
      OSError: 当文件无法访问或读取时
      ValueError: 当文件格式不正确时
  """

  # 加载 NPY 文件
  array: np.ndarray = np.load(npy_path)

  # 记录数组信息
  logger.info(f"数组形状: {array.shape}")
  logger.info(f"数组数据类型: {array.dtype}")
  logger.info(f"数组内容: {array.reshape(1, -1)}")
  logger.info(f"数组元素数量: {array.size}")
  logger.info(f"数组字节数: {array.nbytes}")


def write_npy_for_cpp(res_path: str) -> None:
  """
  创建测试数据并保存为 NPY 文件供 C++ 读取。

  此函数创建不同数据类型和形状的 NumPy 数组，
  然后将这些数组保存到不同的 NPY 文件中，
  并记录保存的详细信息到日志中。

  :arg
      res_path (str): npy文件的保存目录

  :returns:
      None: 此函数不返回任何值，只输出日志信息

  :raises:
      IOError: 当文件无法写入时
      OSError: 当文件系统操作失败时
  """
  # 创建不同类型的测试数据
  float_array = np.array([[1.0, 2.0], [3.0, 4.0]], dtype=np.float32)
  int_array = np.array([1, 2, 3, 4, 5], dtype=np.int32)
  double_array = np.array([1.1, 2.2, 3.3], dtype=np.float64)

  # 保存为 NPY 文件
  float_path = os.path.join(res_path, 'float_array.npy')
  int_path = os.path.join(res_path, 'int_array.npy')
  double_path = os.path.join(res_path, 'double_array.npy')

  np.save(float_path, float_array)
  np.save(int_path, int_array)
  np.save(double_path, double_array)

  logger.info("=== [保存 | Python | NPY 文件] ===")

  # 验证保存的内容
  logger.info("验证 Python 保存的数据:")
  logger.info(
    f"  float_array: shape={float_array.shape}, "
    f"dtype={float_array.dtype}, "
    f"data={float_array.reshape(1, -1)}")
  logger.info(
    f"  int_array: shape={int_array.shape}, "
    f"dtype={int_array.dtype}, "
    f"data={int_array.reshape(1, -1)}")
  logger.info(
    f"  double_array: shape={double_array.shape}, "
    f"dtype={double_array.dtype}, "
    f"data={double_array.reshape(1, -1)}")


def main() -> None:
  """
  主函数，协调 Python 与 C++ 之间的 NPY 文件交互验证。
  """
  parser = argparse.ArgumentParser(description='NPY 文件读写验证工具')
  parser.add_argument('--npy_paths', type=str, nargs='+',
                      default=['cpp_saved.npy'], help='要读取的NPY文件路径列表')
  parser.add_argument('--res_path', type=str, default='.',
                      help='保存NPY文件的目录路径')

  args = parser.parse_args()

  # 读取多个NPY文件
  logger.info(f"=== [读取 | C++ | NPY 文件] ===")
  for npy_path in args.npy_paths:
    try:
      read_cpp_npy_file(npy_path)
    except FileNotFoundError:
      logger.error(f"未找到 C++ 保存的文件: {npy_path}")

  write_npy_for_cpp(args.res_path)


if __name__ == "__main__":
  main()
