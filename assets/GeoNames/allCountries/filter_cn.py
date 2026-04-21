#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
GeoNames allCountries.txt 中国数据提取脚本
功能：
    • 只保留 country_code == "CN" 的所有记录（省、市、县、村庄、POI 等全部保留）
    • 完全保留原始 allCountries.txt 的格式（每一行字段数量、顺序、Tab 分隔符完全一致）
    • 不修改任何字段内容，不做任何额外处理
    • 输出文件可直接用于 C++ 加载（load 函数无需任何修改）
"""

import sys
from pathlib import Path

def extract_china_all(input_file: str = "allCountries.txt",
                      output_file: str = "china_all.txt") -> None:
    """
    提取中国全部 GeoNames 记录，保留原始格式
    """
    input_path = Path(input_file)
    output_path = Path(output_file)

    if not input_path.exists():
        print(f"❌ 错误：输入文件 {input_file} 不存在！")
        print("请先下载 allCountries.zip 并解压得到 allCountries.txt")
        print("下载地址：http://download.geonames.org/export/dump/allCountries.zip")
        sys.exit(1)

    count = 0
    total_lines = 0
    print("🚀 开始提取中国全部记录（保留完整原始格式）...")

    with open(input_path, "r", encoding="utf-8") as fin, \
         open(output_path, "w", encoding="utf-8", newline="\n") as fout:

        for line_num, line in enumerate(fin, 1):
            total_lines += 1
            original_line = line.rstrip("\n\r")   # 保留原始内容

            if not original_line or original_line.startswith("#"):
                # 空行或注释行直接跳过（GeoNames 数据文件中极少）
                continue

            fields = original_line.split("\t")

            # 关键判断：第 9 列（索引 8）是 country code
            if len(fields) > 8 and fields[8] == "CN":
                fout.write(original_line + "\n")
                count += 1

            # 每处理 50 万行打印一次进度
            if total_lines % 500000 == 0:
                print(f"  已处理 {total_lines:,} 行，已提取中国记录 {count:,} 条...")

    print(f"\n✅ 提取完成！")
    print(f"   中国全部 GeoNames 记录总数：{count:,} 条")
    print(f"   输出文件：{output_path.resolve()}")
    print(f"   文件大小约 {output_path.stat().st_size / 1024 / 1024:.1f} MB（远小于原文件）")
    print("\n💡 使用方法（C++）：")
    print('   geocoder.load("china_all.txt")')
    print("   现在你的程序可以直接使用中国完整行政区划 + 所有地点数据！")


def extract_china_no_villages(input_file: str = "china_all.txt",
                              output_file: str = "china_no_ppl.txt") -> None:
    """
    提取中国数据并删除村庄（P 类）记录
    """
    input_path = Path(input_file)
    output_path = Path(output_file)

    if not input_path.exists():
        print(f"❌ 错误：输入文件 {input_file} 不存在！")
        print("请先运行之前的 extract_china_all.py 生成 china_all.txt")
        print("或把 allCountries.txt 改名为 china_all.txt")
        sys.exit(1)

    count = 0
    total_lines = 0
    kept_admin = 0
    kept_other = 0

    print("🚀 开始处理：提取中国数据并删除村庄一级（P 类）...")

    with open(input_path, "r", encoding="utf-8") as fin, \
         open(output_path, "w", encoding="utf-8", newline="\n") as fout:

        for line_num, line in enumerate(fin, 1):
            total_lines += 1
            original_line = line.rstrip("\n\r")

            if not original_line or original_line.startswith("#"):
                continue

            fields = original_line.split("\t")

            if len(fields) <= 8:
                continue

            country = fields[8]
            if country != "CN":
                continue

            # 关键过滤：删除 feature_class == "P"（村庄、城镇、居民点）
            feature_class = fields[6] if len(fields) > 6 else ""

            if feature_class == "P":
                continue  # 跳过所有村庄一级

            # 写入保留的记录
            fout.write(original_line + "\n")
            count += 1

            if feature_class == "A":
                kept_admin += 1
            else:
                kept_other += 1

            if total_lines % 500000 == 0:
                print(f"  已处理 {total_lines:,} 行，已保留 {count:,} 条...")

    print(f"\n✅ 处理完成！")
    print(f"   总处理行数：{total_lines:,}")
    print(f"   最终保留记录：{count:,} 条")
    print(f"     - 行政区划（A 类）：{kept_admin:,} 条（省、市、县等）")
    print(f"     - 其他非村庄记录：{kept_other:,} 条")
    print(f"   输出文件：{output_path.resolve()}")
    print(f"   文件大小约 {output_path.stat().st_size / 1024 / 1024:.1f} MB")
    print("\n💡 使用建议（C++）：")
    print('   geocoder.load("china_no_ppl.txt")')
    print("   现在最近邻搜索将主要落在行政区划上，不再返回小村庄。")



if __name__ == "__main__":
    # 使用示例：
    # if len(sys.argv) >= 3:
    #     extract_china_all(sys.argv[1], sys.argv[2])
    # elif len(sys.argv) == 2:
    #     extract_china_all(sys.argv[1])
    # else:
    #     extract_china_all()
    extract_china_no_villages()
    