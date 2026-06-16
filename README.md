# ExifWatermark
Add  EXIF watermarks (date, camera, lens, GPS location) to your photos

# how to compile
```shell
cmake --preset release
cmake --build --preset release
```
- go to [baidumap](https://lbsyun.baidu.com/apiconsole/key), apply your access key and corresponding sk
- put your image in your_path, drag the image onto exe, then picture will be generated in current path.
- Positional arguments:
  input              image file path [required]

- Optional arguments:
  -h, --help                shows help message and exits  
  -v, --version             prints version information and exits  
  --verbose                 enable verbose output  
  --reverse_geocode_local   enable local reverse geocode using GeoNames data  
  --reverse_geocode_online  enable baidu map reverse geocode  
  --ak                      baidu map ak, visit https://lbsyun.baidu.com/apiconsole/key [nargs=0..1] [default: ""]  
  --sk                      baidu map sk corresponding to ak [nargs=0..1] [default: ""]  
  --add_logo                enable adding camera logo to image  
  
# reference
![with_exif_logo](./assets/image/dslfinal.jpg)
![with_exif_logo](./assets/image/woodfinal.jpg)

# tips
- chinese characters in path is not allowed