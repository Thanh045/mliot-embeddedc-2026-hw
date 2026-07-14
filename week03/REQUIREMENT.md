# HW03: TỰ ĐỘNG HÓA QUY TRÌNH BUILD VÀ FLASH FIRMWARE QUA DÒNG LỆNH

## 1. MIÊU TẢ BÀI TOÁN

Trong môi trường phát triển phần mềm nhúng chuyên nghiệp hoặc khi triển khai các hệ thống kiểm thử tự động (CI/CD), các kỹ sư nhúng thường rời xa giao diện đồ họa (GUI) của các phần mềm IDE đóng gói sẵn để chuyển sang làm việc hoàn toàn với giao diện dòng lệnh (CLI). Việc này giúp tối ưu hóa tốc độ biên dịch, quản lý dự án độc lập với môi trường và tự động hóa quy trình nạp code xuống phần cứng.

Bài tập này yêu cầu học viên vận dụng kiến thức về luồng biên dịch, cấu hình dự án bằng **CMake**, quản lý build tăng dần bằng **Ninja** và tương tác phần cứng bằng **STM32_Programmer_CLI**. Học viên sẽ xây dựng một kịch bản (Script) tự động hóa hoàn chỉnh để dọn dẹp, cấu hình, biên dịch và nạp firmware cho board mạch STM32 Blue Pill.

---

## 2. YÊU CẦU

Học viên phải hoàn thành **3 nhiệm vụ thực hành và viết script** cùng **6 câu hỏi tự luận mở**.

### Task 1 (Tổng quan Công cụ & Luồng làm việc): Phân tích trình tự và công dụng của Toolchain (Tự luận)

Để xây dựng một file script tự động hóa chính xác, trước hết bạn cần hiểu rõ vai trò và thứ tự xuất hiện của từng công cụ trong chuỗi công cụ (Toolchain).
- **1.1**: Hãy nêu ngắn gọn công dụng chính của 4 công cụ sau trong một dự án nhúng: **CMake**, **Ninja**, **Compiler (arm-none-eabi-gcc)**, và **STM32_Programmer_CLI**.
- **1.2**: Trình bày trình tự logic khi kết hợp 4 công cụ trên để đưa một dòng mã nguồn C mới viết trên máy tính trở thành một chương trình chạy trực tiếp trên chip STM32 (Công cụ nào chạy trước, công cụ nào chạy sau và kết quả của công cụ trước phục vụ gì cho công cụ sau)?

---

### Task 2 (Bản chất Luồng Biên dịch & Tối ưu hóa): Phân bổ vai trò của Linker và Ninja (Tự luận)

- **2.1**: Trong giai đoạn biên dịch, trình biên dịch dịch các file nguồn `.c` thành các file đối tượng mã máy riêng lẻ (`.o`). Lúc này, tại sao chúng ta chưa thể nạp ngay các file `.o` này xuống chip mà bắt buộc phải cần đến công cụ **Linker**? File final định dạng `.bin` do Linker tạo ra có điểm gì khác biệt cốt lõi so với các file `.o` riêng lẻ?
- **2.2**: Khi dự án mở rộng lên hàng trăm file code, tính năng "Build tăng dần" (Incremental Build) của **Ninja** có vai trò như thế nào trong việc tiết kiệm thời gian cho lập trình viên? Dựa vào cơ chế nào mà Ninja biết được một file `.c` vừa mới bị sửa đổi để chỉ biên dịch lại duy nhất file đó thay vì biên dịch lại toàn bộ dự án?

---

### Task 3 (Bản đồ bộ nhớ & Linker Script): Phân tích phân vùng VMA/LMA và Startup Code (Tự luận nâng cao)

Trong file Linker Script `STM32F103C8TX_FLASH.ld`, ta bắt gặp đoạn mã cấu hình vùng nhớ nguồn của các biến toàn cục có khởi tạo giá trị đầu như sau:
```linker-script
_sidata = LOADADDR(.data);
.data :
{
  _sdata = .;
  *(.data*)
  _edata = .;
} >RAM AT> FLASH
```
- **3.1**: Dựa vào kiến thức về Bản đồ bộ nhớ (Memory Layout), hãy giải thích sự khác nhau giữa LMA (Load Memory Address) và VMA (Virtual/Runtime Memory Address) của vùng `.data` qua đoạn script trên. Tại sao phân vùng này cần phải có cả 2 loại địa chỉ?
- **3.2**: Chuyện gì sẽ xảy ra với giá trị của các biến toàn cục này ngay sau khi vi điều khiển thực hiện hành động Reset nếu hàm khởi tạo hệ thống (`Reset_Handler` / Startup code) quên nhiệm vụ sao chép dữ liệu từ `_sidata` đến `_sdata`?
---

### Task 4 (Gõ lệnh thủ công): Khai báo các câu lệnh điều khiển hệ thống

