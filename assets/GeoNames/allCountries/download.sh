#!/bin/bash
# GeoNames 官方下载地址（已验证有效）[citation:8][citation:10]
DATA_URL="http://download.geonames.org/export/dump/allCountries.zip"

echo "正在下载 GeoNames 数据..."
wget -O data/allCountries.zip $DATA_URL
echo "下载完成。如需解压，请运行: unzip data/allCountries.zip -d data/"
