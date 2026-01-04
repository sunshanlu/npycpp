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


def read_cpp_npz_file(npz_path: str) -> None:
    """
    读取由 C++ 保存的 NPZ 文件并验证其内容

    此函数加载指定路径的 NPZ 文件，遍历其中的所有数组，
    并记录每个数组的名称、形状、数据类型和内容到日志中。

    :arg:
        npz_path (str): 要读取的 NPZ 文件的路径

    :returns:
        None: 此函数不返回任何值，只输出日志信息

    :raises:
        FileNotFoundError: 当指定的文件不存在时
        OSError: 当文件无法访问或读取时
        ValueError: 当文件格式不正确时
    """
    logger.info(f"=== 读取 C++ 保存的 NPZ 文件: {npz_path} ===")

    # 加载 NPZ 文件
    data = np.load(npz_path)

    # 打印所有数组名称
    logger.info("文件中包含的数组:")
    for key in data.files:
        logger.info(f"  数组名: {key}")
        array: np.ndarray = data[key]
        logger.info(f"    形状: {array.shape}")
        logger.info(f"    数据类型: {array.dtype}")
        logger.info(f"    数据内容: {array.reshape(1, -1)}")

    # 关闭文件
    data.close()


def write_npz_for_cpp(res_path: str) -> None:
    """
    创建测试数据并保存为 NPZ 文件供 C++ 读取。

    此函数创建三个不同数据类型和形状的 NumPy 数组：
    - float_array: 2x2 的 float32 数组
    - int_array: 长度为 5 的 int32 数组
    - double_array: 长度为 3 的 float64 数组

    然后将这些数组保存到 'python_saved.npz' 文件中，
    并记录保存的详细信息到日志中。
    :arg
        res_path (str): npz文件的保存目录

    :returns:
        None: 此函数不返回任何值，只输出日志信息

    :raises:
        IOError: 当文件无法写入时
        OSError: 当文件系统操作失败时
    """
    # 创建测试数据
    array1: np.ndarray = np.array([[1.0, 2.0], [3.0, 4.0]], dtype=np.float32)
    array2: np.ndarray = np.array([1, 2, 3, 4, 5], dtype=np.int32)
    array3: np.ndarray = np.array([1.1, 2.2, 3.3], dtype=np.float64)

    # 保存为 NPZ 文件
    np.savez(
        os.path.join(res_path, 'python_saved.npz'),
        float_array=array1,
        int_array=array2,
        double_array=array3
    )

    logger.info("=== Python 已保存 NPZ 文件: python_saved.npz ===")

    # 验证保存的内容
    logger.info("验证 Python 保存的数据:")
    logger.info(
        f"  float_array: shape={array1.shape}, dtype={array1.dtype}, data={array1.reshape(1, -1)}")
    logger.info(
        f"  int_array: shape={array2.shape}, dtype={array2.dtype}, data={array2.reshape(1, -1)}")
    logger.info(
        f"  double_array: shape={array3.shape}, dtype={array3.dtype}, data={array3.reshape(1, -1)}")


def main() -> None:
    """
    主函数，协调 Python 与 C++ 之间的 NPZ 文件交互验证。

    此函数首先尝试读取由 C++ 保存的 'cpp_saved.npz' 文件，
    如果文件不存在则记录错误日志。
    然后调用 write_npz_for_cpp() 函数创建供 C++ 读取的文件。

    :raises:
        任何子函数可能抛出的异常
    """
    parser = argparse.ArgumentParser(description='NPZ 文件读写验证工具')
    parser.add_argument('--npz_path', type=str, default='cpp_saved.npz', help='要读取的NPZ文件路径')
    parser.add_argument('--res_path', type=str, default='.', help='保存NPZ文件的目录路径')

    args = parser.parse_args()

    try:
        read_cpp_npz_file(args.npz_path)
    except FileNotFoundError:
        logger.error(f"未找到 C++ 保存的文件: {args.npz_path}")

    write_npz_for_cpp(args.res_path)


if __name__ == "__main__":
    main()