Để chuẩn bị tích hợp vào file tự động hóa, học viên cần xác định chính xác các câu lệnh đơn độc lập trên Terminal:
- **4.1**: Viết câu lệnh **CMake** để cấu hình dự án, ép buộc hệ thống sử dụng bộ sinh (Generator) là **Ninja**, đồng thời chỉ định mọi file trung gian và file đích xuất ra phải nằm trong thư mục có tên là `build`.
- **4.2**: Viết câu lệnh gọi **Ninja** để bắt đầu tiến hành biên dịch (build) dự án sau khi bước cấu hình ở trên đã hoàn thành thành công.

---

### Task 5 (Tự động hóa bằng Script): Xây dựng file Script tự động hóa tích hợp Build & Flash

Học viên tiến hành viết một file kịch bản tự động hóa có tên chính xác là `build_and_flash.bat` (Dành cho hệ điều hành Windows). File script này khi kích hoạt phải thực thi tuần tự các phân đoạn công việc sau:

- **5.1 (Clean)**: Kiểm tra nếu thư mục `build` cũ đang tồn tại trong dự án thì thực hiện xóa hoàn toàn thư mục này để đảm bảo quá trình build sạch từ đầu, tránh xung đột file cũ.
- **5.2 (Configure)**: Thực thi câu lệnh CMake cấu hình hệ thống (đã chuẩn bị ở Task 3.1).
- **5.3 (Build)**: Thực thi câu lệnh Ninja biên dịch mã nguồn (đã chuẩn bị ở Task 3.2) để sinh ra file sản phẩm cuối cùng có tên mặc định là `app_firmware.bin` nằm trong thư mục `build/`.
- **5.4 (Flash & Reset)**: Sử dụng công cụ **STM32_Programmer_CLI** kết nối với mạch nạp ST-Link (giao thức SWD) để nạp trực tiếp file `build/app_firmware.bin` xuống chip STM32 tại địa chỉ bắt đầu của vùng nhớ Flash (`0x08000000`). Yêu cầu bổ sung tham số để chip tự động **Reset** và thực thi chương trình ngay sau khi nạp thành công mà không cần người dùng rút cáp nguồn.

---

## 3. RÀNG BUỘC KỸ THUẬT

  1. Tất cả câu trả lời tự luận của **Task 1, Task 2, Task 3** và câu lệnh đơn ở **Task 4** phải được ghi chung vào duy nhất một file văn bản thuần có tên là `answers.txt`.
  2. Toàn bộ mã kịch bản tự động hóa ở **Task 5** phải được viết vào duy nhất một file script có tên là `build_and_flash.bat`.
* **Ràng buộc file nạp**: Trong mã kịch bản của Task 5, đường dẫn file nhị phân bắt buộc phải chỉ định chính xác tới file `build/app_firmware.bin` và địa chỉ nạp tuyệt đối trên Flash của STM32F103C8T6 là `0x08000000`.

---

## 4. KẾT QUẢ ĐẦU RA MẪU

Học viên có thể đối chiếu cấu trúc lệnh và logic hiển thị của file `build_and_flash.bat` khi chạy trên Terminal để đảm bảo tính đúng đắn:

```text
STEP 1: CLEANING BUILD DIRECTORY
Deleting existing build folder...

STEP 2: CONFIGURING PROJECT WITH CMAKE
-- The C compiler identification is GNU 10.3.1
-- Configuring done
-- Generating done
-- Build files have been written to: C:/Project/build

STEP 3: COMPILING FIRMWARE WITH NINJA
[1/3] Building C object CMakeFiles/app_firmware.dir/main.c.obj
[2/3] Building C object CMakeFiles/app_firmware.dir/stm32f1xx_it.c.obj
[3/3] Linking C executable build/app_firmware.elf
Generating binary flash image: build/app_firmware.bin

STEP 4: FLASHING FIRMWARE TO TARGET MCU
Memory Programming...
Opening and Parsing File: app_firmware.bin
Address       : 0x08000000
Size          : 4232 Bytes

Data flash Progress:
██████████████████████████████████████████████████ 100%

Application running ...
MCU Reset.
```
---

## 5. HƯỚNG DẪN KIỂM TRA BÀI LÀM

Sau khi hoàn thành bài tập, học viên có thể chạy thử với Project mẫu đã được gửi ở buổi học trước đó. Học viên hãy giữ file `build_and_flash.bat` này để sử dung trong quá trình build và flash xuyên suốt khóa học.

Học viên chỉ cần thực hiện các bước sau để nghiệm thu bài tập:
### Bước 1: Thả file build_and_flash.bat của mình vào trực tiếp thư mục Project mẫu đó.
### Bước 2: Kết nối board STM32 Blue Pill với máy tính thông qua mạch nạp ST-Link/V2.
### Bước 3: Nhấp đúp chuột để thực thi file build_and_flash.bat và quan sát hệ thống tự động hóa quy trình cho đến khi đèn LED trên mạch nạp bắt đầu nhấp nháy.

---

# Chúc các bạn làm bài vui vẻ!